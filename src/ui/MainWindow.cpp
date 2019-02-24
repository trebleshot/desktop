#include <QtGui/QDesktopServices>
#include <src/broadcast/SeamlessClient.h>
#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>
#include <QRandomGenerator>
#include "MainWindow.h"
#include "ManageDevicesDialog.h"
#include "ShowTransferDialog.h"
#include "DeviceChooserDialog.h"
#include "FileAdditionProgressDialog.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), m_ui(new Ui::MainWindow),
          m_commServer(new CommunicationServer), m_groupModel(new TransferGroupModel())
{
    m_ui->setupUi(this);

    setAcceptDrops(true);

    setWindowTitle(QString("%1 - %2")
                           .arg(QApplication::applicationName())
                           .arg(QApplication::applicationVersion()));

    if (AppUtils::getDatabase() != nullptr) {
        auto *error = new QMessageBox(this);

        error->setWindowTitle("Database error");
        error->setText("The database used to store information did not open. Refer to the development notes. "
                       "The program will force close.");

        error->show();

        connect(error, &QMessageBox::finished, this, &MainWindow::close);
    } else {
        connect(m_commServer, &CoolSocket::Server::serverStarted, [this]() {
            m_ui->label->setText(QString("TrebleShot is ready to accept files"));
            QObject::connect(m_commServer, &CommunicationServer::textReceived,
                             this, &MainWindow::showReceivedText);

            QObject::connect(m_commServer, &CommunicationServer::transferRequest,
                             this, &MainWindow::showTransferRequest);
        });

        connect(m_commServer, &CoolSocket::Server::serverFailure, [this]() {
            auto *error = new QMessageBox(this);

            error->setWindowTitle(QString("Server error"));
            error->setText(QString("TrebleShot server has returned with an error. "
                                   "Try restarting the application to solve the problem."));

            error->show();
            connect(this, SIGNAL(destroyed()), error, SLOT(deleteLater()));
        });

        m_commServer->start(0);
        m_ui->label->setText(QString("TrebleShot will not receive files"));
        m_ui->treeView->setModel(m_groupModel);

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
        FileAdditionProgressDialog(this, event->mimeData()->urls()).exec();
    }
}

void MainWindow::transferItemActivated(QModelIndex modelIndex)
{
    qDebug() << "transferItemActivated(): row=" << modelIndex.row()
             << "column=" << modelIndex.column();

    const auto &data = m_groupModel->list().at(modelIndex.row());

    ShowTransferWidget(this, data.group.id).exec();
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
