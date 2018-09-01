#include "mainwindow.h"
#include "accessdatabase.h"
#include "stringlistmodel.h"
#include "transferobject.h"
#include "ui_mainwindow.h"

#include <QHostAddress>
#include <QKeyEvent>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>
#include <QSqlTableModel>
#include <QTime>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QStringList numbers;
    QListView* listView;
    StringListModel* itemModel;
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    db.setDatabaseName("/home/veli/test.db");

    if (db.open()) {
        cout << "DB has opened" << endl;

        AccessDatabase* dbInstance = new AccessDatabase(&db);
        QSqlQuery sqlQuery;
        QSqlTableModel tableModel(this, db);

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

        testtObject.friendlyName = "Sagopaaaaaa";

        SqlSelection* selection = testtObject.getWhere();

        dbInstance->update(&testtObject);
        dbInstance->remove(&testtObject);

        db.close();
    }

    ui->setupUi(this);
    listView = ui->listView;

    numbers << "One"
            << "Two"
            << "Three"
            << "Four";

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
}

void MainWindow::clickedButtonConnect(bool checked)
{
    cout << "Test client started" << endl;
    testClient->start();
}

void MainWindow::clickedButtonServer(bool checked)
{
    cout << "Test server started" << endl;
    commServer->start(true);
}

void MainWindow::clickedButtonServerStop(bool checked)
{
    cout << "Test server stopped" << endl;
    commServer->stop(true);
}

void MainWindow::dropEvent(QDropEvent* event)
{
    cout << "has something been dropp'd" << endl;
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent* event)
{
    cout << "has something been 2x click'd" << endl;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if ((event->modifiers() & Qt::ShiftModifier)
        && (event->modifiers() & Qt::ControlModifier)
        && event->key() == Qt::Key_S)
        cout << "Modifiers are okay" << endl;
}
