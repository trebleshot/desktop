#include "MainWindow.h"

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
        errorMessage->setText("The getDatabase used to store information did not open. Refer to the development notes. "
                              "The program will force close.");

        errorMessage->show();

        connect(errorMessage, SIGNAL(finished(int)), this, SLOT(failureDialogFinished(int)));
    } else {
        connect(m_commServer, &CoolSocket::Server::serverStarted, [this]() {
            m_ui->label->setText(QString("TrebleShot is ready to accept files"));

            NetworkDeviceLoader::loadAsynchronously(this, "127.0.0.1", [](NetworkDevice *device) {
                if (device != nullptr)
                    qDebug() << "Device is" << device->nickname;
            });
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

        connect(m_ui->tableView, SIGNAL(activated(QModelIndex)), this, SLOT(transferItemActivated(QModelIndex)));
        connect(m_ui->actionAbout_TrebleShot, SIGNAL(triggered(bool)), this, SLOT(about()));
        connect(m_ui->actionAbout_Qt, SIGNAL(triggered(bool)), this, SLOT(aboutQt()));

        m_ui->tableView->setModel(model);
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
    QPushButton *buttonMoreInfo = about->addButton(QString("More info"), QMessageBox::ButtonRole::NoRole);

    connect(buttonMoreInfo, &QPushButton::pressed, []() {
        qDebug() << "Works like a charm";
    });

    about->show();
}

void MainWindow::aboutQt()
{
    QApplication::aboutQt();
}
