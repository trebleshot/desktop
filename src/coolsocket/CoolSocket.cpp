#include <src/util/GThread.h>
#include "CoolSocket.h"

CSServer::CSServer(QHostAddress hostAddress, quint16 port, int timeout, QObject *parent)
        : QObject(parent), m_server(new QTcpServer(this))
{
    setTimeout(timeout);
    setHostAddress(hostAddress);
    setPort(port);

    connect(m_server, &QTcpServer::newConnection, this, &CSServer::connectionRequest);
}

CSServer::~CSServer()
{
    stop();
    delete m_server;
}

void CSServer::connectionRequest()
{
    QTcpSocket *socket = server()->nextPendingConnection();
    auto *connection = new CSActiveConnection(socket, timeout());
    connected(connection);
    delete connection;
    /*auto *thread = new GThread([this, socket](GThread *thread) {
        auto *connection = new CSActiveConnection(socket, timeout(), thread);
        connected(connection);
        delete connection;
    }, true);

    thread->start();*/
}

bool CSServer::serving()
{
    return server()->isListening();
}

bool CSServer::start()
{
    if (serving())
        return true;

    return server()->listen(hostAddress(), port());
}

bool CSServer::stop()
{
    server()->close();
    return !serving();
}

void CSActiveConnection::reply(const QJsonObject &reply)
{
    this->reply(QJsonDocument(reply).toJson().toStdString().c_str());
}

void CSActiveConnection::reply(const char *reply)
{
    qDebug() << this << " : Entered write sequence";

    QByteArray replyImpl(reply);

    QJsonObject headerIndex{
            {QString(COOLSOCKET_KEYWORD_LENGTH), QJsonValue(replyImpl.size())}
    };

    socket()->write(QJsonDocument(headerIndex).toJson());
    socket()->write(COOLSOCKET_HEADER_DIVIDER);
    socket()->flush();

    socket()->write(replyImpl);
    socket()->flush();

    while (socket()->bytesToWrite() != 0) {
        if (!socket()->waitForBytesWritten(timeout() < 1000 ? 1000 : timeout())) {
            qDebug() << this << "Timed out !!!";
            throw exception();
        }
    }

    qDebug() << this << " : Exited write sequence";
}

CSResponse CSActiveConnection::receive()
{
    qDebug() << this << " : Entered read sequence";

    CSResponse response;
    size_t headerPosition = string::npos;
    string headerData;
    string contentData;

    clock_t lastDataAvailable = clock();

    while (socket()->isReadable()) {
        if (headerPosition == string::npos) {
            if (socket()->waitForReadyRead(2000)) {
                headerData.append(socket()->readAll());
                lastDataAvailable = clock();
            }

            headerPosition = headerData.find(COOLSOCKET_HEADER_DIVIDER);

            if (headerPosition != string::npos) {
                size_t dividerOccupiedSize = sizeof COOLSOCKET_HEADER_DIVIDER + headerPosition - 1;

                if (headerData.length() > dividerOccupiedSize)
                    contentData.append(headerData.substr(dividerOccupiedSize));

                headerData.resize(headerPosition);

                QJsonObject jsonObject = QJsonDocument::fromJson(QByteArray::fromStdString(headerData))
                        .object();

                if (jsonObject.contains(QString(COOLSOCKET_KEYWORD_LENGTH))) {
                    response.length = jsonObject.value(QString(COOLSOCKET_KEYWORD_LENGTH))
                            .toVariant()
                            .toUInt();
                } else
                    break;

                response.headerIndex = jsonObject;
            }

            if (headerData.length() > COOLSOCKET_HEADER_HEAP_SIZE) {
                qDebug() << "Header exceeds heap size: " << headerData.length();
                throw exception();
            }
        } else {
            if (socket()->waitForReadyRead(2000)) {
                contentData.append(socket()->readAll());
                lastDataAvailable = clock();
            }

            if (contentData.length() >= response.length) {
                response.response = QByteArray::fromStdString(contentData);
                break;
            }
        }

        if (timeout() >= 0 && (clock() - lastDataAvailable) > timeout())
            throw exception();
    }

    qDebug() << this << " : Exited read sequence";

    return response;
}

CSActiveConnection *CSClient::openConnection(const QObject *sender,
                                             const QHostAddress &hostName,
                                             quint16 port,
                                             int timeoutMSeconds)
{
    auto *socket = new QTcpSocket;
    auto *connection = new CSActiveConnection(socket);

    QTcpSocket::connect(sender, SIGNAL(destroyed()), connection, SLOT(deleteLater()));

    socket->connectToHost(hostName, port);

    while (QAbstractSocket::SocketState::ConnectingState == socket->state())
        socket->waitForConnected(timeoutMSeconds);

    if (QAbstractSocket::SocketState::ConnectedState != socket->state())
        throw exception();

    return connection;
}
