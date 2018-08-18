#ifndef COOLSOCKET_H
#define COOLSOCKET_H

#include <QDataStream>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
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
inline const int maximumHeaderLength = 8196;

class Server;
class Worker;
class Response;
class ActiveConnection;
class PendingAppend;

class Server : public QObject {
    Q_OBJECT
    short int port;
    QHostAddress hostAddress;
    Worker* worker;

public:
    explicit Server(QHostAddress hostAddress, int port = 0, QObject* parent = 0);
    friend class Worker;

    QHostAddress getHostAddress() { return hostAddress; }

    int getPort() { return port; }

    void setHostAddress(QHostAddress hostAddress)
    {
        this->hostAddress = hostAddress;
    }

    void setPort(int port) { this->port = port; }

    void start();

public slots:
    void handOverRequest(ActiveConnection* connection);
};

class ActiveConnection : public QObject {
    Q_OBJECT
    QTcpSocket* activeSocket;

public:
    ActiveConnection(QTcpSocket* tcpServer, QObject* parent = 0)
        : QObject(parent)
    {
        this->activeSocket = tcpServer;
    }

    QTcpSocket* getSocket()
    {
        return activeSocket;
    }

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

class Worker : public QThread {
    Q_OBJECT
    QTcpServer* tcpServer;
    Server* server;

public:
    explicit Worker(Server* server, QObject* parent = 0);

    ~Worker()
    {
        quit();
    }

    QTcpServer* getTcpServer() { return tcpServer; }

    void setTcpServer(QTcpServer* server) { this->tcpServer = server; }

protected:
    void run();

signals:
    void linkRequest(ActiveConnection* ActiveConnection);
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
        printf("read data");
        bytes->append(this->ioDevice->readAll());
    }
};
}

#endif // COOLSOCKET_H
