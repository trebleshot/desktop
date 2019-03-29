//
// Created by veli on 2/9/19.
//

#pragma once

#include <src/util/Interrupter.h>
#include "src/coolsocket/CoolSocket.h"
#include "src/config/Config.h"
#include "src/config/Keyword.h"

class SeamlessServer : public CSServer {
Q_OBJECT

public:
    explicit SeamlessServer(QObject *parent = nullptr);

    void connected(CSActiveConnection *connection) override;

signals:

    void taskStarted(groupid groupId, QString deviceId);

    void taskDone(groupid groupId, QString deviceId);
};

class Thread_SeamlessServer : public QThread {
	Q_OBJECT

public:
	explicit Thread_SeamlessServer(QObject *parent = nullptr);

	~Thread_SeamlessServer();

	SeamlessServer* server();

	SeamlessServer* operator->() {
		return m_server;
	}
protected:
	SeamlessServer* m_server;

	void run() override;
};