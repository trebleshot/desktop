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
#include <utility>

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
        qint16 port;
        QHostAddress hostAddress;
        QPointer<ServerWorker> worker;
        QList<RequestHandler *> ongoingTasks;

    protected:
        void setWorker(ServerWorker *worker)
        {
            this->worker = worker;
        }

    public:
        explicit Server(QHostAddress hostAddress, int port = 0, QObject *parent = 0);

        friend class ServerWorker;

        friend class ActiveConnection;

        friend class PendingAppend;

        friend class Response;

        friend class RequestHandler;

        QHostAddress getHostAddress()
        { return hostAddress; }

        ServerWorker *getWorker()
        { return worker; }

        qint16 getPort()
        { return port; }

        bool isServing();

        void setHostAddress(QHostAddress hostAddress)
        {
            this->hostAddress = std::move(hostAddress);
        }

        void setPort(qint16 port)
        { this->port = port; }

        bool start(int blockingTime = -1);

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
        ActiveConnection(QTcpSocket *tcpServer, int msecTimeout = 2000, QObject *parent = 0)
                : QObject(parent)
        {
            this->activeSocket = tcpServer;
            this->timeout = msecTimeout;
        }

        ~ActiveConnection()
        {
            cout << "ActiveConnection is deleted" << endl;

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
        size_t length;
    };

    class ServerWorker : public QThread {
    Q_OBJECT
        QTcpServer *tcpServer;
        Server *server;
        bool serverListening = false;

    public:
        explicit ServerWorker(Server *server, QObject *parent = 0);

        ~ServerWorker()
        {
        }

        bool isServing()
        {
            return isRunning() && serverListening;
        }

        QTcpServer *getTcpServer()
        { return tcpServer; }

        void setTcpServer(QTcpServer *server);

    protected:
        void run();
    };

    class RequestHandler : public QThread {
    Q_OBJECT
        Server *server;
        ActiveConnection *connection;

    public:
        RequestHandler(Server *server, ActiveConnection *connection, QObject *parent = 0)
                : QThread(parent)
        {
            this->server = server;
            this->connection = connection;
        }

    protected:
        void run();
    };

    class Client : public QThread {
    Q_OBJECT

    public:
        Client(QObject *parent = 0)
                : QThread(parent)
        {
        }

        ~Client()
        {
            cout << "Scope removed along with the data" << endl;
        }

        ActiveConnection *openConnection(QString hostAddress, quint16 port, int timeoutMSeconds = 3000);

        virtual void connectionPhase() = 0;

    protected:
        virtual void run()
        {
            connectionPhase();
        }
    };

    class PendingAppend : public QObject {
    Q_OBJECT
        QIODevice *ioDevice;
        QByteArray *bytes = new QByteArray;

    public:
        PendingAppend(QIODevice *ioDevice)
        {
            this->ioDevice = ioDevice;

            connect(ioDevice, SIGNAL(readyRead()), this, SLOT(readData()));
        }

        virtual ~PendingAppend()
        {
            delete bytes;
        }

        QByteArray *getBytes()
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
