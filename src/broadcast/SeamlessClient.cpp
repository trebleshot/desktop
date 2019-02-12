//
// Created by veli on 2/9/19.
//

#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <src/util/CommunicationBridge.h>
#include <src/database/object/TransferGroup.h>
#include "SeamlessClient.h"

SeamlessClient::SeamlessClient(const QString &deviceId, quint32 groupId, QObject *parent)
        : QThread(parent), m_deviceId(deviceId), m_groupId(groupId)
{
}

void SeamlessClient::run()
{
    auto *localDevice = AppUtils::getLocalDevice();
    auto *device = new NetworkDevice(m_deviceId);
    auto *group = new TransferGroup(m_groupId);
    auto *assignee = new TransferAssignee(m_groupId, m_deviceId, nullptr, this);
    auto *connection = new DeviceConnection(); // Adapter name passed when assignee reconstruction is successful
    auto *client = new CommunicationBridge;
    bool retry = false;

    auto connectionLambda = [connection, assignee]() -> DeviceConnection * {
        connection->deviceId = assignee->deviceId;
        connection->adapterName = assignee->connectionAdapter;

        return connection;
    };

    if (gDbSignal->reconstruct(device)
        && gDbSignal->reconstruct(group)
        && gDbSignal->reconstruct(assignee)
        && gDbSignal->reconstruct(connectionLambda())) {

        client->setDevice(device);
    } else {
        qDebug() << "Could not produce information within given group id"
                 << m_groupId
                 << "and device id"
                 << m_deviceId;
    }

    delete client;
    delete connection;
    delete assignee;
    delete group;
    delete device;
    delete localDevice;
}
