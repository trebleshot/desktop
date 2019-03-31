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
	explicit DNSSDService(QObject* parent = nullptr);

	void start();

	~DNSSDService() override;

public slots:

	void serviceFound(KDNSSD::RemoteService::Ptr service);

protected:
	KDNSSD::PublicService *m_serviceBroadcast;
	KDNSSD::ServiceBrowser *m_serviceBrowser;
};
#else
#include <QObject>

class DNSSDService : public QObject {
	Q_OBJECT

public:
	explicit DNSSDService(QObject* parent = nullptr);

	void start();
};
#endif // USE_DNSSD_FEATURE