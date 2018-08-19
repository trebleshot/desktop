#include "mainwindow.h"
#include "stringlistmodel.h"
#include "ui_mainwindow.h"

#include <QHostAddress>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    CommunicationServer* server = new CommunicationServer;
    server->start(true);

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

    connect(ui->pushButton, SIGNAL(clicked(bool)), this, SLOT(testButtonClicked(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::testButtonClicked(bool checked)
{
    cout << "Test server started" << endl;

    TestClient* testClient = new TestClient;
    testClient->start();
}
