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

void NetworkDeviceLoader::load(QString &ipAddress)
{
    try {
        load(false, ipAddress);
    } catch (exception& e) {
        // todo signal/slots instead of listener
    }
}

void NetworkDeviceLoader::load(bool currentThread, const QString ipAddress)
{

}

NetworkDevice *NetworkDeviceLoader::loadFrom(const QJsonObject jsonIndex)
{
    return nullptr;
}
