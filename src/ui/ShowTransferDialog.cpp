//
// Created by veli on 2/16/19.
//

#include <QtCore/QFileInfo>
#include <QtWidgets/QMessageBox>
#include <src/util/AppUtils.h>
#include <QtWidgets/QFileDialog>
#include <src/util/TransferUtils.h>
#include <QtGui/QDesktopServices>
#include "ShowTransferDialog.h"
#include "DeviceChooserDialog.h"
#include "TransferRequestProgressDialog.h"

ShowTransferDialog::ShowTransferDialog(QWidget *parent, groupid groupId)
	: QDialog(parent), m_ui(new Ui::ShowTransferDialog), m_objectModel(new TransferObjectModel(groupId)),
	m_errorsModel(new FlawedTransferModel(groupId)), m_group(groupId), m_groupInfo()
{
	m_ui->setupUi(this);
	m_ui->errorTreeView->setModel(m_errorsModel);
	m_ui->transfersTreeView->setModel(m_objectModel);
	m_ui->errorTreeView->setColumnWidth(0, 250);
	m_ui->transfersTreeView->setColumnWidth(0, 250);
	m_ui->transfersTreeView->setColumnWidth(1, 80);
	m_ui->transfersTreeView->setColumnWidth(2, 80);

	connect(gTaskMgr, &TransferTaskManager::taskAdded, this, &ShowTransferDialog::globalTaskStarted);
	connect(gTaskMgr, &TransferTaskManager::taskRemoved, this, &ShowTransferDialog::globalTaskFinished);
	connect(gTaskMgr, &TransferTaskManager::taskItemTransferred, this, &ShowTransferDialog::transferFileChange);
	connect(gTaskMgr, &TransferTaskManager::taskByteTransferred, this, &ShowTransferDialog::transferBitChange);
	connect(gDatabase, &AccessDatabase::databaseChanged, this, &ShowTransferDialog::checkGroupIntegrity);
	connect(m_ui->retryReceivingButton, &QPushButton::pressed, this, &ShowTransferDialog::retryReceiving);
	connect(m_ui->transfersTreeView, &QTreeView::activated, this, &ShowTransferDialog::transferItemActivated);
	connect(m_ui->assigneesComboBox, SIGNAL(activated(int)), this, SLOT(assigneeChanged(int)));
	connect(m_ui->startButton, &QPushButton::pressed, this, &ShowTransferDialog::startTransfer);
	connect(m_ui->showFilesButton, &QPushButton::pressed, this, &ShowTransferDialog::showFiles);
	connect(m_ui->saveDirectoryButton, &QPushButton::pressed, this, &ShowTransferDialog::saveDirectory);
	connect(m_ui->removeButton, &QPushButton::pressed, this, &ShowTransferDialog::removeTransfer);
	connect(m_ui->chooseDirectoryButton, &QPushButton::pressed, this, &ShowTransferDialog::changeSavePath);
	connect(m_ui->addDevicesButton, &QPushButton::pressed, this, &ShowTransferDialog::addDevices);

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

	fileDialog->setWindowTitle("Choose a folder where the files will be put");
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

void ShowTransferDialog::saveDirectory()
{
	const auto &text = m_ui->storageLineEdit->text();

	if (text.isEmpty() || !QFileInfo::exists(text)) {
		QMessageBox box;
		box.setWindowTitle("Error");
		box.setText("The entered path cannot be used.");
		box.exec();
	}

	m_group.savePath = text;
	gDatabase->update(m_group);
}

void ShowTransferDialog::checkGroupIntegrity(const SqlSelection &change, ChangeType type)
{
	if (!change.valid() || change.tableName == DB_TABLE_TRANSFERASSIGNEE) {
		m_assigneeList.clear();
		TransferUtils::getAllAssigneeInfo(m_group, m_assigneeList);
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
	const QList<AssigneeInfo> &copyList = m_assigneeList;
	m_ui->assigneesComboBox->clear();
	m_ui->assigneesComboBox->addItem(tr("Any"), QString());

	for (const auto &info : copyList)
		m_ui->assigneesComboBox->addItem(info.device.nickname, info.device.id);

	assigneeChanged(0);
}

void ShowTransferDialog::updateButtons()
{
	bool hasRunning = gTaskMgr->hasActiveTasksFor(m_group.id);

	m_ui->startButton->setEnabled(m_groupInfo.hasIncoming || hasRunning);
	m_ui->startButton->setText(hasRunning ? "Pause" : "Start");
	m_ui->addDevicesButton->setEnabled(m_groupInfo.hasOutgoing);
	m_ui->saveDirectoryButton->setEnabled(m_groupInfo.hasIncoming);
	m_ui->storageLineEdit->setEnabled(m_groupInfo.hasIncoming);
	m_ui->storageText->setEnabled(m_groupInfo.hasIncoming);
	m_ui->showFilesButton->setEnabled(m_groupInfo.hasIncoming);
	m_ui->chooseDirectoryButton->setEnabled(m_groupInfo.hasIncoming);
	m_ui->noIncomingFileText->setVisible(!m_groupInfo.hasIncoming);
	m_ui->retryReceivingButton->setEnabled(m_groupInfo.hasIncoming);
}

void ShowTransferDialog::addDevices()
{
	auto *dialog = new DeviceChooserDialog(this, m_group.id);
	connect(dialog, &DeviceChooserDialog::devicesSelected, this, &ShowTransferDialog::sendToDevices);
	connect(dialog, &QDialog::finished, dialog, &QObject::deleteLater);
	dialog->show();
}

void ShowTransferDialog::sendToDevices(groupid groupId, QList<NetworkDevice> devices)
{
	auto* dialog = new TransferRequestProgressDialog(this, groupId, devices);
	connect(dialog, &QDialog::finished, dialog, &QObject::deleteLater);
	dialog->show();
}

void ShowTransferDialog::removeTransfer()
{
	if (m_ui->assigneesComboBox->currentIndex() > 0 && !m_groupInfo.hasIncoming) {
		SqlSelection selection;
		selection.setTableName(DB_TABLE_TRANSFERASSIGNEE);
		selection.setWhere(QString("%1 = ? AND %2 = ?")
			.arg(DB_FIELD_TRANSFERASSIGNEE_DEVICEID)
			.arg(DB_FIELD_TRANSFERASSIGNEE_GROUPID));
		selection.whereArgs << m_ui->assigneesComboBox->currentData().toString()
			<< m_group.id;

		gDatabase->removeAsObject(selection, TransferAssignee());
		return;
	}

	gDatabase->remove(m_group);
}

void ShowTransferDialog::startTransfer()
{
	taskToggle();
}

void ShowTransferDialog::assigneeChanged(int index)
{
	m_objectModel->setDeviceId(m_ui->assigneesComboBox->itemData(index).toString());
	m_objectModel->databaseChanged(SqlSelection(), ChangeType::Any);
}

void ShowTransferDialog::globalTaskStarted(groupid groupId, const QString &deviceId, int type)
{
	updateButtons();
}

void ShowTransferDialog::globalTaskFinished(groupid groupId, const QString &deviceId, int type)
{
	updateButtons();
}

void ShowTransferDialog::taskToggle()
{
	if (gTaskMgr->hasActiveTasksFor(m_group.id))
		gTaskMgr->pauseTasks(m_group.id);
	else if (m_groupInfo.hasIncoming && !m_assigneeList.empty())
		TransferUtils::startTransfer(m_group.id, m_assigneeList[0].device.id);
}

void ShowTransferDialog::showFiles()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(TransferUtils::getSavePath(m_group)));
}

void ShowTransferDialog::updateStats()
{
	m_ui->progressBar->setMaximum(100);
	m_ui->progressBar->setValue((int)(((double)m_groupInfo.completedBytes / m_groupInfo.totalBytes) * 100));
	m_ui->textFilesLeft->setText(tr("%1 of %2").arg(m_groupInfo.completed).arg(m_groupInfo.total));
}

void ShowTransferDialog::transferBitChange(groupid groupId, const QString &deviceId, int type, qint64 bit, qint64 file)
{
	if (groupId == m_group.id) {
		if (m_fileSessionSize == 0) {
			m_groupInfo.completedBytes += file;
			m_fileSessionSize = file;
		}
		else
			m_groupInfo.completedBytes += bit;
	}

	updateStats();
}

void ShowTransferDialog::transferFileChange(groupid groupId, const QString &deviceId, int type)
{
	if (groupId == m_group.id) {
		m_groupInfo.completed++;
		m_fileSessionSize = 0;
	}

	updateStats();
}

void ShowTransferDialog::transferItemActivated(const QModelIndex &modelIndex)
{
	if (!modelIndex.isValid())
		return;

	auto item = m_objectModel->list().at(modelIndex.row());

	if (modelIndex.column() == TransferObjectModel::Status && item.flag != TransferObject::Done
		&& item.flag != TransferObject::Removed) {
		item.flag = TransferObject::Pending;
		gDatabase->update(item);
	}
	else if (item.flag == TransferObject::Flag::Done && item.type == TransferObject::Type::Incoming)
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
