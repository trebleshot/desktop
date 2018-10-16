//
// Created by veli on 9/28/18.
//

#include <QtCore/QJsonObject>
#include "networkdeviceloader.h"

DeviceConnection *
NetworkDeviceLoader::processConnection(AccessDatabase *database, NetworkDevice *device, const QString ipAddress)
{
    DeviceConnection *connection = new DeviceConnection(ipAddress);

    processConnection(database, device, connection);

    return connection;
}

void
NetworkDeviceLoader::processConnection(AccessDatabase *database, NetworkDevice *device, DeviceConnection *connection)
{
    try {
        database->reconstruct(connection);
    } catch (exception &e) {
        // fixme We should have an implementation telling which IP address routes to which adapter
        //AppUtils.applyAdapterName(connection);
    }

    time_t currentTime;
    // todo: did it apply the epoch?

    connection->lastCheckedDate = static_cast<int>(currentTime);
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

    database->publish(connection);
}

void NetworkDeviceLoader::load(AccessDatabase *database, const QString ipAddress)
{
    try {
        load(false, database, ipAddress);
    } catch (exception& e) {
        // todo We don't have listener this time around
    }
}

void NetworkDeviceLoader::load(bool currentThread, AccessDatabase *database, const QString ipAddress)
{

}

NetworkDevice *NetworkDeviceLoader::loadFrom(AccessDatabase *database, const QJsonObject jsonIndex)
{
    return nullptr;
}
