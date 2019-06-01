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

#include "DNSSDService.h"

#ifdef USE_DNSSD_FEATURE

DNSSDService::DNSSDService(QObject *parent)
		: QObject(parent),
		  m_serviceBroadcast(new KDNSSD::PublicService(TS_SERVICE_NAME, TS_SERVICE_TYPE, PORT_COMMUNICATION_DEFAULT)),
		  m_serviceBrowser(new KDNSSD::ServiceBrowser(QStringLiteral(TS_SERVICE_TYPE), true))
{
	connect(m_serviceBrowser, &KDNSSD::ServiceBrowser::serviceAdded, this, &DNSSDService::serviceFound);
}

DNSSDService::~DNSSDService()
{
	delete m_serviceBrowser;
	delete m_serviceBroadcast;
}

void DNSSDService::serviceFound(KDNSSD::RemoteService::Ptr service)
{
	const auto &resolvedAddress = KDNSSD::ServiceBrowser::resolveHostName(service.data()->hostName());

	if (!resolvedAddress.isNull())
		NetworkDeviceLoader::loadAsynchronously(resolvedAddress, nullptr);
}

void DNSSDService::start()
{
	m_serviceBrowser->startBrowse();
	m_serviceBroadcast->publish();
}

#else
DNSSDService::DNSSDService(QObject *parent)
		: QObject(parent), m_hostname(&this->m_server), m_provider(&this->m_server, &this->m_hostname),
		  m_browser(&this->m_server, TS_SERVICE_TYPE, &this->m_cache)
{
	connect(&m_browser, &QMdnsEngine::Browser::serviceAdded, this, &DNSSDService::loadService);
	connect(&m_browser, &QMdnsEngine::Browser::serviceUpdated, this, &DNSSDService::loadService);
}

void DNSSDService::start()
{
	m_service.setType(TS_SERVICE_TYPE);
	m_service.setName(TS_SERVICE_NAME);
	m_service.setPort(PORT_COMMUNICATION_DEFAULT);
	m_provider.update(m_service);
}

void DNSSDService::loadService(const QMdnsEngine::Service &service)
{
	if (service.port() == PORT_COMMUNICATION_DEFAULT) {
		auto *resolver = new QMdnsEngine::Resolver(&m_server, service.hostname(), &m_cache);
		connect(resolver, &QMdnsEngine::Resolver::resolved, [](const QHostAddress &address) {
			NetworkDeviceLoader::loadAsynchronously(address, nullptr);
		});
	}
}

DNSSDService::~DNSSDService()
{
	qDeleteAll(m_resolverList);
}

#endif // USE_DNSSD_FEATURE