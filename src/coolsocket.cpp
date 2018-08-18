#include "coolsocket.h"

namespace CoolSocket {
Server::Server(QHostAddress hostAddress, int port, QObject* parent)
    : QObject(parent)
{
    this->setHostAddress(hostAddress);
    this->setPort(port);
}

void Server::start()
{
    worker = new Worker(this);
    worker->start();
}

void Server::handOverRequest(ActiveConnection* connection)
{
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
}

Response* ActiveConnection::receive()
{
    Response* response = new Response;

    bool receivedHeader = false;
    std::string* headerData = new std::string;
    std::string* contentData = new std::string;

    char buffer[8196];

    QByteArray* byteArray = new QByteArray;

    while (true) {
        cout << "headerData: " << *headerData << endl;
        cout << "contentData: " << *contentData << endl;
        cout << "byte data: " << byteArray->toStdString() << endl;

        if (!receivedHeader) {
            byteArray->append(activeSocket->readAll());
            //socket->read(buffer, 8196);

            int enderPosition = headerData->find(COOLSOCKET_HEADER_DIVIDER);

            if (enderPosition != std::string::npos) {
                if (enderPosition + sizeof COOLSOCKET_HEADER_DIVIDER < headerData->length())
                    contentData->append(headerData->substr(enderPosition + sizeof COOLSOCKET_HEADER_DIVIDER - 1));

                headerData->resize(enderPosition);
            }
        }
    }

    return response;
}

Worker::Worker(Server* server, QObject* parent)
    : QThread(parent)
{
    this->server = server;
}

void Worker::run()
{
    setTcpServer(new QTcpServer());
    getTcpServer()->listen(server->getHostAddress(), server->getPort());

    connect(this, SIGNAL(linkRequest(ActiveConnection*)), server, SLOT(handOverRequest(ActiveConnection*)));
    connect(this, SIGNAL(finished()), getTcpServer(), SLOT(deleteLater()));

    while (getTcpServer()->isListening()) {
        getTcpServer()->waitForNewConnection(2000);

        if (getTcpServer()->hasPendingConnections()) {
            QTcpSocket* socket = getTcpServer()->nextPendingConnection();
            ActiveConnection* activeConnection = new ActiveConnection(socket);

            emit linkRequest(activeConnection);
        }
    }
}
}
