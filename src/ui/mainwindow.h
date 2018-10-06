#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "src/database/accessdatabase.h"
#include "src/broadcast/communicationserver.h"
#include "src/coolsocket/coolsocket.h"
#include <QMainWindow>
#include <iostream>

namespace Ui {
    class MainWindow;
}

class TestServer : public CoolSocket::Server {
public:
    TestServer()
            : CoolSocket::Server(QHostAddress::Any, 5555)
    {
    }

    // Server interface
public:
    void connected(CoolSocket::ActiveConnection *connection)
    {
        connection->setTimeout(3000);

        try {
            while (connection->getSocket()->isOpen()) {
                cout << "Request start sequence" << endl;

                CoolSocket::Response *response = connection->receive();
                cout << "client said: " << response->response->toStdString() << endl;

                connection->reply("hi, there!");
            }
        } catch (...) {
            cerr << "Could not follow the rules" << endl;
        }
    }
};

class TestClient : public CoolSocket::Client {
Q_OBJECT
protected:
    void connectionPhase() override
    {
        try {
            CoolSocket::ActiveConnection *connection(connect("0.0.0.0", 5555));

            int iterator = 0;

            while (connection->getSocket()->state() == QAbstractSocket::SocketState::ConnectedState
                   && iterator <= 10) {
                connection->reply("thank you!");

                CoolSocket::Response *response = connection->receive();
                cout << "server said: " << response->response->toStdString() << endl;

                iterator++;
            }
        } catch (exception& e) {
            cerr << "Failed to connect or read failed" << endl;
        }

        cout << "Exited" << endl;
    }
};

class MainWindow : public QMainWindow {
Q_OBJECT
    TestClient *testClient = new TestClient;
    TestServer *testServer = new TestServer;

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private:
    Ui::MainWindow *ui;

public slots:

    void clickedButtonConnect(bool checked);

    void clickedButtonServer(bool checked);

    void clickedButtonServerStop(bool checked);

protected:
    void dropEvent(QDropEvent *event);

    void mouseDoubleClickEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
};

#endif // MAINWINDOW_H
