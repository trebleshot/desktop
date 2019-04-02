/*
* Copyright (C) 2019 Veli Tasalı, created on 2/16/19
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <QtCore/QFileInfo>
#include <QtWidgets/QMessageBox>
#include <src/util/AppUtils.h>
#include <QtWidgets/QFileDialog>
#include <src/util/TransferUtils.h>
#include <QtGui/QDesktopServices>
#include "ShowTransferDialog.h"
#include "DeviceChooserDialog.h"
#include "TransferRequestProgressDialog.h"
#include "ConnectionReselectForDeviceDialog.h"

ShowTransferDialog::ShowTransferDialog(QWidget *parent, groupid groupId, bool showDeviceSelector)
		: QDialog(parent), m_ui(new Ui::ShowTransferDialog), m_objectModel(new TransferObjectModel(groupId)),
		  m_errorsModel(new FlawedTransferModel(groupId)),
		  m_group(groupId), m_groupInfo()
{
	m_showAddDeviceDialog = showDeviceSelector;

	m_ui->setupUi(this);
	m_ui->errorTreeView->setModel(m_errorsModel);
	m_ui->transfersTreeView->setModel(m_objectModel);
	m_ui->errorTreeView->setColumnWidth(0, 250);
	m_ui->transfersTreeView->setColumnWidth(0, 250);
	m_ui->transfersTreeView->setColumnWidth(1, 80);
	m_ui->transfersTreeView->setColumnWidth(2, 80);

	connect(gTaskMgr, &TransferTaskManager::taskStatus, this, &ShowTransferDialog::globalTaskStatus);
	connect(gTaskMgr, &TransferTaskManager::taskAdded, this, &ShowTransferDialog::globalTaskAdded);
	connect(gTaskMgr, &TransferTaskManager::taskRemoved, this, &ShowTransferDialog::globalTaskRemoved);
	connect(gDatabase, &AccessDatabase::databaseChanged, this, &ShowTransferDialog::checkGroupIntegrity);
	connect(m_ui->retryReceivingButton, &QPushButton::pressed, this, &ShowTransferDialog::retryReceiving);
	connect(m_ui->transfersTreeView, &QTreeView::activated, this, &ShowTransferDialog::transferItemActivated);
	connect(m_ui->assigneesComboBox, SIGNAL(activated(int)), this, SLOT(assigneeChanged(int)));
	connect(m_ui->startButton, &QPushButton::pressed, this, &ShowTransferDialog::startTransfer);
	connect(m_ui->showFilesButton, &QPushButton::pressed, this, &ShowTransferDialog::showFiles);
	connect(m_ui->saveDirectoryButton, &QPushButton::pressed, this, &ShowTransferDialog::saveDirectory);
	connect(m_ui->removeButton, &QPushButton::pressed, this, &ShowTransferDialog::removeTransfer);
	connect(m_ui->chooseDirectoryButton, &QPushButton::pressed, this, &ShowTransferDialog::changeSavePath);
	connect(m_ui->addAndChConnectionButton, &QPushButton::pressed, this, &ShowTransferDialog::addDevOrChangeConnection);
	connect(m_objectModel, &QAbstractTableModel::layoutChanged, this, &ShowTransferDialog::updateStats);

	checkGroupIntegrity(SqlSelection(), ChangeType::Any);
}

ShowTransferDialog::~ShowTransferDialog()
{
	delete m_ui;
	delete m_objectModel;
	delete m_errorsModel;
}

void ShowTransferDialog::changeSavePath()
{
	auto *fileDialog = new QFileDialog();

	fileDialog->setWindowTitle(tr("Choose the folder where the files will be put"));
	fileDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
	fileDialog->setDirectory(TransferUtils::getSavePath(m_group));
	fileDialog->setFileMode(QFileDialog::FileMode::DirectoryOnly);
	fileDialog->show();

	connect(fileDialog, &QDialog::accepted, [this, fileDialog]() {
		m_group.savePath = fileDialog->directory().path();
		m_ui->storageLineEdit->setText(m_group.savePath);
		gDatabase->update(m_group);
	});

	connect(fileDialog, &QDialog::finished, fileDialog, &QObject::deleteLater);
}

void ShowTransferDialog::connectionChanged(const QString &adapterName)
{
	if (m_groupInfo.assignees.empty())
		return;

	auto assignee = m_groupInfo.assignees[0].assignee;
	assignee.connectionAdapter = adapterName;

	gDatabase->update(assignee);
}

void ShowTransferDialog::saveDirectory()
{
	const auto &text = m_ui->storageLineEdit->text();

	if (text.isEmpty() || !QFileInfo::exists(text)) {
		QMessageBox box;
		box.setWindowTitle(tr("Error"));
		box.setText(tr("The entered path cannot be used."));
		box.exec();
	}

	m_group.savePath = text;
	gDatabase->update(m_group);
}

void ShowTransferDialog::checkGroupIntegrity(const SqlSelection &change, ChangeType type)
{
	if (!change.valid() || change.tableName == DB_TABLE_TRANSFERASSIGNEE) {
		qDebug() << this << "Refreshing assignee list";

		m_groupInfo.assignees.clear();
		TransferUtils::getAllAssigneeInfo(m_group, m_groupInfo.assignees);
		updateAssignees();
	}

	if (!change.valid() || change.tableName == DB_TABLE_TRANSFERGROUP) {
		if (type == ChangeType::Update || type == ChangeType::Any)
			m_ui->storageLineEdit->setText(TransferUtils::getSavePath(m_group));

		if (type == ChangeType::Delete || type == ChangeType::Insert || type == ChangeType::Any) {
			if (!gDatabase->reconstructSilently(m_group)) {
				close();
				return;
			}

			m_groupInfo = TransferUtils::getInfo(m_group);

			updateStats();
			updateButtons();
		}
	}
}

void ShowTransferDialog::updateAssignees()
{
	m_ui->assigneesComboBox->clear();

	if (m_groupInfo.assignees.size() > 1)
		m_ui->assigneesComboBox->addItem(tr("All"), QString());

	for (const auto &info : m_groupInfo.assignees)
		m_ui->assigneesComboBox->addItem(info.device.nickname, info.device.id);

	assigneeChanged(0);
}

void ShowTransferDialog::updateButtons()
{
	bool hasRunning = gTaskMgr->hasActiveTasksFor(m_group.id, m_objectModel->m_deviceId);

	m_ui->startButton->setEnabled(m_groupInfo.hasIncoming || hasRunning);
	m_ui->startButton->setText(hasRunning ? tr("Pause") : tr("Start"));
	m_ui->saveDirectoryButton->setEnabled(m_groupInfo.hasIncoming);
	m_ui->storageLineEdit->setEnabled(m_groupInfo.hasIncoming);
	m_ui->storageText->setEnabled(m_groupInfo.hasIncoming);
	m_ui->showFilesButton->setEnabled(m_groupInfo.hasIncoming);
	m_ui->chooseDirectoryButton->setEnabled(m_groupInfo.hasIncoming);
	m_ui->noIncomingFileText->setVisible(!m_groupInfo.hasIncoming);
	m_ui->retryReceivingButton->setEnabled(m_groupInfo.hasIncoming);
	m_ui->addAndChConnectionButton->setText(m_groupInfo.hasOutgoing ? tr("Add devices") : tr("Change connection"));

	if (!hasRunning && m_ongoingTaskInfo.object.id != 0) {
		m_ongoingTaskInfo.reset();
		updateStats();
	}
}

void ShowTransferDialog::addDevOrChangeConnection()
{
	if (m_groupInfo.hasOutgoing) {
		auto *dialog = new DeviceChooserDialog(this, m_group.id);
		connect(dialog, &DeviceChooserDialog::devicesSelected, this, &ShowTransferDialog::sendToDevices);
		connect(dialog, &QDialog::finished, dialog, &QObject::deleteLater);
		dialog->show();
	} else if (!m_groupInfo.assignees.empty()) {
		auto *dialog = new ConnectionReselectForDeviceDialog(this, m_groupInfo.assignees[0].device.id);

		connect(dialog, &QDialog::finished, dialog, &QObject::deleteLater);
		connect(dialog, &ConnectionReselectForDeviceDialog::connectionSelected,
		        this, &ShowTransferDialog::connectionChanged);

		dialog->show();
	}
}

void ShowTransferDialog::sendToDevices(groupid groupId, const QList<NetworkDevice> &devices)
{
	auto *dialog = new TransferRequestProgressDialog(this, groupId, devices);
	connect(dialog, &QDialog::finished, dialog, &QObject::deleteLater);
	dialog->show();
}

void ShowTransferDialog::removeTransfer()
{
	const auto &removedDevice = m_ui->assigneesComboBox->currentText();
	const auto &removedDeviceId = m_ui->assigneesComboBox->currentData().toString();
	const auto removeAssignee = m_ui->assigneesComboBox->currentIndex() > 0 && !m_groupInfo.hasIncoming;
	auto *messageBox = new QMessageBox(this);

	messageBox->setWindowTitle(tr("Remove"));
	messageBox->setText(removeAssignee ? tr("Remove %1 from this transfer?").arg(removedDevice)
	                                   : tr("Remove this transfer altogether?"));

	messageBox->addButton(QMessageBox::StandardButton::Cancel);
	auto *button = messageBox->addButton(QMessageBox::StandardButton::Ok);

	connect(button, &QPushButton::pressed, [this, removeAssignee, removedDeviceId]() {
		if (removeAssignee) {
			SqlSelection selection;
			selection.setTableName(DB_TABLE_TRANSFERASSIGNEE);
			selection.setWhere(QString("%1 = ? AND %2 = ?")
					                   .arg(DB_FIELD_TRANSFERASSIGNEE_DEVICEID)
					                   .arg(DB_FIELD_TRANSFERASSIGNEE_GROUPID));
			selection.whereArgs << removedDeviceId
			                    << m_group.id;

			gDatabase->removeAsObject(selection, TransferAssignee());
			return;
		}

		gDatabase->remove(m_group);
	});

	connect(messageBox, &QMessageBox::finished, messageBox, &QObject::deleteLater);
	messageBox->show();
}

void ShowTransferDialog::startTransfer()
{
	taskToggle();
}

void ShowTransferDialog::assigneeChanged(int index)
{
	m_ongoingTaskInfo.reset();

	m_objectModel->setDeviceId(m_ui->assigneesComboBox->itemData(index).toString());
	m_objectModel->databaseChanged(SqlSelection(), ChangeType::Any);

	updateButtons();
}

void ShowTransferDialog::globalTaskAdded(groupid groupId, const QString &deviceId, int type)
{
	updateButtons();
}

void ShowTransferDialog::globalTaskRemoved(groupid groupId, const QString &deviceId, int type)
{
	updateButtons();
}

void ShowTransferDialog::globalTaskStatus(groupid groupId, const QString &deviceId, int type, qint64 completed,
                                          const TransferObject &object)
{
	if (m_objectModel->m_groupId == groupId
	    && (m_objectModel->m_deviceId.isNull() || m_objectModel->m_deviceId == deviceId)) {
		m_ongoingTaskInfo.completedBytes = completed;
		m_ongoingTaskInfo.object = object;

		updateStats();
	}
}

void ShowTransferDialog::taskToggle()
{
	if (gTaskMgr->hasActiveTasksFor(m_group.id, m_objectModel->m_deviceId))
		gTaskMgr->pauseTasks(m_group.id, m_objectModel->m_deviceId);
	else if (m_groupInfo.hasIncoming && !m_groupInfo.assignees.empty())
		TransferUtils::startTransfer(m_group.id, m_groupInfo.assignees[0].device.id);
}

void ShowTransferDialog::showFiles()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(TransferUtils::getSavePath(m_group)));
}

void ShowTransferDialog::updateStats()
{
	{
		MutexEnablingScope mutexScope(m_objectModel);
		TransferUtils::getInfo(m_groupInfo, *m_objectModel->list(), true);
	}

	m_ui->progressBar->setValue((int) (((double) m_groupInfo.completedBytes / m_groupInfo.totalBytes) * 100));
	m_ui->textFilesLeft->setText(tr("%1 of %2").arg(m_groupInfo.completed).arg(m_groupInfo.total));

	if (m_ui->progressBar->value() == 100) {
		m_ui->transferObjectTextView->setText(tr("Completed"));
		m_ui->progressBarPerFile->setValue(100);
	} else {
		if (m_ongoingTaskInfo.object.id == 0)
			m_ui->transferObjectTextView->setText(tr("Paused"));
		else {
			m_ui->transferObjectTextView->setText(m_ongoingTaskInfo.object.friendlyName);
			m_ui->progressBarPerFile->setValue((int) (((double) m_ongoingTaskInfo.completedBytes
			                                           / m_ongoingTaskInfo.object.fileSize) * 100));
			m_groupInfo.completedBytes += m_ongoingTaskInfo.completedBytes;
		}
	}
}

void ShowTransferDialog::transferItemActivated(const QModelIndex &modelIndex)
{
	if (!modelIndex.isValid())
		return;

	auto item = m_objectModel->list()->at(modelIndex.row());

	if (modelIndex.column() == TransferObjectModel::Status && item.flag != TransferObject::Done
	    && item.flag != TransferObject::Removed) {
		item.flag = TransferObject::Pending;
		gDatabase->update(item);
	} else if (item.flag == TransferObject::Flag::Done && item.type == TransferObject::Type::Incoming)
		QDesktopServices::openUrl(QUrl::fromLocalFile(TransferUtils::getIncomingFilePath(m_group, item)));
	else if (item.type == TransferObject::Type::Outgoing && QFile::exists(item.file))
		QDesktopServices::openUrl(QUrl::fromLocalFile(item.file));
}

void ShowTransferDialog::retryReceiving()
{
	SqlSelection selection;
	selection.setTableName(DB_TABLE_TRANSFER);
	selection.setWhere(QString("%1 = ? AND %2 = ?").arg(DB_FIELD_TRANSFER_GROUPID).arg(DB_FIELD_TRANSFER_FLAG));
	selection.whereArgs << m_group.id
	                    << TransferObject::Flag::Interrupted;

	gDatabase->update(selection, DbObjectMap{
			{DB_FIELD_TRANSFER_FLAG, TransferObject::Flag::Pending}
	});
}

void ShowTransferDialog::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);

	if (m_showAddDeviceDialog) {
		addDevOrChangeConnection();
		m_showAddDeviceDialog = false;
	}
}
