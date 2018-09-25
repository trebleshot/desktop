#ifndef COMMUNICATIONSERVER_H
#define COMMUNICATIONSERVER_H

#include "config.h"
#include "coolsocket.h"
#include "keyword.h"

class CommunicationServer : public CoolSocket::Server {
public:
    CommunicationServer(QObject* parent = 0);

    void connected(CoolSocket::ActiveConnection* connection);
};

#endif // COMMUNICATIONSERVER_H
