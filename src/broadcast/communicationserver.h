#ifndef COMMUNICATIONSERVER_H
#define COMMUNICATIONSERVER_H

#include "src/config/config.h"
#include "src/coolsocket/coolsocket.h"
#include "src/config/keyword.h"

class CommunicationServer : public CoolSocket::Server {
public:
    CommunicationServer(QObject* parent = nullptr);

    void connected(CoolSocket::ActiveConnection* connection) override;
};

#endif // COMMUNICATIONSERVER_H
