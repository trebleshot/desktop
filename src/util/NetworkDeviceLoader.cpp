//
// Created by veli on 9/28/18.
//

#include <QtCore/QJsonObject>
#include <sys/socket.h>
#include "NetworkDeviceLoader.h"
#include "AppUtils.h"
#include "CommunicationBridge.h"
#include "GThread.h"

DeviceConnection *
NetworkDeviceLoader::processConnection(NetworkDevice *device, const QString ipAddress)
{
    DeviceConnection *connection = new DeviceConnection(ipAddress);

    processConnection(device, connection);

    return connection;
}

void NetworkDeviceLoader::processConnection(NetworkDevice *device,
                                            DeviceConnection *connection)
{
    try {
        gDbSignal->reconstruct(connection);
    } catch (exception &e) {
        AppUtils::applyAdapterName(connection);
    }

    connection->lastCheckedDate = clock();
    connection->deviceId = device->deviceId;

    auto *sqlSelection = new SqlSelection();

    sqlSelection->setTableName(AccessDatabaseStructure::TABLE_DEVICECONNECTION)
            ->setWhere(QString("`%1` = ? AND `%2` = ?")
                               .arg(AccessDatabaseStructure::FIELD_DEVICECONNECTION_DEVICEID)
                               .arg(AccessDatabaseStructure::FIELD_DEVICECONNECTION_ADAPTERNAME));

    sqlSelection->whereArgs << QVariant(connection->deviceId)
                            << QVariant(connection->adapterName);

    gDbSignal->remove(sqlSelection);
    gDbSignal->publish(connection);
}


void NetworkDeviceLoader::loadAsynchronously(QObject *sender,
                                             const QString &ipAddress,
                                             const std::function<void(NetworkDevice *)> &listener)
{
    auto *testThread = new GThread([sender, ipAddress, listener](QThread *thisThread) {
        listener(load(sender, ipAddress));
    });

    QObject::connect(testThread, &QThread::finished, testThread, &QThread::deleteLater);

    testThread->start();
}

NetworkDevice *NetworkDeviceLoader::load(QObject *sender, const QString &ipAddress)
{
    try {
        auto *bridge = new CommunicationBridge(sender);
        auto *device = bridge->loadDevice(ipAddress);

        if (device->deviceId != nullptr) {
            NetworkDevice *localDevice = AppUtils::getLocalDevice();
            DeviceConnection *connection = processConnection(device, ipAddress);

            delete localDevice;
            delete connection;
        }

        delete bridge;
        return device;
    } catch (...) {
        // do nothing
    }

    return nullptr;
}

NetworkDevice *NetworkDeviceLoader::loadFrom(const QJsonObject jsonIndex)
{
    QJsonObject deviceInfo = jsonIndex.value(KEYWORD_DEVICE_INFO).toObject();
    QJsonObject appInfo = jsonIndex.value(KEYWORD_APP_INFO).toObject();

    NetworkDevice *networkDevice = new NetworkDevice(deviceInfo.value(KEYWORD_DEVICE_INFO_SERIAL).toString());

    gDbSignal->reconstruct(networkDevice);

    networkDevice->brand = deviceInfo.value(KEYWORD_DEVICE_INFO_BRAND).toString();
    networkDevice->model = deviceInfo.value(KEYWORD_DEVICE_INFO_MODEL).toString();
    networkDevice->nickname = deviceInfo.value(KEYWORD_DEVICE_INFO_USER).toString();
    networkDevice->lastUsageTime = clock();
    networkDevice->versionNumber = appInfo.value(KEYWORD_APP_INFO_VERSION_CODE).toInt();
    networkDevice->versionName = appInfo.value(KEYWORD_APP_INFO_VERSION_NAME).toString();

    if (networkDevice->nickname.length() > NICKNAME_LENGTH_MAX)
        networkDevice->nickname = networkDevice->nickname.left(NICKNAME_LENGTH_MAX - 1);

    return networkDevice;
}
