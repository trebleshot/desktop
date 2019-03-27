//
// Created by veli on 3/6/19.
//
#ifndef TREBLESHOT_BONJOURSERVICE_H
#define TREBLESHOT_BONJOURSERVICE_H

#include <KDNSSD/DNSSD/PublicService>
#include <KDNSSD/DNSSD/ServiceBrowser>
#include <src/util/NetworkDeviceLoader.h>

class DNSSDService : public QObject {
	Q_OBJECT

public:
	explicit DNSSDService(QObject* parent = nullptr);

	void start();

#ifdef USE_DNSSD_FEATURE
	~DNSSDService() override;

public slots:

	void serviceFound(KDNSSD::RemoteService::Ptr service);

protected:
	KDNSSD::PublicService *m_serviceBroadcast;
	KDNSSD::ServiceBrowser *m_serviceBrowser;
#endif // USE_DNSSD_FEATURE
};
#endif //TREBLESHOT_BONJOURSERVICE_H
