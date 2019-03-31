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
DNSSDService::DNSSDService(QObject *parent) : QObject(parent)
{}

void DNSSDService::start()
{}
#endif // USE_DNSSD_FEATURE