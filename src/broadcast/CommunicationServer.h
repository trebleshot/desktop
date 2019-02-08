#ifndef COMMUNICATIONSERVER_H
#define COMMUNICATIONSERVER_H

#include "src/config/Config.h"
#include "src/coolsocket/CoolSocket.h"
#include "src/config/Keyword.h"
#include <QApplication>
#include <src/util/AppUtils.h>

class CommunicationServer : public CoolSocket::Server {
Q_OBJECT

    void pushReply(CoolSocket::ActiveConnection *activeConnection, QJsonObject &json, bool result);

public:
    explicit CommunicationServer(QObject *parent = nullptr);

    void connected(CoolSocket::ActiveConnection *connection) override;

signals:

    void textReceived(const QString &text, const QString &deviceId);
};

#endif // COMMUNICATIONSERVER_H
