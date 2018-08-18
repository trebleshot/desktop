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
    Response* response = connection->receive();

    cout << "length: " << response->length << endl;
    cout << "content: " << response->response->toStdString() << endl;

    connection->reply("Thank you all");
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

    int headerPosition = string::npos;
    string* headerData = new string;
    string* contentData = new string;

    while (activeSocket->isReadable()) {
        if (activeSocket->waitForReadyRead(2000)) {
            if (headerPosition == string::npos) {
                headerData->append(activeSocket->readAll());
                headerPosition = headerData->find(COOLSOCKET_HEADER_DIVIDER);

                if (headerData->length() > COOLSOCKET_HEADER_HEAP_SIZE) {
                    cerr << "Header exceeds heap size: " << headerData->length();
                    break;
                }

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
            } else {
                contentData->append(activeSocket->readAll());

                if (contentData->length() >= response->length) {
                    response->response = new QString(QByteArray::fromStdString(*contentData));
                    break;
                }
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
