#include "coolsocket.h"

namespace CoolSocket {
Server::Server(QHostAddress hostAddress, int port, QObject* parent)
    : QObject(parent)
{
    setWorker(new ServerWorker(this));

    this->setHostAddress(hostAddress);
    this->setPort(port);
}

bool Server::isServing()
{
    return getWorker()->isServing();
}

bool Server::start(int blockingTime)
{
    if (isServing())
        return true;

    getWorker()->start();

    if (blockingTime >= 0) {
        time_t timeout = clock() + blockingTime;
        while (!isServing() && timeout >= clock()) {
            <
        }
    }

    return getWorker()->isServing();
}

void Server::stop(int blockingTime)
{
    if (getWorker()->isRunning()) {
        getWorker()->requestInterruption();

        if (blockingTime >= 0) {
            time_t timeout = clock() + blockingTime;
            while (getWorker()->isServing() && timeout >= clock()) {
            }
        }
    }
}

void ActiveConnection::reply(const char* reply)
{
    cout << this << " : Entered write sequence" << endl;

    QByteArray replyImpl(reply);

    QJsonObject headerIndex{
        { QString(COOLSOCKET_KEYWORD_LENGTH), QJsonValue(replyImpl.length()) }
    };

    time_t startTime = clock();

    activeSocket->write(QJsonDocument(headerIndex).toJson());
    activeSocket->write(COOLSOCKET_HEADER_DIVIDER);
    activeSocket->flush();

    activeSocket->write(replyImpl);
    activeSocket->flush();

    while (activeSocket->bytesToWrite() != 0) {
        if (getTimeout() >= 0 && (clock() - startTime) > getTimeout())
            throw exception();
    }

    cout << this << " : Exited write sequence" << endl;
}

Response* ActiveConnection::receive()
{
    cout << this << " : Entered read sequence" << endl;

    Response* response = new Response;

    size_t headerPosition = string::npos;
    string* headerData = new string;
    string* contentData = new string;

    time_t lastDataAvailable = clock();

    while (activeSocket->isReadable()) {
        if (headerPosition == string::npos) {
            if (activeSocket->waitForReadyRead(2000)) {
                headerData->append(activeSocket->readAll());
                lastDataAvailable = clock();
            }

            headerPosition = headerData->find(COOLSOCKET_HEADER_DIVIDER);

            if (headerPosition != string::npos) {
                size_t dividerOccupiedSize = sizeof COOLSOCKET_HEADER_DIVIDER + headerPosition - 1;

                if (headerData->length() > dividerOccupiedSize)
                    contentData->append(headerData->substr(dividerOccupiedSize));

                headerData->resize(headerPosition);

                QJsonObject jsonObject = QJsonDocument::fromJson(QByteArray::fromStdString(*headerData))
                                             .object();

                if (jsonObject.contains(QString(COOLSOCKET_KEYWORD_LENGTH))) {
                    response->length = (jsonObject.value(QString(COOLSOCKET_KEYWORD_LENGTH))).toInt();
                } else
                    break;

                response->headerIndex = &jsonObject;
            }

            if (headerData->length() > COOLSOCKET_HEADER_HEAP_SIZE) {
                cerr << "Header exceeds heap size: " << headerData->length();
                throw exception();
            }
        } else {
            if (activeSocket->waitForReadyRead(2000)) {
                contentData->append(activeSocket->readAll());
                lastDataAvailable = clock();
            }

            if (contentData->length() >= response->length) {
                response->response = new QString(QByteArray::fromStdString(*contentData));
                break;
            }
        }

        if (getTimeout() >= 0 && (clock() - lastDataAvailable) > getTimeout())
            throw exception();
    }

    cout << this << " : Exited read sequence" << endl;

    return response;
}

ServerWorker::ServerWorker(Server* server, QObject* parent)
    : QThread(parent)
{
    this->server = server;
    this->setTcpServer(new QTcpServer());
}

void ServerWorker::setTcpServer(QTcpServer* server)
{
    this->tcpServer = server;
}

void ServerWorker::run()
{
    this->setTcpServer(new QTcpServer());

    if (getTcpServer()->listen(server->getHostAddress(), server->getPort())) {
        while (!isInterruptionRequested() && getTcpServer()->isListening()) {
            this->serverListening = true;

            getTcpServer()->waitForNewConnection(2000);

            if (getTcpServer()->hasPendingConnections()) {
                QTcpSocket* socket = getTcpServer()->nextPendingConnection();
                ActiveConnection* activeConnection = new ActiveConnection(socket);
                RequestHandler* handler = new RequestHandler(server, activeConnection);

                handler->start();
            }
        }

        this->serverListening = false;
        getTcpServer()->close();
    }
}

void RequestHandler::run()
{
    server->ongoingTasks.append(this);

    connect(this, SIGNAL(finished()), this->connection->getSocket(), SLOT(deleteLater()));

    this->connection->getSocket()->waitForConnected(2000);
    emit server->clientConnected(this->connection);
    server->connected(this->connection);

    server->ongoingTasks.removeOne(this);

    delete this->connection;
}

ActiveConnection* Client::connect(QString hostAddress, quint16 port, int timeoutMSeconds)
{
    QTcpSocket* socket = new QTcpSocket;
    ActiveConnection* connection = new ActiveConnection(socket);

    socket->connect(this, SIGNAL(finished()), connection, SLOT(deleteLater()));

    socket->connectToHost(hostAddress, port);

    while (QAbstractSocket::SocketState::ConnectingState == socket->state())
        socket->waitForConnected(timeoutMSeconds);

    if (QAbstractSocket::SocketState::ConnectedState != socket->state())
        throw exception();

    return connection;
}
} // namespace CoolSocket
