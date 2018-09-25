#include "mainwindow.h"
#include "src/model/stringlistmodel.h"
#include "src/database/object/transferobject.h"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QNetworkConfigurationManager>
#include <QSqlDriver>
#include <QTime>
#include <src/database/object/transfergroup.h>
#include <src/database/object/networkdevice.h>
#include <QtSql/QSqlError>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow)
{
    CommunicationServer *cserver = new CommunicationServer();
    cout << "Start cserver stat: " << cserver->start(5000) << endl;

    QStringList numbers;
    QListView *listView;
    StringListModel *itemModel;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("local.db");

    if (db.open()) {
        cout << "DB has opened" << endl;

        AccessDatabase *dbInstance(new AccessDatabase(&db));
        dbInstance->initialize();

        TransferObject transferObject;

        transferObject.accessPort = 10;
        transferObject.friendlyName = "Manameisjeff";
        transferObject.requestId = 1;
        transferObject.groupId = 1;
        transferObject.skippedBytes = 3;
        transferObject.type = TransferObject::Type::Incoming;
        transferObject.flag = TransferObject::Flag::Pending;

        dbInstance->publish(&transferObject);

        TransferObject testtObject(1);

        dbInstance->reconstruct(&testtObject);

        cout << "Generated ?? " << testtObject.friendlyName.toStdString() << endl;

        if (testtObject.flag == TransferObject::Flag::Pending)
            cout << "Well matched" << endl;

        testtObject.friendlyName = "Sals";

        dbInstance->update(&testtObject);
        dbInstance->remove(&testtObject);

        TransferGroup *transferGroup = new TransferGroup;

        transferGroup->savePath = "fuckthis";
        transferGroup->dateCreated = 23829;
        transferGroup->groupId = 23232;

        dbInstance->publish(transferGroup);

        TransferGroup *testGroup = new TransferGroup(23232);

        dbInstance->reconstruct(testGroup);

        NetworkDevice *device = new NetworkDevice;

        device->brand = QString("Brand");
        device->model = QString("Model");
        device->nickname = QString("Harakiri");
        device->versionNumber = 57;
        device->versionName = QString("1.0");
        device->deviceId = "1a1a1a1a1";
        device->tmpSecureKey = 2;
        device->lastUsageTime = 1;
        device->isLocalAddress = false;
        device->isRestricted = false;
        device->isTrusted = true;

        dbInstance->publish(device);

        NetworkDevice *testDevice = new NetworkDevice(QString("1a1a1a1a1"));

        dbInstance->reconstruct(testDevice);
    }

    ui->setupUi(this);
    listView = ui->listView;

    QNetworkConfigurationManager manager;

    QList<QNetworkConfiguration> configurations = manager.allConfigurations(QNetworkConfiguration::StateFlag::Active);

    for (QNetworkConfiguration config : configurations) {
        numbers << config.name();
    }

    itemModel = new StringListModel(numbers);
    listView->setModel(itemModel);

    connect(ui->buttonStartServer, SIGNAL(clicked(bool)), this, SLOT(clickedButtonServer(bool)));
    connect(ui->buttonStartServerStop, SIGNAL(clicked(bool)), this, SLOT(clickedButtonServerStop(bool)));
    connect(ui->buttonConnect, SIGNAL(clicked(bool)), this, SLOT(clickedButtonConnect(bool)));

    setWindowTitle(tr("Who is that") + "[*]");
    setWindowFilePath("/home/veli/test.db");
    setWindowModified(true);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete testClient;
    delete testServer;
}

void MainWindow::clickedButtonConnect(bool checked)
{
    cout << "Test client started" << endl;
    testClient->start();
}

void MainWindow::clickedButtonServer(bool checked)
{
    if (testServer->start(2000))
        cout << "Test server started" << endl;
    else
        QMessageBox::critical(this, QString("Critical error"), QString("Communication server won't start. It may be because there is another instance of the app running"));
}

void MainWindow::clickedButtonServerStop(bool checked)
{
    cout << "Test server stopped" << endl;
    testServer->stop(true);
}

void MainWindow::dropEvent(QDropEvent *event)
{
    cout << "has something been dropp'd" << endl;
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    cout << "has something been 2x click'd" << endl;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if ((event->modifiers() & Qt::ShiftModifier)
        && (event->modifiers() & Qt::ControlModifier)
        && event->key() == Qt::Key_S)
        cout << "Modifiers are okay" << endl;
}
