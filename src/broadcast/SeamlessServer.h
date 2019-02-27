//
// Created by veli on 2/9/19.
//

#ifndef TREBLESHOT_SEAMLESSSERVER_H
#define TREBLESHOT_SEAMLESSSERVER_H

#include <src/util/Interrupter.h>
#include "src/coolsocket/CoolSocket.h"
#include "src/config/Config.h"
#include "src/config/Keyword.h"

class SeamlessServer : public CSServer, public Interrupter {
Q_OBJECT

public:
    explicit SeamlessServer(QObject *parent = nullptr);

    void connected(CSActiveConnection *connection) override;
};


#endif //TREBLESHOT_SEAMLESSSERVER_H
