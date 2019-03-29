//
// Created by veli on 3/6/19.
//

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