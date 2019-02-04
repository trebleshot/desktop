#ifndef COOLSOCKET_H
#define COOLSOCKET_H

#define COOLSOCKET_KEYWORD_LENGTH "length"
#define COOLSOCKET_HEADER_DIVIDER "\nHEADER_END\n"
#define COOLSOCKET_HEADER_HEAP_SIZE 8196

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
#include <utility>

using namespace std;

namespace CoolSocket {

    class ServerWorker;

    class Client;

    class Server;

    class Response;

    class RequestHandler;

    class ActiveConnection;

    class Server : public QObject {
    Q_OBJECT
        quint16 port;
        QHostAddress hostAddress;
        QPointer<ServerWorker> worker;
        QList<RequestHandler *> ongoingTasks;

    protected:
        void setWorker(ServerWorker *worker)
        {
            this->worker = worker;
        }

    public:
        explicit Server(QHostAddress hostAddress, quint16 port = 0, QObject *parent = nullptr);

        friend class ServerWorker;

        friend class ActiveConnection;

        friend class PendingAppend;

        friend class Response;

        friend class RequestHandler;

        QHostAddress getHostAddress()
        { return hostAddress; }

        ServerWorker *getWorker()
        { return worker; }

        quint16 getPort()
        { return port; }

        bool isServing();

        void setHostAddress(const QHostAddress &hostAddress)
        {
            this->hostAddress = hostAddress;
        }

        void setPort(quint16 port)
        { this->port = port; }

        bool start(int blockingTime = -1);

        bool startEnsured(int blockingTime = -1);

        void stop(int blockingTime = -1);

        virtual void connected(ActiveConnection *connection) = 0;

    signals:

        void clientConnected(ActiveConnection *connection);
    };

    class ActiveConnection : public QObject {
    Q_OBJECT
        QTcpSocket *activeSocket;
        int timeout = 2000;

    public:
        explicit ActiveConnection(QTcpSocket *tcpServer, int msecTimeout = 2000,
                                  QObject *parent = nullptr)
                : QObject(parent)
        {
            this->activeSocket = tcpServer;
            this->timeout = msecTimeout;
        }

        ~ActiveConnection() override
        {
            if (this->activeSocket->isOpen())
                this->activeSocket->close();

            delete activeSocket;
        }

        QTcpSocket *getSocket()
        {
            return activeSocket;
        }

        int getTimeout()
        { return timeout; }

        void setTimeout(int msecs)
        { this->timeout = msecs; }

        void reply(const char *reply);

        Response *receive();
    };

    class Response : public QObject {
    Q_OBJECT
    public:
        QString *response;
        QJsonObject *headerIndex;
        qsizetype length;
    };

    class ServerWorker : public QThread {
    Q_OBJECT
        QTcpServer *tcpServer;
        Server *server;
        bool serverListening = false;

    public:
        explicit ServerWorker(Server *server, QObject *parent = nullptr);

        ~ServerWorker() override
        = default;

        bool isServing()
        {
            return isRunning() && serverListening;
        }

        QTcpServer *getTcpServer()
        { return tcpServer; }

        void setTcpServer(QTcpServer *server);

    protected:
        void run() override;
    };

    class RequestHandler : public QThread {
    Q_OBJECT
        Server *server;
        ActiveConnection *connection;

    public:
        RequestHandler(Server *server, ActiveConnection *connection, QObject *parent = nullptr)
                : QThread(parent)
        {
            this->server = server;
            this->connection = connection;
        }

    protected:
        void run() override;
    };

    class Client : public QObject {
    Q_OBJECT

    public:
        explicit Client(QObject *parent = nullptr)
                : QObject(parent)
        {
        }

        static ActiveConnection *openConnection(const QObject *sender,
                                                const QString &hostName,
                                                quint16 port,
                                                int timeoutMSeconds = 3000);
    };
}

#endif // COOLSOCKET_H
