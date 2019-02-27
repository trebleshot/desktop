#include "CoolSocket.h"

namespace CoolSocket {
    Server::Server(QHostAddress hostAddress, quint16 port, int timeout, QObject *parent)
            : QObject(parent)
    {
        setWorker(new ServerWorker(this));
        setTimeout(timeout);
        setHostAddress(hostAddress);
        setPort(port);
    }

    bool Server::serving()
    {
        return worker()->serving();
    }

    bool Server::start(int blockingTime)
    {
        if (serving())
            return true;

        worker()->start();

        if (blockingTime >= 0) {
            time_t timeout = clock() + blockingTime;
            while (!serving() && timeout >= clock()) {
            }
        }

        return worker()->serving();
    }

    bool Server::startEnsured(int blockingTime)
    {
        start(blockingTime);

        if (blockingTime >= 0 && worker() != nullptr) {
            time_t timeout = clock() + blockingTime;
            while (worker()->tcpServer() != nullptr
                   && !worker()->tcpServer()->isListening()
                   && timeout >= clock()) {
            }
        }

        return worker() != nullptr
               && worker()->tcpServer() != nullptr
               && worker()->tcpServer()->isListening();
    }

    void Server::stop(int blockingTime)
    {
        if (worker()->isRunning()) {
            worker()->requestInterruption();

            if (blockingTime >= 0) {
                time_t timeout = clock() + blockingTime;
                while (worker()->serving() && timeout >= clock()) {
                }
            }
        }
    }

    void ActiveConnection::reply(const QJsonObject &reply)
    {
        this->reply(QJsonDocument(reply).toJson().toStdString().c_str());
    }

    void ActiveConnection::reply(const char *reply)
    {
        qDebug() << this << " : Entered write sequence";

        QByteArray replyImpl(reply);

        QJsonObject headerIndex{
                {QString(COOLSOCKET_KEYWORD_LENGTH), QJsonValue(replyImpl.size())}
        };

        m_activeSocket->write(QJsonDocument(headerIndex).toJson());
        m_activeSocket->write(COOLSOCKET_HEADER_DIVIDER);
        m_activeSocket->flush();

        m_activeSocket->write(replyImpl);
        m_activeSocket->flush();

        while (m_activeSocket->bytesToWrite() != 0) {
            if (!m_activeSocket->waitForBytesWritten(getTimeout() < 1000 ? 1000 : getTimeout())) {
                qDebug() << this << "Timed out !!!";
                throw exception();
            }
        }

        qDebug() << this << " : Exited write sequence";
    }

    Response ActiveConnection::receive()
    {
        qDebug() << this << " : Entered read sequence";

        Response response;
        size_t headerPosition = string::npos;
        string headerData;
        string contentData;

        clock_t lastDataAvailable = clock();

        while (m_activeSocket->isReadable()) {
            if (headerPosition == string::npos) {
                if (m_activeSocket->waitForReadyRead(2000)) {
                    headerData.append(m_activeSocket->readAll());
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
                if (m_activeSocket->waitForReadyRead(2000)) {
                    contentData.append(m_activeSocket->readAll());
                    lastDataAvailable = clock();
                }

                if (contentData.length() >= response.length) {
                    response.response = QByteArray::fromStdString(contentData);
                    break;
                }
            }

            if (getTimeout() >= 0 && (clock() - lastDataAvailable) > getTimeout())
                throw exception();
        }

        qDebug() << this << " : Exited read sequence";

        return response;
    }

    ServerWorker::ServerWorker(Server *server, QObject *parent)
            : QThread(parent)
    {
        m_server = server;
        setTcpServer(new QTcpServer());
    }

    void ServerWorker::setTcpServer(QTcpServer *server)
    {
        this->m_tcpServer = server;
    }

    void ServerWorker::run()
    {
        try {
            this->setTcpServer(new QTcpServer());

            if (tcpServer()->listen(m_server->hostAddress(), m_server->port())) {
                emit m_server->serverStarted();

                while (!isInterruptionRequested() && tcpServer()->isListening()) {
                    this->m_serverListening = true;

                    tcpServer()->waitForNewConnection(2000);

                    if (tcpServer()->hasPendingConnections()) {
                        QTcpSocket *socket = tcpServer()->nextPendingConnection();
                        auto *activeConnection = new ActiveConnection(socket, m_server->timeout());
                        auto *handler = new RequestHandler(m_server, activeConnection);

                        handler->start();
                    }
                }

                this->m_serverListening = false;
                tcpServer()->close();

                emit m_server->serverStopped();
            }
        } catch (...) {
            emit m_server->serverFailure();
        }
    }

    void RequestHandler::run()
    {
        m_server->m_ongoingTasks.append(this);

        connect(this, SIGNAL(finished()), this->m_connection->getSocket(), SLOT(deleteLater()));

        this->m_connection->getSocket()->waitForConnected(2000);
        emit m_server->clientConnected(this->m_connection);
        m_server->connected(this->m_connection);

        m_server->m_ongoingTasks.removeOne(this);

        delete this->m_connection;
    }

    ActiveConnection *Client::openConnection(const QObject *sender,
                                             const QHostAddress &hostName,
                                             quint16 port,
                                             int timeoutMSeconds)
    {
        auto *socket = new QTcpSocket;
        auto *connection = new ActiveConnection(socket);

        QTcpSocket::connect(sender, SIGNAL(destroyed()), connection, SLOT(deleteLater()));

        socket->connectToHost(hostName, port);

        while (QAbstractSocket::SocketState::ConnectingState == socket->state())
            socket->waitForConnected(timeoutMSeconds);

        if (QAbstractSocket::SocketState::ConnectedState != socket->state())
            throw exception();

        return connection;
    }
} // namespace CoolSocket
