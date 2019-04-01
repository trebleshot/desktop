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

#define COOLSOCKET_KEYWORD_LENGTH "length"
#define COOLSOCKET_HEADER_DIVIDER "\nHEADER_END\n"
#define COOLSOCKET_HEADER_HEAP_SIZE 8196
#define COOLSOCKET_NO_TIMEOUT -1

#include <src/config/Config.h>
#include <QDataStream>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <iostream>
#include <utility>

using namespace std;

class CSResponse;

class CSActiveConnection;

class CSClient;

class CSServer : public QTcpServer {
Q_OBJECT
	int m_timeout = COOLSOCKET_NO_TIMEOUT;
	quint16 m_port = 0;
	QHostAddress m_hostAddress;

public:
	explicit CSServer(const QHostAddress &hostAddress, quint16 port = 0, int timeout = COOLSOCKET_NO_TIMEOUT,
	                  QObject *parent = nullptr);

	~CSServer() override;

	friend class CSActiveConnection;

	friend class CSResponse;

	QHostAddress hostAddress() const
	{
		return m_hostAddress;
	}

	quint16 port() const
	{
		return m_port;
	}

	void setHostAddress(const QHostAddress &hostAddress)
	{
		m_hostAddress = hostAddress;
	}

	void setPort(quint16 port)
	{
		m_port = port;
	}

	void setTimeout(int timeout)
	{
		m_timeout = timeout;
	}

	bool start();

	bool stop();

	int timeout()
	{
		return m_timeout;
	}

protected slots:

	virtual void connected(CSActiveConnection *connection) = 0;

	void incomingConnection(qintptr handle) override;
};


class CSActiveConnection : public QObject {
Q_OBJECT
	int m_timeout = 2000;
	QTcpSocket *m_socket;

public:
	explicit CSActiveConnection(QTcpSocket *socket, int msecTimeout = 2000, QObject *parent = nullptr)
			: QObject(parent)
	{
		m_socket = socket;
		m_timeout = msecTimeout;
	}

	~CSActiveConnection() override
	{
		if (this->m_socket->isOpen())
			this->m_socket->close();

		delete m_socket;
	}

	void setTimeout(int msecs)
	{
		this->m_timeout = msecs;
	}

	QTcpSocket *socket()
	{
		return m_socket;
	}

	int timeout()
	{
		return m_timeout;
	}

public slots:

	void reply(const QJsonObject &reply);

	void reply(const char *reply);

	CSResponse receive();
};

class CSResponse {
public:
	QString response;
	QJsonObject headerIndex;
	size_t length = 0;

	CSResponse() = default;

	virtual ~CSResponse() = default;

	QJsonObject asJson() const
	{
		return QJsonDocument::fromJson(QByteArray::fromStdString(response.toStdString())).object();
	}
};

class CSClient : public QObject {
Q_OBJECT

public:
	explicit CSClient(QObject *parent = nullptr) : QObject(parent)
	{
	}

	static CSActiveConnection *openConnection(const QHostAddress &hostName, quint16 port,
	                                          int timeoutMSeconds = TIMEOUT_SOCKET_DEFAULT, QObject *sender = nullptr);
};