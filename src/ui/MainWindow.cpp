#include <QtGui/QDesktopServices>
#include <src/broadcast/SeamlessClient.h>
#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>
#include "MainWindow.h"
#include "ManageDevicesDialog.h"
#include "ShowTransferDialog.h"
#include "DeviceChooserDialog.h"
#include "FileAdditionProgressDialog.h"

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

            QObject::connect(m_commServer, &CommunicationServer::textReceived,
                             this, &MainWindow::showReceivedText);

            QObject::connect(m_commServer, &CommunicationServer::transferRequest,
                             this, &MainWindow::showTransferRequest);
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

            menu.addAction(m_ui->actionStart_receiver);

            menu.exec(m_ui->treeView->viewport()->mapToGlobal(point));
        });

        connect(m_ui->treeView, SIGNAL(activated(QModelIndex)), this, SLOT(transferItemActivated(QModelIndex)));
        connect(m_ui->actionAbout_TrebleShot, SIGNAL(triggered(bool)), this, SLOT(about()));
        connect(m_ui->actionAbout_Qt, SIGNAL(triggered(bool)), this, SLOT(aboutQt()));
        connect(m_ui->actionManage_devices, &QAction::triggered, this, &MainWindow::manageDevices);

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
    auto *gThread = new GThread([groupId, devices](GThread *thread) {
        QList<TransferObject> objectList;

        {
            SqlSelection selection;
            selection.setTableName(DB_DIVIS_TRANSFER);
            selection.setWhere(QString("%1 = ?").arg(DB_FIELD_TRANSFER_GROUPID));
            selection.whereArgs << groupId;

            gDbSignal->doSynchronized([&selection, &objectList](AccessDatabase *db) {
                objectList << db->castQuery(selection, TransferObject());
            });
        }

        if (objectList.empty() || devices.empty()) {
            qDebug() << "deviceForAddedFiles << Empty object list" << objectList.size() << devices.size();
            return;
        }

        QList<NetworkDevice> passedDevices;

        for (const NetworkDevice &device : devices) {
            SqlSelection deviceSelection;
            deviceSelection.setTableName(DB_TABLE_TRANSFER);
            deviceSelection.setWhere(QString("%1 = ? AND %2 = ? AND %3 = ?")
                                             .arg(DB_FIELD_TRANSFER_GROUPID)
                                             .arg(DB_FIELD_TRANSFER_DEVICEID)
                                             .arg(DB_FIELD_TRANSFER_TYPE));
            deviceSelection.whereArgs << groupId << device.id << TransferObject::Type::Outgoing;
            deviceSelection.setLimit(1);

            if (gDbSignal->contains(deviceSelection)) {
                qDebug() << "deviceForAddedFiles << Already contains for" << device.nickname;
                passedDevices << device;
            } else {
                if (gDbSignal->transaction()) {
                    for (auto &object : objectList) {
                        object.deviceId = device.id;
                        gDbSignal->insert(object);
                    }

                    gDbSignal->commit();
                    passedDevices << device;
                }
            }
        }

        for (auto &thisDevice : passedDevices) {
            SqlSelection connectionSelection;
            connectionSelection.setTableName(DB_TABLE_DEVICECONNECTION);
            connectionSelection.setWhere(QString("%1 = ?").arg(DB_FIELD_DEVICES_ID));
            connectionSelection.setOrderBy(DB_FIELD_DEVICECONNECTION_LASTCHECKEDDATE, false);
            connectionSelection.whereArgs << thisDevice.id;

            QList<DeviceConnection> connections;

            gDbSignal->doSynchronized([&connectionSelection, &connections](AccessDatabase *db) {
                connections << db->castQuery(connectionSelection, DeviceConnection());
            });

            if (connections.empty()) {
                qDebug() << "deviceForAddedFiles << No connection for" << thisDevice.nickname;
            } else {
                bool shouldTryNext = true;

                for (const auto &thisConnection : connections) {
                    CSActiveConnection *connection = nullptr;
                    CommunicationBridge bridge;
                    bridge.setDevice(thisDevice);

                    TransferAssignee assignee(groupId, thisDevice.id, thisConnection.adapterName);
                    QJsonObject thisObject{
                            {KEYWORD_REQUEST, KEYWORD_REQUEST_TRANSFER},
                            {KEYWORD_TRANSFER_GROUP_ID, QVariant(groupId).toLongLong()}
                    };

                    QJsonArray filesIndex;

                    for (const auto &object : objectList) {
                        QJsonObject jsonObject{
                                {KEYWORD_INDEX_FILE_NAME,     object.friendlyName},
                                {KEYWORD_INDEX_FILE_SIZE,     QVariant((qulonglong) object.fileSize).toLongLong()},
                                {KEYWORD_TRANSFER_REQUEST_ID, QVariant(object.id).toLongLong()},
                                {KEYWORD_INDEX_FILE_MIME,     object.fileMimeType}
                        };

                        if (object.directory != nullptr)
                            jsonObject.insert(KEYWORD_INDEX_DIRECTORY, object.directory);

                        filesIndex.append(jsonObject);
                    }

                    thisObject.insert(KEYWORD_FILES_INDEX, filesIndex);

                    try {
                        connection = bridge.communicate(thisDevice, thisConnection);
                        shouldTryNext = false;

                        connection->reply(thisObject);

                        {
                            const QJsonObject &thisReply = connection->receive().asJson();

                            if (thisReply.value(KEYWORD_RESULT).toBool(false)) {
                                qDebug() << "deviceForAddedFiles << Successful for" << thisDevice.nickname;
                                gDbSignal->publish(assignee);
                            } else
                                qDebug() << "deviceForAddedFiles << Failed for <<" << thisDevice.nickname << thisReply;
                        }
                    } catch (...) {
                        // do nothing
                        qDebug() << "deviceForAddedFiles << Error" << thisDevice.nickname << thisConnection.adapterName;
                        qDebug() << "deviceForAddedFiles << Continue ?? " << shouldTryNext;
                    }

                    delete connection;

                    if (!shouldTryNext)
                        break;
                }
            }
        }

    }, true);

    gThread->start();
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