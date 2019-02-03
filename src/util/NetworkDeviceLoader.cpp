//
// Created by veli on 9/28/18.
//

#include <QtCore/QJsonObject>
#include "NetworkDeviceLoader.h"
#include "AppUtils.h"

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
            ->setWhere(QString::asprintf("`%s` = ? AND `%s` = ? AND `%s` != ?",
                                         AccessDatabaseStructure::FIELD_DEVICECONNECTION_DEVICEID.toStdString().c_str(),
                                         AccessDatabaseStructure::FIELD_DEVICECONNECTION_ADAPTERNAME.toStdString().c_str(),
                                         AccessDatabaseStructure::FIELD_DEVICECONNECTION_IPADDRESS.toStdString().c_str()));

    sqlSelection->whereArgs << QVariant(connection->deviceId)
                            << QVariant(connection->adapterName)
                            << QVariant(connection->ipAddress);

    AppUtils::getDatabase()->publish(connection);
}

void NetworkDeviceLoader::loadAsynchronously(const QString &ipAddress)
{
    try {

    } catch (exception &e) {
        // todo signal/slots instead of listener
    }
}

void NetworkDeviceLoader::load(const QString &ipAddress)
{
    
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
