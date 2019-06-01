/*
* Copyright (C) 2019 Veli Tasalı, created on 3/6/19
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

#ifdef USE_DNSSD_FEATURE

#include <KDNSSD/DNSSD/PublicService>
#include <KDNSSD/DNSSD/ServiceBrowser>
#include <src/util/NetworkDeviceLoader.h>

class DNSSDService : public QObject {
Q_OBJECT

public:
	explicit DNSSDService(QObject *parent = nullptr);

	void start();

	~DNSSDService() override;

public slots:

	static void serviceFound(KDNSSD::RemoteService::Ptr service);

protected:
	KDNSSD::PublicService *m_serviceBroadcast;
	KDNSSD::ServiceBrowser *m_serviceBrowser;
};

#else
#include <QObject>
#include <qmdnsengine/server.h>
#include <qmdnsengine/service.h>
#include <qmdnsengine/hostname.h>
#include <qmdnsengine/cache.h>
#include <qmdnsengine/browser.h>
#include <qmdnsengine/provider.h>
#include <qmdnsengine/resolver.h>
#include <src/util/NetworkDeviceLoader.h>

class DNSSDService : public QObject {
	Q_OBJECT

public:
	explicit DNSSDService(QObject* parent = nullptr);

	~DNSSDService() override;

	void start();

public slots:
	void loadService(const QMdnsEngine::Service &service);

protected:
	QMdnsEngine::Server m_server;
	QMdnsEngine::Hostname m_hostname;

	QMdnsEngine::Service m_service;
	QMdnsEngine::Provider m_provider;

	QMdnsEngine::Cache m_cache;
	QMdnsEngine::Browser m_browser;
	QList<QMdnsEngine::Resolver *> m_resolverList;
};
#endif // USE_DNSSD_FEATURE