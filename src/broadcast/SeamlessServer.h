//
// Created by veli on 2/9/19.
//

#ifndef TREBLESHOT_SEAMLESSSERVER_H
#define TREBLESHOT_SEAMLESSSERVER_H

#include "src/coolsocket/CoolSocket.h"
#include "src/config/Config.h"
#include "src/config/Keyword.h"

class SeamlessServer : public CoolSocket::Server {
Q_OBJECT

public:
    explicit SeamlessServer(QObject *parent = nullptr);

    void connected(CoolSocket::ActiveConnection *connection) override;
};


#endif //TREBLESHOT_SEAMLESSSERVER_H
