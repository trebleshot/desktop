#ifndef COOLSOCKET_H
#define COOLSOCKET_H

#include <QDataStream>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <iostream>

#define COOLSOCKET_KEYWORD_LENGTH "length"
#define COOLSOCKET_HEADER_DIVIDER "\nHEADER_END\n"
#define COOLSOCKET_HEADER_HEAP_SIZE 8196

using namespace std;

namespace CoolSocket {

class ServerWorker;
class Client;
class Server;
class Response;
class RequestHandler;
class ActiveConnection;
class PendingAppend;

class Server : public QObject {
    Q_OBJECT
    short int port;
    QHostAddress hostAddress;
    QPointer<ServerWorker> worker;

protected:
    void setWorker(ServerWorker* worker)
    {
        this->worker = worker;
    }

public:
    explicit Server(QHostAddress hostAddress, int port = 0, QObject* parent = 0);
    friend class ServerWorker;
    friend class ActiveConnection;
    friend class PendingAppend;
    friend class Response;
    friend class RequestHandler;

    QHostAddress getHostAddress() { return hostAddress; }

    ServerWorker* getWorker() { return worker; }

    int getPort() { return port; }

    bool isRunning();

    void setHostAddress(QHostAddress hostAddress)
    {
        this->hostAddress = hostAddress;
    }

    void setPort(int port) { this->port = port; }

    void start(bool block);

    void stop(bool block);

    virtual void connected(ActiveConnection* connection)
    {
    }
signals:
    void clientConnected(ActiveConnection* connection);
};

class ActiveConnection : public QObject {
    Q_OBJECT
    QTcpSocket* activeSocket;
    int timeout = 2000;

public:
    ActiveConnection(QTcpSocket* tcpServer, int msecTimeout = 2000, QObject* parent = 0)
        : QObject(parent)
    {
        this->activeSocket = tcpServer;
        this->timeout = msecTimeout;
    }

    QTcpSocket* getSocket()
    {
        return activeSocket;
    }

    int getTimeout() { return timeout; }

    void setTimeout(int msecs) { this->timeout = msecs; }

    void reply(char* reply);
    Response* receive();
};

class Response : public QObject {
    Q_OBJECT
public:
    QString* response;
    QJsonObject* headerIndex;
    int length;
};

class ServerWorker : public QThread {
    Q_OBJECT
    QTcpServer* tcpServer;
    Server* server;

public:
    explicit ServerWorker(Server* server, QObject* parent = 0);

    ~ServerWorker()
    {
        quit();
    }

    QTcpServer* getTcpServer() { return tcpServer; }

    void setTcpServer(QTcpServer* server);

protected:
    void run();
};

class RequestHandler : public QThread {
    Q_OBJECT
    Server* server;
    ActiveConnection* connection;

public:
    RequestHandler(Server* server, ActiveConnection* connection, QObject* parent = 0)
        : QThread(parent)
    {
        this->server = server;
        this->connection = connection;
    }

    ~RequestHandler()
    {
        cout << "Scope removed along with the data" << endl;
    }

protected:
    void run()
    {
        connect(this, SIGNAL(finished()), this->connection->getSocket(), SLOT(deleteLater()));

        emit server->clientConnected(this->connection);
        server->connected(this->connection);
    }
};

class Client : public QThread {
    Q_OBJECT

public:
    ~Client()
    {
        quit();
    }

    ActiveConnection* connect(QString hostAddress, quint16 port, int timeoutMSeconds = 3000)
    {
        QTcpSocket* socket = new QTcpSocket;
        socket->connectToHost(hostAddress, port);

        while (QAbstractSocket::SocketState::ConnectingState == socket->state())
            socket->waitForConnected(timeoutMSeconds);

        if (QAbstractSocket::SocketState::ConnectedState != socket->state())
            throw exception();

        return new ActiveConnection(socket);
    }

    virtual void connectionPhase()
    {
    }

protected:
    virtual void run()
    {
        connectionPhase();
    }
};

class PendingAppend : public QObject {
    QIODevice* ioDevice;
    QByteArray* bytes = new QByteArray;
    Q_OBJECT

public:
    PendingAppend(QIODevice* ioDevice)
    {
        this->ioDevice = ioDevice;
        connect(ioDevice, SIGNAL(readyRead()), this, SLOT(readData()));
    }
    virtual ~PendingAppend() {}

    QByteArray* getBytes()
    {
        return bytes;
    }

public slots:
    void readData()
    {
        bytes->append(this->ioDevice->readAll());
    }
};
}

#endif // COOLSOCKET_H
