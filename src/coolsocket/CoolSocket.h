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

class CSResponse;

class CSActiveConnection;

class CSClient;

class CSServer : public QObject {
Q_OBJECT
    int m_timeout = COOLSOCKET_NO_TIMEOUT;
    quint16 m_port = 0;
    QHostAddress m_hostAddress;
    QTcpServer *m_server;
    QMap<CSActiveConnection *, QTcpSocket *> mConnections;

public:
    explicit CSServer(QHostAddress hostAddress, quint16 port = 0, int timeout = COOLSOCKET_NO_TIMEOUT,
                      QObject *parent = nullptr);

    ~CSServer() override;

    friend class CSActiveConnection;

    friend class CSResponse;

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

    QTcpServer *server()
    {
        return m_server;
    }

    bool start();

    bool stop();

    int timeout()
    {
        return m_timeout;
    }

protected slots:

    void connectionRequest();

    virtual void connected(CSActiveConnection *connection) = 0;
};


class CSActiveConnection : public QObject {
Q_OBJECT
    int m_timeout = 2000;
    QTcpSocket *m_socket;

public:
    explicit CSActiveConnection(QTcpSocket *socket, int msecTimeout = 2000, QObject *parent = nullptr)
            : QObject(parent)
    {
        m_socket = socket;
        m_timeout = msecTimeout;

        connect(this, &CSActiveConnection::remoteReceive,
                this, &CSActiveConnection::receive, Qt::BlockingQueuedConnection);
        connect(this, SIGNAL(remoteReply(const char * )),
                this, SLOT(reply(const char * )), Qt::BlockingQueuedConnection);
        connect(this, SIGNAL(remoteReply(const QJsonObject & )),
                this, SLOT(reply(const QJsonObject & )), Qt::BlockingQueuedConnection);
    }

    ~CSActiveConnection() override
    {
        if (this->m_socket->isOpen())
            this->m_socket->close();

        delete m_socket;
    }

    void setTimeout(int msecs)
    {
        this->m_timeout = msecs;
    }

    QTcpSocket *socket()
    {
        return m_socket;
    }

    int timeout()
    {
        return m_timeout;
    }

public slots:
    void reply(const QJsonObject &reply);

    void reply(const char *reply);

    CSResponse receive();

signals:

    CSResponse remoteReceive();

    void remoteReply(const QJsonObject &reply);

    void remoteReply(const char *reply);

};

class CSResponse {
public:
    QString response;
    QJsonObject headerIndex;
    string::size_type length = 0;

    CSResponse() = default;

    virtual ~CSResponse() = default;

    QJsonObject asJson() const
    {
        return QJsonDocument::fromJson(QByteArray::fromStdString(response.toStdString())).object();
    }
};

class CSClient : public QObject {
Q_OBJECT

public:
    explicit CSClient(QObject *parent = nullptr) : QObject(parent)
    {
    }

    static CSActiveConnection *openConnection(const QObject *sender, const QHostAddress &hostName, quint16 port,
                                              int timeoutMSeconds = 3000);
};

#endif // COOLSOCKET_H
