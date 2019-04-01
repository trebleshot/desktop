/*
* Copyright (C) 2019 Veli Tasalı
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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