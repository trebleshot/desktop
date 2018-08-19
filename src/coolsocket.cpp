#include "coolsocket.h"

namespace CoolSocket {
Server::Server(QHostAddress hostAddress, int port, QObject* parent)
    : QObject(parent)
{
    setWorker(new ServerWorker(this));

    this->setHostAddress(hostAddress);
    this->setPort(port);
}

bool Server::isRunning()
{
    return getWorker()->isRunning();
}

void Server::start(bool block)
{
    getWorker()->start();

    if (block)
        while (!isRunning()) {
        }
}

void Server::stop(bool block)
{
    if (isRunning()) {
        getWorker()->requestInterruption();

        if (block)
            while (isRunning()) {
            }
    }
}

void ActiveConnection::reply(char* reply)
{
    QByteArray replyImpl(reply);

    QJsonObject headerIndex{
        { QString(COOLSOCKET_KEYWORD_LENGTH), QJsonValue(replyImpl.length()) }
    };

    activeSocket->write(QJsonDocument(headerIndex).toJson());
    activeSocket->write(COOLSOCKET_HEADER_DIVIDER);
    activeSocket->flush();

    activeSocket->write(replyImpl);
    activeSocket->flush();

    if (!activeSocket->waitForBytesWritten(getTimeout() < 0 ? 2000 : getTimeout())) {
        if (getTimeout() < 0)
            throw exception();
    }
}

Response* ActiveConnection::receive()
{
    Response* response = new Response;

    size_t headerPosition = string::npos;
    string* headerData = new string;
    string* contentData = new string;

    while (activeSocket->isReadable()) {
        if (activeSocket->waitForReadyRead(getTimeout() < 0 ? 2000 : getTimeout())) {
            if (headerPosition == string::npos) {
                headerData->append(activeSocket->readAll());
                headerPosition = headerData->find(COOLSOCKET_HEADER_DIVIDER);

                if (headerPosition != string::npos) {
                    long int dividerOccupiedSize = sizeof COOLSOCKET_HEADER_DIVIDER + headerPosition;

                    if (headerData->length() > dividerOccupiedSize)
                        contentData->append(headerData->substr(dividerOccupiedSize));

                    headerData->resize(headerPosition);

                    QJsonObject jsonObject = QJsonDocument::fromJson(QByteArray::fromStdString(*headerData)).object();

                    if (jsonObject.contains(QString(COOLSOCKET_KEYWORD_LENGTH))) {
                        response->length = (jsonObject.value(QString(COOLSOCKET_KEYWORD_LENGTH))).toInt();
                    } else
                        break;

                    response->headerIndex = &jsonObject;
                }

                if (headerData->length() > COOLSOCKET_HEADER_HEAP_SIZE) {
                    cerr << "Header exceeds heap size: " << headerData->length();
                    break;
                }
            } else {
                contentData->append(activeSocket->readAll());

                if (contentData->length() >= response->length) {
                    response->response = new QString(QByteArray::fromStdString(*contentData));
                    break;
                }
            }
        } else if (getTimeout() >= 0)
            throw exception();
    }

    return response;
}

ServerWorker::ServerWorker(Server* server, QObject* parent)
    : QThread(parent)
{
    this->server = server;
    this->setTcpServer(new QTcpServer());
}

void ServerWorker::setTcpServer(QTcpServer* server) { this->tcpServer = server; }

void ServerWorker::run()
{
    setTcpServer(new QTcpServer());
    getTcpServer()->listen(server->getHostAddress(), server->getPort());

    connect(this, SIGNAL(finished()), getTcpServer(), SLOT(deleteLater()));

    while (!isInterruptionRequested() && getTcpServer()->isListening()) {
        getTcpServer()->waitForNewConnection(2000);

        if (getTcpServer()->hasPendingConnections()) {
            QTcpSocket* socket = getTcpServer()->nextPendingConnection();
            ActiveConnection* activeConnection = new ActiveConnection(socket);

            (new RequestHandler(server, activeConnection))->start();
        }
    }
}
}
