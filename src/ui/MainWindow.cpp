#include <QtGui/QDesktopServices>
#include <src/broadcast/SeamlessClient.h>
#include <QtWidgets/QFileDialog>
#include <src/util/ViewUtils.h>
#include "MainWindow.h"
#include "ManageDevicesDialog.h"
#include "ShowTransferDialog.h"
#include "DeviceChooserDialog.h"
#include "FileAdditionProgressDialog.h"
#include "TransferRequestProgressDialog.h"
#include "AboutDialog.h"

MainWindow::MainWindow(QWidget *parent)
		: QMainWindow(parent), m_ui(new Ui::MainWindow),
		  m_seamlessServer(new SeamlessServer), m_commServer(new CommunicationServer),
		  m_groupModel(new TransferGroupModel()), m_deviceModel(new NetworkDeviceModel),
		  m_discoveryService(new DNSSDService)
{
	m_ui->setupUi(this);

	setAcceptDrops(true);
	setWindowTitle(tr("Home"));

	if (AppUtils::getDatabase() == nullptr) {
		auto *error = new QMessageBox(this);

		error->setWindowTitle(tr("Database error"));
		error->setText(tr("The database used to store information did not open. Refer to the development notes. "
		                  "The program will force close."));

		error->show();

		connect(error, &QMessageBox::finished, this, &MainWindow::close);
	} else {
		connect(m_commServer, &CommunicationServer::textReceived, this, &MainWindow::showReceivedText);
		connect(m_commServer, &CommunicationServer::transferRequest, this, &MainWindow::showTransferRequest);
		connect(m_commServer, &CommunicationServer::deviceBlocked, this, &MainWindow::deviceBlocked);

		if (!m_commServer->start() || !m_seamlessServer->start()) {
			auto *error = new QMessageBox(this);
			error->setWindowTitle(tr("Server error"));
			error->setText(tr("TrebleShot server has returned with an error. "
			                  "Try restarting the application to solve the problem."));
			connect(this, &MainWindow::destroyed, error, &QObject::deleteLater);
			error->show();
		}

		m_discoveryService->start();
		m_ui->transfersTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
		m_ui->transfersTreeView->setModel(m_groupModel);
		m_ui->transfersTreeView->setColumnWidth(0, 160);
		m_ui->transfersTreeView->setColumnWidth(1, 80);
		m_ui->transfersTreeView->setColumnWidth(2, 80);
		m_ui->transfersTreeView->setColumnWidth(3, 80);
		m_ui->devicesTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
		m_ui->devicesTreeView->setModel(m_deviceModel);
		m_ui->devicesTreeView->setColumnWidth(0, 160);

		connect(m_ui->startPauseButton, &QPushButton::pressed, this, &MainWindow::taskToggle);
		connect(m_ui->showButton, SIGNAL(pressed()), this, SLOT(showTransfer()));
		connect(m_ui->removeButton, &QPushButton::pressed, this, &MainWindow::removeTransfer);
		connect(m_ui->devicesTreeView, &QTreeView::customContextMenuRequested, this,
		        &MainWindow::deviceContextMenu);
		connect(m_ui->devicesTreeView, &QTreeView::activated, this, &MainWindow::deviceSelected);
		connect(m_ui->transfersTreeView, &QTreeView::customContextMenuRequested, this,
		        &MainWindow::transferContextMenu);
		connect(m_ui->transfersTreeView, &QTreeView::activated, this, &MainWindow::transferItemActivated);
		connect(m_ui->transfersTreeView->selectionModel(), &QItemSelectionModel::selectionChanged,
		        this, &MainWindow::transferSelectionChanged);
		connect(m_ui->actionPause, &QAction::triggered, this, &MainWindow::taskPause);
		connect(m_ui->actionStart, &QAction::triggered, this, &MainWindow::taskStart);
		connect(m_ui->sendFilesButton, &QPushButton::clicked, this, &MainWindow::selectFilesToSend);
		connect(m_ui->actionAbout_TrebleShot, &QAction::triggered, this, &MainWindow::about);
		connect(m_ui->actionAbout_Qt, &QAction::triggered, this, &MainWindow::aboutQt);
		connect(m_ui->actionStorage_location, &QAction::triggered, this, &MainWindow::setStorageLocation);
		connect(m_ui->changeStorageFolderButton, &QPushButton::clicked, this, &MainWindow::setStorageLocation);
		connect(m_ui->actionShow_received_files, &QAction::triggered, this, &MainWindow::showReceivedFiles);
		connect(m_ui->usernameLineEdit, &QLineEdit::editingFinished, this, &MainWindow::usernameChanged);
		connect(m_ui->saveStorageButton, &QPushButton::clicked, this, &MainWindow::savePathChanged);

		refreshStorageLocation();
		transferSelectionChanged(QItemSelection(), QItemSelection());
		m_ui->usernameLineEdit->setText(getUserNickname());
	}

	const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
	move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
}

MainWindow::~MainWindow()
{
	delete m_ui;
	delete m_groupModel;
	delete m_deviceModel;
	delete m_commServer;
	delete m_discoveryService;
	delete gTaskMgr;
	delete gDbSignal;
	delete gDatabase;
}


void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasUrls())
		event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasUrls()) {
		event->acceptProposedAction();

		QList<QString> paths;
		TransferUtils::getPaths(event->mimeData()->urls(), paths);

		FileAdditionProgressDialog progressDialog(this, paths);
		connect(&progressDialog, SIGNAL(filesAdded(groupid)), this, SLOT(showTransferWithAddDevicesDialog(groupid)));
		progressDialog.exec();
	}
}

void MainWindow::transferItemActivated(const QModelIndex &modelIndex)
{
	TransferGroupInfo info;

	{
		MutexEnablingScope mutexScope(m_groupModel);
		info = m_groupModel->list()->at(modelIndex.row());
	}

	if (info.group.id != 0)
		showTransfer(info.group.id);
}

void MainWindow::about()
{
	auto *about = new AboutDialog(this);
	connect(about, &QDialog::finished, about, &QObject::deleteLater);
	about->show();
}

void MainWindow::aboutQt()
{
	QApplication::aboutQt();
}

void MainWindow::showTransferWithAddDevicesDialog(groupid groupId)
{
	showTransfer(groupId, true);
}

void MainWindow::showTransfer(groupid groupId, bool showAddDeviceDialog)
{
	ShowTransferDialog(this, groupId, showAddDeviceDialog).exec();
}

void MainWindow::showReceivedText(const QString &text, const QString &deviceId)
{
	NetworkDevice device(deviceId);

	try {
		gDatabase->reconstruct(device);

		QMessageBox messageBox(this);

		messageBox.setWindowTitle(tr("Text received from %1").arg(device.nickname));
		messageBox.setText(text);
		messageBox.addButton(QMessageBox::StandardButton::Close);

		QPushButton *buttonCopy = messageBox.addButton(tr("Copy to clipboard"), QMessageBox::ButtonRole::ActionRole);

		connect(buttonCopy, &QPushButton::pressed, [text]() {
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(text);
		});

		messageBox.exec();
	}
	catch (...) {
		// do nothing
	}
}

void MainWindow::showTransferRequest(const QString &deviceId, groupid groupId, int filesTotal)
{
	NetworkDevice device(deviceId);

	try {
		gDatabase->reconstruct(device);

		QMessageBox messageBox(this);

		messageBox.setWindowTitle(QString("%1").arg(device.nickname));
		messageBox.setText(tr("Receive files from %1, %2 in total?")
				                   .arg(device.nickname)
				                   .arg(filesTotal));
		auto *okButton = messageBox.addButton(QMessageBox::StandardButton::Yes);
		auto *noButton = messageBox.addButton(QMessageBox::StandardButton::No);
		messageBox.addButton(QMessageBox::StandardButton::Close);

		connect(okButton, &QPushButton::pressed, [groupId, deviceId]() {
			TransferUtils::startTransfer(groupId, deviceId);
		});

		messageBox.exec();

		auto *clickedButton = messageBox.clickedButton();
		auto accepted = clickedButton != noButton;

		GThread::startIndependent([groupId, deviceId, accepted](GThread *thread) {
			TransferGroup group(groupId);
			NetworkDevice copyDevice(deviceId);
			TransferAssignee assignee(groupId, deviceId);

			if (gDbSignal->reconstruct(group) && gDbSignal->reconstruct(copyDevice)
			    && gDbSignal->reconstruct(assignee)) {
				if (!accepted)
					gDbSignal->remove(group);

				DeviceConnection connection(deviceId, assignee.connectionAdapter);

				if (gDbSignal->reconstruct(connection)) {
					CommunicationBridge bridge;

					try {
						auto *activeConnection = bridge.communicate(copyDevice, connection);

						activeConnection->reply({
								                        {KEYWORD_REQUEST, KEYWORD_REQUEST_RESPONSE},
								                        {KEYWORD_TRANSFER_GROUP_ID,    QVariant(groupId).toLongLong()},
								                        {KEYWORD_TRANSFER_IS_ACCEPTED, accepted}
						                        });

						activeConnection->receive();
					}
					catch (...) {
						qDebug() << thread << "Response failed";
					}
				}
			} else
				qDebug() << thread << "Reconstruction failed";
		});

		if (okButton == clickedButton)
			showTransfer(groupId);
	}
	catch (...) {
		// do nothing
	}
}

void MainWindow::showTransfer()
{
	QList<TransferGroupInfo> resultList;

	if (ViewUtils::gatherSelections(m_ui->transfersTreeView->selectionModel(), m_groupModel, resultList))
		showTransfer(resultList[0].group.id);
}

void MainWindow::removeTransfer()
{
	QList<TransferGroupInfo> resultList;

	if (ViewUtils::gatherSelections(m_ui->transfersTreeView->selectionModel(), m_groupModel, resultList))
		for (auto &thisObject : resultList)
			gDatabase->remove(thisObject.group);
}

void MainWindow::refreshStorageLocation()
{
	m_ui->storageFolderLineEdit->setText(TransferUtils::getDefaultSavePath());
}

void MainWindow::setStorageLocation()
{
	auto *fileDialog = new QFileDialog();

	fileDialog->setWindowTitle(tr("Choose a folder where files will be put"));
	fileDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
	fileDialog->setDirectory(TransferUtils::getDefaultSavePath());
	fileDialog->setFileMode(QFileDialog::FileMode::DirectoryOnly);
	fileDialog->show();

	connect(fileDialog, &QDialog::accepted, [fileDialog]() {
		AppUtils::getDefaultSettings().setValue("savePath", fileDialog->directory().path());
	});

	connect(fileDialog, &QDialog::accepted, this, &MainWindow::refreshStorageLocation);
	connect(fileDialog, &QDialog::finished, fileDialog, &QObject::deleteLater);
}

void MainWindow::deviceBlocked(const QString &deviceId, const QHostAddress &address)
{
	NetworkDevice device(deviceId);

	if (gDatabase->reconstructSilently(device)) {
		QMessageBox box(this);
		box.setWindowTitle(device.nickname);
		box.setText(tr("Restricted %1 is trying to communicate with you?").arg(device.nickname));
		box.addButton(QMessageBox::StandardButton::Ignore);
		QPushButton *allowButton = box.addButton(tr("Allow"), QMessageBox::ButtonRole::AcceptRole);
		QPushButton *blockButton = box.addButton(tr("Deny for this session"), QMessageBox::ButtonRole::RejectRole);

		connect(allowButton, &QPushButton::pressed, [&device]() {
			device.isRestricted = false;
			gDatabase->publish(device);
		});

		connect(blockButton, &QPushButton::pressed, [this, address]() {
			m_commServer->blockAddress(address);
		});

		box.exec();
	}
}

void MainWindow::usernameChanged()
{
	const auto &username = m_ui->usernameLineEdit->text();

	if (!username.isEmpty())
		AppUtils::getDefaultSettings().setValue("nickname", username);
}

void MainWindow::savePathChanged()
{
	const auto &path = m_ui->storageFolderLineEdit->text();

	if (!path.isEmpty() && QFile::exists(path))
		AppUtils::getDefaultSettings().setValue("savePath", path);
	else {
		QMessageBox box;
		box.setWindowTitle(tr("Inappropriate folder path"));
		box.setText(tr("Non-existing folders cannot be set"));
		box.exec();
	}
}

void MainWindow::deviceContextMenu(const QPoint &point)
{
	QList<NetworkDevice> resultList;

	if (ViewUtils::gatherSelections(m_ui->devicesTreeView->selectionModel(), m_deviceModel, resultList)) {
		QMenu menu(m_ui->devicesTreeView);
		NetworkDevice device = resultList[0];

		menu.addAction(device.isTrusted ? tr("Remove from TrustZone") : tr("Set as TrustZone"), [&device]() {
			device.isTrusted = !device.isTrusted;
			gDatabase->publish(device);
		});

		menu.addAction(device.isRestricted ? tr("Allow to access") : tr("Restrict"), [&device]() {
			device.isRestricted = !device.isRestricted;
			gDatabase->publish(device);
		});
		menu.addAction(tr("Remove"), [&device]() { gDatabase->remove(device); });
		menu.exec(m_ui->devicesTreeView->mapToGlobal(point));
	}
}

void MainWindow::deviceSelected(const QModelIndex &modelIndex)
{
	if (modelIndex.isValid()) {
		NetworkDevice device;

		{
			MutexEnablingScope mutexScope(m_deviceModel);
			device = m_deviceModel->list()->at(modelIndex.row());
		}

		if (!device.id.isEmpty()) {
			if (modelIndex.column() == NetworkDeviceModel::Status) {
				device.isRestricted = !device.isRestricted;
				gDatabase->publish(device);
			} else if (modelIndex.column() == NetworkDeviceModel::TrustZone) {
				device.isTrusted = !device.isTrusted;
				gDatabase->publish(device);
			}
		}
	}
}

void MainWindow::selectFilesToSend()
{
	auto *fileDialog = new QFileDialog();

	fileDialog->setWindowTitle(tr("Choose files to send"));
	fileDialog->setAcceptMode(QFileDialog::AcceptMode::AcceptOpen);
	fileDialog->setFileMode(QFileDialog::FileMode::ExistingFiles);
	fileDialog->show();

	connect(fileDialog, &QDialog::accepted, [this, fileDialog]() {
		auto *progressDialog = new FileAdditionProgressDialog(this, fileDialog->selectedFiles());
		progressDialog->show();

		connect(progressDialog, SIGNAL(filesAdded(groupid)), this, SLOT(showTransfer(groupid)));
		connect(progressDialog, &QDialog::finished, progressDialog, &QObject::deleteLater);
	});

	connect(fileDialog, &QDialog::finished, fileDialog, &QObject::deleteLater);
}

void MainWindow::taskStart()
{
	QList<TransferGroupInfo> resultList;

	if (ViewUtils::gatherSelections(m_ui->transfersTreeView->selectionModel(), m_groupModel, resultList))
		for (auto &thisObject : resultList)
			if (thisObject.hasIncoming && !thisObject.assignees.empty())
				TransferUtils::startTransfer(thisObject.group.id, thisObject.assignees[0].device.id);
}

void MainWindow::taskPause()
{
	QList<TransferGroupInfo> resultList;

	if (ViewUtils::gatherSelections(m_ui->transfersTreeView->selectionModel(), m_groupModel, resultList))
		for (auto &thisObject : resultList)
			gTaskMgr->pauseTasks(thisObject.group.id);
}

void MainWindow::transferSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
	updateButtons();
}

void MainWindow::transferContextMenu(const QPoint &point)
{
	QMenu menu(m_ui->transfersTreeView);

	// The buttons that the actions are based on are updated when the user changes the list selection.
	// Triggering another update for this sequence is arbitrary.

	{
		menu.addAction(m_ui->startPauseButton->text(), this, &MainWindow::taskToggle)
				->setEnabled(m_ui->startPauseButton->isEnabled());
		menu.addSeparator();
	}

	{
		menu.addAction(m_ui->showButton->text(), this, SLOT(showTransfer()))
				->setEnabled(m_ui->showButton->isEnabled());
		menu.addAction(m_ui->removeButton->text(), this, &MainWindow::removeTransfer)
				->setEnabled(m_ui->removeButton->isEnabled());
	}

	menu.exec(m_ui->transfersTreeView->viewport()->mapToGlobal(point));
}

void MainWindow::showReceivedFiles()
{
	QDesktopServices::openUrl(QUrl::fromLocalFile(TransferUtils::getDefaultSavePath()));
}

void MainWindow::taskToggle()
{
	QList<TransferGroupInfo> resultList;

	if (ViewUtils::gatherSelections(m_ui->transfersTreeView->selectionModel(), m_groupModel, resultList)) {
		for (auto &item : resultList) {
			if (gTaskMgr->hasActiveTasksFor(item.group.id))
				gTaskMgr->pauseTasks(item.group.id);
			else if (item.hasIncoming && !item.assignees.empty())
				TransferUtils::startTransfer(item.group.id, item.assignees[0].device.id);
		}
	}
}

void MainWindow::updateButtons()
{
	QList<TransferGroupInfo> resultList;

	if (ViewUtils::gatherSelections(m_ui->transfersTreeView->selectionModel(), m_groupModel, resultList)) {
		m_ui->showButton->setEnabled(resultList.size() == 1);
		m_ui->removeButton->setEnabled(!resultList.empty());
		bool hasRunning = false;

		for (const auto &item : resultList)
			if ((hasRunning = gTaskMgr->hasActiveTasksFor(item.group.id)))
				break;

		if (hasRunning) {
			m_ui->startPauseButton->setText(tr("Pause"));
			m_ui->startPauseButton->setEnabled(true);
		} else if (resultList.size() == 1) {
			m_ui->startPauseButton->setText(tr("Start"));
			m_ui->startPauseButton->setEnabled(resultList[0].hasIncoming);
		} else
			m_ui->startPauseButton->setEnabled(false);
	} else {
		m_ui->showButton->setEnabled(false);
		m_ui->removeButton->setEnabled(false);
		m_ui->startPauseButton->setText(tr("Start"));
		m_ui->startPauseButton->setEnabled(false);
	}
}
