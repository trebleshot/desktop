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
          m_groupModel(new TransferGroupModel())
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
            m_ui->label->setText(QString("TrebleShot is ready to accept files"));

            connect(m_commServer, &CommunicationServer::textReceived,this, &MainWindow::showReceivedText);
            connect(m_commServer, &CommunicationServer::transferRequest, this, &MainWindow::showTransferRequest);
            connect(m_commServer, &CommunicationServer::deviceBlocked, this, &MainWindow::deviceBlocked);
        } else {
            m_ui->label->setText(QString("TrebleShot will not receive files"));

            auto *error = new QMessageBox(this);
            error->setWindowTitle(QString("Server error"));
            error->setText(QString("TrebleShot server has returned with an error. "
                                   "Try restarting the application to solve the problem."));
            connect(this, &MainWindow::destroyed, error, &QObject::deleteLater);
            error->show();
        }

        m_ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
        m_ui->treeView->setModel(m_groupModel);

        connect(m_ui->treeView, &QTreeView::customContextMenuRequested, [this](const QPoint &point) {
            QMenu menu(m_ui->treeView);

            const auto &model = m_ui->treeView->indexAt(point);

            if (model.isValid()) {
                updateAvailability();

                menu.addAction(m_ui->actionStart);
                menu.addAction(m_ui->actionPause);
                menu.addSeparator();
                menu.addAction(m_ui->actionAdd_devices);
                menu.addAction(m_ui->actionRemove);

                menu.exec(m_ui->treeView->viewport()->mapToGlobal(point));
            }
        });

        connect(m_ui->menuEdit, &QMenu::aboutToShow, this, &MainWindow::updateAvailability);
        connect(m_ui->treeView, &QTreeView::activated, this, &MainWindow::transferItemActivated);
        connect(m_ui->actionAbout_TrebleShot, &QAction::triggered, this, &MainWindow::about);
        connect(m_ui->actionAbout_Qt, &QAction::triggered, this, &MainWindow::aboutQt);
        connect(m_ui->actionManage_devices, &QAction::triggered, this, &MainWindow::manageDevices);
        connect(m_ui->actionRemove, &QAction::triggered, this, &MainWindow::remove);
        connect(m_ui->actionAdd_devices, &QAction::triggered, this, &MainWindow::send);
        connect(m_ui->actionStorage_location, &QAction::triggered, this, &MainWindow::setStorageLocation);

        connect(m_ui->actionStart_receiver, &QAction::triggered, []() {
            qDebug() << "Debug process";

            SqlSelection sqlSelection;
            sqlSelection.setTableName(DB_TABLE_TRANSFERASSIGNEE);
            sqlSelection.setOrderBy(DB_FIELD_TRANSFERASSIGNEE_GROUPID, false);

            const auto &assigneeList = gDatabase->castQuery(sqlSelection, TransferAssignee());

            if (!assigneeList.empty()) {
                const auto &assignee = assigneeList.first();
                auto *client = new SeamlessClient(assignee.deviceId, assignee.groupId);

                qDebug() << "Randomly starting a receive process for device"
                         << assignee.deviceId
                         << "for group id"
                         << assignee.groupId;

                client->start();
            } else {
                qDebug() << "No assignee found to start as debug receiver process";
            }
        });

        refreshStorageLocation();

        {
            auto thisDevice = AppUtils::getLocalDevice();
            DeviceConnection connection(thisDevice.id, "unk0");

            connection.hostAddress = QHostAddress("127.0.0.1");

            gDatabase->publish(thisDevice);
            gDatabase->publish(connection);
        }
    }

    const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
    //adjustSize();
    move((availableGeometry.width() - width()) / 2, (availableGeometry.height() - height()) / 2);
}

MainWindow::~MainWindow()
{
    delete m_ui;
    delete m_groupModel;
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

        FileAdditionProgressDialog progressDialog(this, event->mimeData()->urls());
        connect(&progressDialog, &FileAdditionProgressDialog::filesAdded, this, &MainWindow::filesAdded);
        progressDialog.exec();
    }
}

void MainWindow::transferItemActivated(QModelIndex modelIndex)
{
    const auto &data = m_groupModel->list().at(modelIndex.row());
    ShowTransferDialog(this, data.group.id).exec();
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

void MainWindow::deviceForAddedFiles(groupid groupId, QList<NetworkDevice> devices)
{
    TransferRequestProgressDialog(this, groupId, devices).exec();
}

void MainWindow::filesAdded(groupid groupId)
{
    DeviceChooserDialog dialog(this, groupId);
    connect(&dialog, &DeviceChooserDialog::devicesSelected, this, &MainWindow::deviceForAddedFiles);
    dialog.exec();
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

        messageBox.setWindowTitle(QString("File receive request from %1").arg(device.nickname));
        messageBox.setText(QString("%1 wants to you send you files, %2 in total. Do you want to receive now?")
                                   .arg(device.nickname)
                                   .arg(filesTotal));
        messageBox.addButton(QMessageBox::StandardButton::Ignore);

        messageBox.exec();
    } catch (...) {
        // do nothing
    }
}

void MainWindow::manageDevices()
{
    ManageDevicesDialog(this).exec();
}

void MainWindow::send()
{
    const auto &selectedIndexes = m_ui->treeView->selectionModel()->selectedIndexes();

    if (selectedIndexes.empty())
        return;

    const QList<int> &ids = ViewUtils::getSelectionRows(m_ui->treeView->selectionModel());

    if (ids.size() == 1)
        filesAdded(m_groupModel->list().at(ids[0]).group.id);
}

void MainWindow::remove()
{
    auto list = m_groupModel->list();

    for (int row : ViewUtils::getSelectionRows(m_ui->treeView->selectionModel())) {
        TransferGroup group = list.at(row).group;
        gDatabase->remove(group);
    }
}

void MainWindow::updateAvailability()
{
    const auto &selectedIndexes = m_ui->treeView->selectionModel()->selectedIndexes();

    if (selectedIndexes.empty())
        return;

    const QList<int> &ids = ViewUtils::getSelectionRows(m_ui->treeView->selectionModel());

    m_ui->actionRemove->setEnabled(!ids.empty());

    if (ids.size() == 1) {
        const auto &item = m_groupModel->list().at(0);

        m_ui->actionStart->setEnabled(item.hasIncoming);
        m_ui->actionPause->setEnabled(item.hasIncoming);
        m_ui->actionAdd_devices->setEnabled(item.hasOutgoing);
    } else {
        m_ui->actionStart->setEnabled(false);
        m_ui->actionPause->setEnabled(false);
        m_ui->actionAdd_devices->setEnabled(false);
    }
}

void MainWindow::refreshStorageLocation()
{
    m_ui->label_2->setText(TransferUtils::getDefaultSavePath());
}

void MainWindow::setStorageLocation()
{
    AppUtils::getDefaultSettings().setValue(
            "savePath", QFileDialog::getExistingDirectory(this, "Choose a location where files are put",
                                                          TransferUtils::getDefaultSavePath()));

    refreshStorageLocation();
}

void MainWindow::deviceBlocked(const QString &deviceId, const QHostAddress &address)
{
    NetworkDevice device(deviceId);

    if (gDatabase->reconstructSilently(device)) {
        QMessageBox box(this);
        box.setWindowTitle(device.nickname);
        box.setText(QString("Restricted %1 trying to communicate with you?").arg(device.nickname));
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
