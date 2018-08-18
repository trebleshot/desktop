#include "mainwindow.h"
#include "stringlistmodel.h"
#include "ui_mainwindow.h"

#include <QHostAddress>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    CoolSocket::Server* coolServer = new CoolSocket::Server(QHostAddress::Any, 5555);

    coolServer->start();

    //connect(coolServer, SIGNAL(handleRequest(CoolSocket::ActiveConnection*)), this, SLOT(communicate(CoolSocket::ActiveConnection*)));

    QStringList numbers;
    QListView* listView;
    StringListModel* itemModel;

    ui->setupUi(this);
    listView = ui->listView;

    numbers << "One"
            << "Two"
            << "Three"
            << "Four";

    itemModel = new StringListModel(numbers);

    listView->setModel(itemModel);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::communicate(CoolSocket::ActiveConnection* connection)
{
}
