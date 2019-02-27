#ifndef COOLSOCKET_H
#define COOLSOCKET_H

#define COOLSOCKET_KEYWORD_LENGTH "length"
#define COOLSOCKET_HEADER_DIVIDER "\nHEADER_END\n"
#define COOLSOCKET_HEADER_HEAP_SIZE 8196
#define COOLSOCKET_NO_TIMEOUT -1

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
        int m_timeout = COOLSOCKET_NO_TIMEOUT;
        quint16 m_port = 0;
        QHostAddress m_hostAddress;
        QPointer<ServerWorker> m_worker;
        QList<RequestHandler *> m_ongoingTasks;

    protected:
        void setWorker(ServerWorker *worker)
        {
            this->m_worker = worker;
        }

    public:
        explicit Server(QHostAddress hostAddress, quint16 port = 0, int timeout = COOLSOCKET_NO_TIMEOUT,
                        QObject *parent = nullptr);

        friend class ServerWorker;

        friend class ActiveConnection;

        friend class Response;

        friend class RequestHandler;

        virtual void connected(ActiveConnection *connection) = 0;

        QHostAddress hostAddress() const
        {
            return m_hostAddress;
        }

        quint16 port() const
        {
            return m_port;
        }

        bool serving();

        void setHostAddress(const QHostAddress &hostAddress)
        {
            m_hostAddress = hostAddress;
        }

        void setPort(quint16 port)
        {
            m_port = port;
        }

        void setTimeout(int timeout)
        {
            m_timeout = timeout;
        }

        bool start(int blockingTime = -1);

        bool startEnsured(int blockingTime = -1);

        void stop(int blockingTime = -1);

        int timeout()
        {
            return m_timeout;
        }

        ServerWorker *worker() const
        {
            return m_worker;
        }

    signals:

        void clientConnected(ActiveConnection *connection);

        void serverStarted();

        void serverStopped();

        void serverFailure();
    };

    class ActiveConnection : public QObject {
    Q_OBJECT
        QTcpSocket *m_activeSocket;
        int m_timeout = 2000;

    public:
        explicit ActiveConnection(QTcpSocket *tcpServer, int msecTimeout = 2000,
                                  QObject *parent = nullptr)
                : QObject(parent)
        {
            this->m_activeSocket = tcpServer;
            this->m_timeout = msecTimeout;
        }

        ~ActiveConnection() override
        {
            if (this->m_activeSocket->isOpen())
                this->m_activeSocket->close();

            delete m_activeSocket;
        }

        QTcpSocket *getSocket()
        {
            return m_activeSocket;
        }

        int getTimeout()
        {
            return m_timeout;
        }

        void setTimeout(int msecs)
        {
            this->m_timeout = msecs;
        }

        void reply(const QJsonObject &reply);

        void reply(const char *reply);

        Response receive();
    };

    class Response {
    public:
        QString response;
        QJsonObject headerIndex;
        string::size_type length = 0;

        Response() = default;

        virtual ~Response() = default;

        QJsonObject asJson() const
        {
            return QJsonDocument::fromJson(QByteArray::fromStdString(response.toStdString()))
                    .object();
        }
    };

    class ServerWorker : public QThread {
    Q_OBJECT
        QTcpServer *m_tcpServer;
        Server *m_server;
        bool m_serverListening = false;

    public:
        explicit ServerWorker(Server *server, QObject *parent = nullptr);

        bool serving()
        {
            return isRunning() && m_serverListening;
        }

        QTcpServer *tcpServer()
        {
            return m_tcpServer;
        }

        void setTcpServer(QTcpServer *server);

    protected:
        void run() override;
    };

    class RequestHandler : public QThread {
    Q_OBJECT
        Server *m_server;
        ActiveConnection *m_connection;

    public:
        RequestHandler(Server *server, ActiveConnection *connection, QObject *parent = nullptr)
                : QThread(parent)
        {
            this->m_server = server;
            this->m_connection = connection;
        }

    protected:
        void run() override;
    };

    class Client : public QObject {
    Q_OBJECT

    public:
        explicit Client(QObject *parent = nullptr) : QObject(parent)
        {
        }

        static ActiveConnection *openConnection(const QObject *sender,
                                                const QHostAddress &hostName,
                                                quint16 port,
                                                int timeoutMSeconds = 3000);
    };
}

#endif // COOLSOCKET_H
