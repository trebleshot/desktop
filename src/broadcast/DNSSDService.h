//
// Created by veli on 3/6/19.
//
#ifndef TREBLESHOT_BONJOURSERVICE_H
#define TREBLESHOT_BONJOURSERVICE_H

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

#endif //TREBLESHOT_BONJOURSERVICE_H
