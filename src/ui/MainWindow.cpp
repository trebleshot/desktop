#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QSqlDriver>
#include <src/dialog/WelcomeDialog.h>
#include <src/model/TransferGroupListModel.h>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow), commServer(new CommunicationServer)
{
    ui->setupUi(this);

    if (AppUtils::getDatabase() == nullptr) {
        auto *errorMessage = new QMessageBox(this);

        errorMessage->setWindowTitle("Database error");
        errorMessage->setText("The database used to store information did not open. Refer to the development notes. "
                              "The program will force close.");

        errorMessage->show();

        connect(errorMessage, SIGNAL(finished(int)), this, SLOT(failureDialogFinished(int)));
    } else {
        bool serverStarted = commServer->startEnsured(5000);
        auto *model = new TransferGroupListModel();

        connect(ui->tableView, SIGNAL(activated(QModelIndex)), this, SLOT(transferItemActivated(QModelIndex)));
        connect(ui->actionAbout_TrebleShot, SIGNAL(triggered(bool)), this, SLOT(about()));
        connect(ui->actionAbout_Qt, SIGNAL(triggered(bool)), this, SLOT(aboutQt()));

        ui->tableView->setModel(model);
        ui->label->setText(serverStarted
                           ? QString("TrebleShot is ready to accept files")
                           : QString("TrebleShot will not receive files"));

        if (!serverStarted) {
            auto *errorMessage = new QMessageBox(this);

            errorMessage->setWindowTitle(QString("Server error"));
            errorMessage->setText(QString("TrebleShot server won't start. "
                                          "You will not be able to communicate with "
                                          "other devices."));

            errorMessage->show();
            connect(this, SIGNAL(destroyed()), errorMessage, SLOT(deleteLater()));
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    delete commServer;
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

    about->show();
}

void MainWindow::aboutQt()
{
    QApplication::aboutQt();
}
