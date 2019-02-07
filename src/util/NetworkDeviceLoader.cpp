//
// Created by veli on 9/28/18.
//

#include <QtCore/QJsonObject>
#include "NetworkDeviceLoader.h"
#include "AppUtils.h"
#include "CommunicationBridge.h"

DeviceConnection *
NetworkDeviceLoader::processConnection(NetworkDevice *device, const QString ipAddress)
{
    DeviceConnection *connection = new DeviceConnection(ipAddress);

    processConnection(device, connection);

    return connection;
}

void
NetworkDeviceLoader::processConnection(NetworkDevice *device, DeviceConnection *connection)
{
    try {
        AppUtils::getDatabase()->reconstruct(connection);
    } catch (exception &e) {
        AppUtils::applyAdapterName(connection);
    }

    connection->lastCheckedDate = clock();
    connection->deviceId = device->deviceId;

    auto *sqlSelection = new SqlSelection();

    sqlSelection->setTableName(AccessDatabaseStructure::TABLE_DEVICECONNECTION)
            ->setWhere(QString("`%1` = ? AND `%2` = ? AND `%3` != ?")
                                         .arg(AccessDatabaseStructure::FIELD_DEVICECONNECTION_DEVICEID)
                                         .arg(AccessDatabaseStructure::FIELD_DEVICECONNECTION_ADAPTERNAME)
                                         .arg(AccessDatabaseStructure::FIELD_DEVICECONNECTION_IPADDRESS));

    sqlSelection->whereArgs << QVariant(connection->deviceId)
                            << QVariant(connection->adapterName)
                            << QVariant(connection->ipAddress);

    AppUtils::getDatabase()->remove(sqlSelection);
    AppUtils::getDatabase()->publish(connection);
}

void NetworkDeviceLoader::loadAsynchronously(const QString &ipAddress)
{
    try {

    } catch (exception &e) {
        // todo signal/slots instead of listener
    }
}

NetworkDevice *NetworkDeviceLoader::load(QObject *sender, const QString &ipAddress)
{
    auto *bridge = new CommunicationBridge(sender);
    auto *device = bridge->loadDevice(ipAddress);

    if (device->deviceId != nullptr) {
        NetworkDevice* localDevice = AppUtils::getLocalDevice();
        DeviceConnection* connection = processConnection(device, ipAddress);

        delete localDevice;
        delete connection;
    }

    delete bridge;
    return device;
}

NetworkDevice *NetworkDeviceLoader::loadFrom(const QJsonObject jsonIndex)
{
    QJsonObject deviceInfo = jsonIndex.value(KEYWORD_DEVICE_INFO).toObject();
    QJsonObject appInfo = jsonIndex.value(KEYWORD_APP_INFO).toObject();

    NetworkDevice *networkDevice = new NetworkDevice(deviceInfo.value(KEYWORD_DEVICE_INFO_SERIAL).toString());

    try {
        AppUtils::getDatabase()->reconstruct(networkDevice);
    } catch (...) {
        // Do nothing because we just want to gather latest instance of the device
    }

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
