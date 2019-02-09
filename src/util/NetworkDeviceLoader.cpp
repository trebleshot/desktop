//
// Created by veli on 9/28/18.
//
#include <QtSql/QSqlError>
#include "NetworkDeviceLoader.h"

DeviceConnection *NetworkDeviceLoader::processConnection(NetworkDevice *device,
                                                         const QHostAddress &hostAddress)
{
    auto *connection = new DeviceConnection(hostAddress);

    processConnection(device, connection);

    return connection;
}

void NetworkDeviceLoader::processConnection(NetworkDevice *device,
                                            DeviceConnection *connection)
{
    if (!AppUtils::applyAdapterName(connection) && !gDbSignal->reconstruct(connection))
        connection->adapterName = KEYWORD_UNKNOWN_INTERFACE;

    time(&connection->lastCheckedDate);
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
                                             const QHostAddress &hostAddress,
                                             const std::function<void(NetworkDevice *)> &listener)
{
    GThread::startIndependent([sender, hostAddress, listener](GThread *thisThread) {
        listener(load(sender, hostAddress));
    });
}

NetworkDevice *NetworkDeviceLoader::load(QObject *sender, const QHostAddress &hostAddress)
{
    try {
        auto *bridge = new CommunicationBridge(sender);
        auto *device = bridge->loadDevice(hostAddress);

        if (device->deviceId != nullptr) {
            NetworkDevice *localDevice = AppUtils::getLocalDevice();
            DeviceConnection *connection = processConnection(device, hostAddress);

            if (localDevice->deviceId != device->deviceId) {
                time(&device->lastUsageTime);
                gDbSignal->publish(device);
            }

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

    time(&networkDevice->lastUsageTime);
    networkDevice->brand = deviceInfo.value(KEYWORD_DEVICE_INFO_BRAND).toString();
    networkDevice->model = deviceInfo.value(KEYWORD_DEVICE_INFO_MODEL).toString();
    networkDevice->nickname = deviceInfo.value(KEYWORD_DEVICE_INFO_USER).toString();
    networkDevice->versionNumber = appInfo.value(KEYWORD_APP_INFO_VERSION_CODE).toInt();
    networkDevice->versionName = appInfo.value(KEYWORD_APP_INFO_VERSION_NAME).toString();

    if (networkDevice->nickname.length() > NICKNAME_LENGTH_MAX)
        networkDevice->nickname = networkDevice->nickname.left(NICKNAME_LENGTH_MAX - 1);

    return networkDevice;
}
