#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "coolsocket.h"

#include <QMainWindow>
#include <iostream>

namespace Ui {
class MainWindow;
}

class CommunicationServer : public CoolSocket::Server {
public:
    CommunicationServer()
        : CoolSocket::Server(QHostAddress::Any, 5555)
    {
    }

    // Server interface
public:
    void connected(CoolSocket::ActiveConnection* connection)
    {
        connection->setTimeout(3000);

        try {
            while (1) {
                CoolSocket::Response* response = connection->receive();
                cout << "client said: " << response->response->toStdString() << endl;

                connection->reply("hi, there!");
            }
        } catch (exception e) {
            cerr << "Could not follow the rules" << endl;
        }
    }
};

class TestClient : public CoolSocket::Client {
    Q_OBJECT
protected:
    void connectionPhase()
    {
        cout << "About to connect" << endl;

        try {
            CoolSocket::ActiveConnection* connection = connect("0.0.0.0", 5555);

            cout << "Connected" << endl;

            while (connection->getSocket()->isValid()) {
                cout << "Running" << endl;
                connection->reply("thank you!");

                cout << "Replied" << endl;

                CoolSocket::Response* response = connection->receive();
                cout << "Received" << endl;
                cout << "server said: " << response->response->toStdString() << endl;
            }
        } catch (exception e) {
            cerr << "Failed to connect or read failed" << endl;
        }

        cout << "Exited" << endl;
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

public slots:
    void testButtonClicked(bool checked);
};

#endif // MAINWINDOW_H
