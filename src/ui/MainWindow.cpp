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

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), m_ui(new Ui::MainWindow),
          m_seamlessServer(new SeamlessServer), m_commServer(new CommunicationServer),
          m_groupModel(new TransferGroupModel()), m_deviceModel(new NetworkDeviceModel)
{
    m_ui->setupUi(this);

    setAcceptDrops(true);
    setWindowTitle(QString("%1 - %2")
                           .arg(QApplication::applicationName())
                           .arg(QApplication::applicationVersion()));

    if (AppUtils::getDatabase() == nullptr) {
        auto *error = new QMessageBox(this);

        error->setWindowTitle("Database error");
        error->setText("The database used to store information did not open. Refer to the development notes. "
                       "The program will force close.");

        error->show();

        connect(error, &QMessageBox::finished, this, &MainWindow::close);
    } else {

        if (m_commServer->start() && m_seamlessServer->start()) {
            connect(m_commServer, &CommunicationServer::textReceived, this, &MainWindow::showReceivedText);
            connect(m_commServer, &CommunicationServer::transferRequest, this, &MainWindow::showTransferRequest);
            connect(m_commServer, &CommunicationServer::deviceBlocked, this, &MainWindow::deviceBlocked);
        } else {
            auto *error = new QMessageBox(this);
            error->setWindowTitle(QString("Server error"));
            error->setText(QString("TrebleShot server has returned with an error. "
                                   "Try restarting the application to solve the problem."));
            connect(this, &MainWindow::destroyed, error, &QObject::deleteLater);
            error->show();
        }

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
        connect(m_ui->devicesTreeView, &QTreeView::customContextMenuRequested, this, &MainWindow::deviceContextMenu);
        connect(m_ui->devicesTreeView, &QTreeView::activated, this, &MainWindow::deviceSelected);
        connect(m_ui->transfersTreeView, &QTreeView::customContextMenuRequested, this, &MainWindow::transferContextMenu);
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
        connect(m_ui->usernameLineEdit, &QLineEdit::textChanged, this, &MainWindow::usernameChanged);
        connect(m_ui->saveStorageButton, &QPushButton::clicked, this, &MainWindow::savePathChanged);

        refreshStorageLocation();
        transferSelectionChanged(QItemSelection(), QItemSelection());
        m_ui->usernameLineEdit->setText(getUserNickname());
    }

    const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
    //adjustSize();
    move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_groupModel;
    delete m_deviceModel;
    delete m_commServer;
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

        FileAdditionProgressDialog progressDialog(this, TransferUtils::getPaths(event->mimeData()->urls()));
        connect(&progressDialog, SIGNAL(filesAdded(groupid)), this, SLOT(showTransfer(groupid)));
        progressDialog.exec();
    }
}

void MainWindow::transferItemActivated(const QModelIndex &modelIndex)
{
    const auto &data = m_groupModel->list().at(modelIndex.row());
    showTransfer(data.group.id);
}

void MainWindow::about()
{
    QMessageBox about(this);

    about.setWindowTitle(QString("About TrebleShot"));
    about.setText(QString("TrebleShot is a cross platform file transferring tool."));
    about.addButton(QMessageBox::StandardButton::Close);
    QPushButton *buttonMoreInfo = about.addButton(QString("More info"), QMessageBox::ButtonRole::ActionRole);

    connect(buttonMoreInfo, &QPushButton::pressed, []() {
        QDesktopServices::openUrl(QUrl(URI_APP_HOME));
    });

    about.exec();
}

void MainWindow::aboutQt()
{
    QApplication::aboutQt();
}

void MainWindow::showTransfer(groupid groupId)
{
    ShowTransferDialog(this, groupId).exec();
}

void MainWindow::showReceivedText(const QString &text, const QString &deviceId)
{
    NetworkDevice device(deviceId);

    try {
        gDatabase->reconstruct(device);

        QMessageBox messageBox(this);

        messageBox.setWindowTitle(QString("Text received from %1").arg(device.nickname));
        messageBox.setText(text);
        messageBox.addButton(QMessageBox::StandardButton::Close);

        QPushButton *buttonCopy = messageBox.addButton(QString("Copy to clipboard"), QMessageBox::ButtonRole::ActionRole);

        connect(buttonCopy, &QPushButton::pressed, [text]() {
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(text);
        });

        messageBox.exec();
    } catch (...) {
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
        messageBox.setText(QString("Receive files from %1, %2 in total?")
                                   .arg(device.nickname)
                                   .arg(filesTotal));
        auto *okButton = messageBox.addButton(QMessageBox::StandardButton::Ok);
        messageBox.addButton(QMessageBox::StandardButton::Ignore);

        connect(okButton, &QPushButton::pressed, [groupId, deviceId]() {
            auto *client = new SeamlessClient(groupId, deviceId, true);
            client->start();
        });

        messageBox.exec();
        showTransfer(groupId);
    } catch (...) {
        // do nothing
    }
}

void MainWindow::showTransfer()
{
    const auto &selectedIndexes = m_ui->transfersTreeView->selectionModel()->selectedIndexes();

    if (selectedIndexes.empty())
        return;

    const QList<int> &ids = ViewUtils::getSelectionRows(m_ui->transfersTreeView->selectionModel());

    if (ids.size() == 1)
        showTransfer(m_groupModel->list().at(ids[0]).group.id);
}

void MainWindow::removeTransfer()
{
    auto list = m_groupModel->list();

    for (int row : ViewUtils::getSelectionRows(m_ui->transfersTreeView->selectionModel())) {
        TransferGroup group = list.at(row).group;
        gDatabase->remove(group);
    }
}

void MainWindow::refreshStorageLocation()
{
    m_ui->storageFolderLineEdit->setText(TransferUtils::getDefaultSavePath());
}

void MainWindow::setStorageLocation()
{
    AppUtils::getDefaultSettings().setValue("savePath", QFileDialog::getExistingDirectory(
            this, "Choose a location where files are put", TransferUtils::getDefaultSavePath()));

    refreshStorageLocation();
}

void MainWindow::deviceBlocked(const QString &deviceId, const QHostAddress &address)
{
    NetworkDevice device(deviceId);

    if (gDatabase->reconstructSilently(device)) {
        QMessageBox box(this);
        box.setWindowTitle(device.nickname);
        box.setText(QString("Restricted %1 is trying to communicate with you?").arg(device.nickname));
        box.addButton(QMessageBox::StandardButton::Ignore);
        QPushButton *allowButton = box.addButton("Allow", QMessageBox::ButtonRole::AcceptRole);
        QPushButton *blockButton = box.addButton("Deny for this session", QMessageBox::ButtonRole::RejectRole);

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

void MainWindow::usernameChanged(QString username)
{
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
        box.setWindowTitle("Inappropriate folder path");
        box.setText("Non-existing folders cannot be set");
        box.exec();
    }
}

void MainWindow::deviceContextMenu(const QPoint &point)
{
    const QModelIndex &modelIndex = m_ui->devicesTreeView->indexAt(point);

    if (modelIndex.isValid()) {
        NetworkDevice device = m_deviceModel->list()->at(modelIndex.row());

        QMenu menu(m_ui->devicesTreeView);
        menu.addAction(device.isRestricted ? "Allow to access" : "Restrict", [&device]() {
            device.isRestricted = !device.isRestricted;
            gDatabase->publish(device);
        });
        menu.addAction("Remove", [&device]() {
            gDatabase->remove(device);
        });

        menu.exec(m_ui->devicesTreeView->mapToGlobal(point));
    }
}

void MainWindow::deviceSelected(const QModelIndex &modelIndex)
{
    if (modelIndex.isValid() && modelIndex.column() == NetworkDeviceModel::Status) {
        NetworkDevice device = m_deviceModel->list()->at(modelIndex.row());
        device.isRestricted = !device.isRestricted;
        gDatabase->publish(device);
    }
}

void MainWindow::selectFilesToSend()
{
    QFileDialog fileDialog;
    fileDialog.setFileMode(QFileDialog::FileMode::ExistingFiles);

    fileDialog.exec();

    FileAdditionProgressDialog progressDialog(this, fileDialog.selectedFiles());
    connect(&progressDialog, SIGNAL(filesAdded(groupid)), this, SLOT(showTransfer(groupid)));
    progressDialog.exec();
}

void MainWindow::taskStart()
{
    auto list = m_groupModel->list();

    for (int row : ViewUtils::getSelectionRows(m_ui->transfersTreeView->selectionModel())) {
        const auto &group = list.at(row).group;
        const auto &assignees = TransferUtils::getAllAssigneeInfo(group);

        if (!assignees.empty())
            TransferUtils::startTransfer(group.id, assignees[0].device.id);
    }
}

void MainWindow::taskPause()
{
    auto list = m_groupModel->list();

    for (int row : ViewUtils::getSelectionRows(m_ui->transfersTreeView->selectionModel())) {
        TransferGroup group = list.at(row).group;
        gTaskMgr->pauseTasks(group.id);
    }
}

void MainWindow::transferSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    const auto &selectedIndexes = m_ui->transfersTreeView->selectionModel()->selectedIndexes();

    if (selectedIndexes.empty())
        return;

    const QList<int> &ids = ViewUtils::getSelectionRows(m_ui->transfersTreeView->selectionModel());

    if (ids.size() == 1) {
        const auto &item = m_groupModel->list().at(ids[0]);
        const bool running = gTaskMgr->hasActiveTasksFor(item.group.id);

        if (running)
            m_ui->startPauseButton->setText("Pause");
        else {
            m_ui->startPauseButton->setText("Start");
            m_ui->startPauseButton->setEnabled(item.hasIncoming);
        }
    } else {
        m_ui->startPauseButton->setText("Pause All");
        m_ui->startPauseButton->setEnabled(ids.size() > 1);
    }

    m_ui->showButton->setEnabled(ids.size() == 1);
    m_ui->removeButton->setEnabled(!ids.empty());
}

void MainWindow::transferContextMenu(const QPoint &point)
{
    const auto &selectedIndexes = m_ui->transfersTreeView->selectionModel()->selectedIndexes();

    if (selectedIndexes.empty())
        return;

    QMenu menu(m_ui->transfersTreeView);
    const QList<int> &ids = ViewUtils::getSelectionRows(m_ui->transfersTreeView->selectionModel());

    if (ids.size() == 1) {
        const auto &item = m_groupModel->list().at(ids[0]);
        const bool running = gTaskMgr->hasActiveTasksFor(item.group.id);

        if (running)
            menu.addAction("Pause", this, &MainWindow::taskPause);
        else
            menu.addAction("Start", this, &MainWindow::taskStart)->setEnabled(item.hasIncoming);
    } else
        menu.addAction("Pause All", this, &MainWindow::taskPause)->setEnabled(ids.size() > 1);

    menu.addSeparator();
    menu.addAction("Show", this, SLOT(showTransfer()))->setEnabled(ids.size() == 1);
    menu.addAction("Remove", this, &MainWindow::removeTransfer)->setEnabled(!ids.empty());

    menu.exec(m_ui->transfersTreeView->viewport()->mapToGlobal(point));
}

void MainWindow::showReceivedFiles()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(TransferUtils::getDefaultSavePath()));
}

void MainWindow::taskToggle()
{
    const auto &selectedIndexes = m_ui->transfersTreeView->selectionModel()->selectedIndexes();

    if (selectedIndexes.empty())
        return;

    QMenu menu(m_ui->transfersTreeView);
    const QList<int> &ids = ViewUtils::getSelectionRows(m_ui->transfersTreeView->selectionModel());

    if (ids.size() == 1) {
        const auto &item = m_groupModel->list().at(ids[0]);
        const bool running = gTaskMgr->hasActiveTasksFor(item.group.id);

        if (running)
            gTaskMgr->pauseTasks(item.group.id);
        else if (item.hasIncoming) {
            for (const auto &assigneeInfo : item.assignees)
                TransferUtils::startTransfer(item.group.id, assigneeInfo.device.id);
        }
    } else {
        for (int index : ids) {
            const auto &item = m_groupModel->list().at(index);
            gTaskMgr->pauseTasks(item.group.id);
        }
    }
}
