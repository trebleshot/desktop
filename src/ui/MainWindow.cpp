#include <QtGui/QDesktopServices>
#include <src/broadcast/SeamlessClient.h>
#include "MainWindow.h"
#include "ManageDevicesWidget.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), m_ui(new Ui::MainWindow), m_commServer(new CommunicationServer)
{
    m_ui->setupUi(this);

    setWindowTitle(QString("%1 - %2")
                           .arg(QApplication::applicationName())
                           .arg(QApplication::applicationVersion()));

    if (AppUtils::getDatabase() == nullptr) {
        auto *errorMessage = new QMessageBox(this);

        errorMessage->setWindowTitle("Database error");
        errorMessage->setText("The database used to store information did not open. Refer to the development notes. "
                              "The program will force close.");

        errorMessage->show();

        connect(errorMessage, SIGNAL(finished(int)), this, SLOT(failureDialogFinished(int)));
    } else {
        connect(m_commServer, &CoolSocket::Server::serverStarted, [this]() {
            m_ui->label->setText(QString("TrebleShot is ready to accept files"));
            QObject::connect(m_commServer, &CommunicationServer::textReceived,
                             this, &MainWindow::showReceivedText);

            QObject::connect(m_commServer, &CommunicationServer::transferRequest,
                             this, &MainWindow::showTransferRequest);
        });

        connect(m_commServer, &CoolSocket::Server::serverFailure, [this]() {
            auto *errorMessage = new QMessageBox(this);

            errorMessage->setWindowTitle(QString("Server error"));
            errorMessage->setText(QString("TrebleShot server has returned with an error. "
                                          "Try restarting the application to solve the problem."));

            errorMessage->show();
            connect(this, SIGNAL(destroyed()), errorMessage, SLOT(deleteLater()));
        });

        m_commServer->start(0);
        m_ui->label->setText(QString("TrebleShot will not receive files"));

        auto *model = new TransferGroupListModel();

        connect(m_ui->treeView, SIGNAL(activated(QModelIndex)), this, SLOT(transferItemActivated(QModelIndex)));
        connect(m_ui->actionAbout_TrebleShot, SIGNAL(triggered(bool)), this, SLOT(about()));
        connect(m_ui->actionAbout_Qt, SIGNAL(triggered(bool)), this, SLOT(aboutQt()));
        connect(m_ui->actionManage_devices, &QAction::triggered, this, &MainWindow::manageDevices);

        m_ui->treeView->setModel(model);

        connect(m_ui->actionStart_receiver, &QAction::triggered, []() {
            qDebug() << "Debug process";

            SqlSelection sqlSelection;
            sqlSelection.setTableName(DbStructure::TABLE_TRANSFERASSIGNEE);
            sqlSelection.setOrderBy(DbStructure::FIELD_TRANSFERASSIGNEE_GROUPID, false);

            auto *assigneeList = gDatabase->castQuery(sqlSelection, new TransferAssignee);

            if (assigneeList->first() != nullptr) {
                auto *assignee = assigneeList->first();
                auto *client = new SeamlessClient(assignee->deviceId, assignee->groupId);

                qDebug() << "Randomly starting a receive process for device"
                         << assignee->deviceId
                         << "for group id"
                         << assignee->groupId;

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
    delete m_commServer;
}

void MainWindow::failureDialogFinished(int state)
{
    close();
}

void MainWindow::transferItemActivated(QModelIndex modelIndex)
{
    qDebug() << "transferItemActivated(): row=" << modelIndex.row()
             << "column=" << modelIndex.column();
}

void MainWindow::about()
{
    auto *about = new QMessageBox(this);

    about->setWindowTitle(QString("About TrebleShot"));
    about->setText(QString("TrebleShot is a cross platform file transferring tool."));
    about->addButton(QMessageBox::StandardButton::Close);
    QPushButton *buttonMoreInfo = about->addButton(QString("More info"), QMessageBox::ButtonRole::ActionRole);

    connect(buttonMoreInfo, &QPushButton::pressed, []() {
        QDesktopServices::openUrl(QUrl(URI_APP_HOME));
    });

    connect(this, SIGNAL(destroyed()), about, SLOT(deleteLater()));

    about->show();
}

void MainWindow::aboutQt()
{
    QApplication::aboutQt();
}

void MainWindow::showReceivedText(const QString &text, const QString &deviceId)
{
    auto *device = new NetworkDevice(deviceId);

    try {
        gDatabase->reconstruct(device);

        auto *messageBox = new QMessageBox(this);

        messageBox->setWindowTitle(QString("Text received from %1").arg(device->nickname));
        messageBox->setText(text);
        messageBox->addButton(QMessageBox::StandardButton::Close);

        QPushButton *buttonCopy = messageBox->addButton(QString("Copy to clipboard"), QMessageBox::ButtonRole::ActionRole);

        connect(buttonCopy, &QPushButton::pressed, [text]() {
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(text);
        });
        connect(this, SIGNAL(destroyed()), messageBox, SLOT(deleteLater()));

        messageBox->show();
    } catch (...) {
        // do nothing
    }

    delete device;
}

void MainWindow::showTransferRequest(const QString &deviceId, quint32 groupId, int filesTotal)
{
    auto *device = new NetworkDevice(deviceId);

    try {
        gDatabase->reconstruct(device);

        auto *messageBox = new QMessageBox(this);

        messageBox->setWindowTitle(QString("File receive request from %1").arg(device->nickname));
        messageBox->setText(QString("%1 wants to you send you files, %2 in total. Do you want to receive now?")
                                    .arg(device->nickname)
                                    .arg(filesTotal));
        messageBox->addButton(QMessageBox::StandardButton::Ignore);

        /*
        messageBox->addButton(QMessageBox::StandardButton::Later);

        QPushButton *buttonCopy = messageBox->addButton(QString("Copy to clipboard"), QMessageBox::ButtonRole::ActionRole);

        connect(buttonCopy, &QPushButton::pressed, [text]() {
            QClipboard *clipboard = QApplication::clipboard();
            clipboard->setText(text);
        });
         */

        connect(this, SIGNAL(destroyed()), messageBox, SLOT(deleteLater()));

        messageBox->show();

    } catch (...) {
        // do nothing
    }

    delete device;
}

void MainWindow::manageDevices()
{
    ManageDevicesWidget manageDevices(
            nullptr);

    manageDevices.show();

    qDebug() << "Did it start?";
}
