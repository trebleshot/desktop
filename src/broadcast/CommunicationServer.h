#pragma once

#include <src/config/Config.h>
#include <src/config/Keyword.h>
#include <src/coolsocket/CoolSocket.h>
#include <src/database/object/TransferGroup.h>
#include <src/util/AppUtils.h>
#include <QList>
#include <QHostAddress>
#include <QApplication>

class CommunicationServer : public CSServer {
Q_OBJECT

	void pushReply(CSActiveConnection *activeConnection, QJsonObject &json, bool result);

public:
	explicit CommunicationServer(QObject *parent = nullptr);

	void connected(CSActiveConnection *connection) override;

signals:

	void deviceBlocked(const QString &deviceId, const QHostAddress &address);

	void textReceived(const QString &text, const QString &deviceId);

	void transferRequest(const QString &deviceId, groupid groupId, int filesTotal);

public slots:

	void blockAddress(const QHostAddress &address);

protected:
	QList<QHostAddress> m_blockedAddresses;
};

class Thread_CommunicationServer : public QThread {
Q_OBJECT

public:
	explicit Thread_CommunicationServer(QObject *parent = nullptr);
};