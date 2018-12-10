#ifndef COMMUNICATIONSERVER_H
#define COMMUNICATIONSERVER_H

#include "src/config/Config.h"
#include "src/coolsocket/CoolSocket.h"
#include "src/config/Keyword.h"

class CommunicationServer : public CoolSocket::Server {
    void pushReply(CoolSocket::ActiveConnection *activeConnection, QJsonObject json, bool result);
public:
    CommunicationServer(QObject *parent = nullptr);

    void connected(CoolSocket::ActiveConnection *connection) override;
};

#endif // COMMUNICATIONSERVER_H
