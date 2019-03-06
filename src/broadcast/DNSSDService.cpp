//
// Created by veli on 3/6/19.
//

#include "DNSSDService.h"

DNSSDService::DNSSDService(QObject *parent) : QObject(parent)
{
    m_serviceBroadcast = new KDNSSD::PublicService(TS_SERVICE_NAME, TS_SERVICE_TYPE, PORT_COMMUNICATION_DEFAULT);
    m_serviceBroadcast->publish();

    m_serviceBrowser = new KDNSSD::ServiceBrowser(QStringLiteral(TS_SERVICE_NAME), true);
    m_serviceBrowser->startBrowse();

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
        NetworkDeviceLoader::loadAsynchronously(this, resolvedAddress, nullptr);
}
