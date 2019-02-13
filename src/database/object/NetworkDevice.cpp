//
// Created by veli on 9/25/18.
//
#include <src/util/NetworkDeviceLoader.h>
#include "NetworkDevice.h"

NetworkDevice::NetworkDevice(const QString &deviceId, QObject *parent)
        : DatabaseObject(parent)
{
    this->deviceId = deviceId;
}

QSqlRecord NetworkDevice::getValues(AccessDatabase *db)
{
    QSqlRecord record = DbStructure::gatherTableModel(db, this)->record();

    record.setValue(DbStructure::FIELD_DEVICES_BRAND, QVariant(this->brand));
    record.setValue(DbStructure::FIELD_DEVICES_MODEL, QVariant(this->model));
    record.setValue(DbStructure::FIELD_DEVICES_USER, QVariant(this->nickname));
    record.setValue(DbStructure::FIELD_DEVICES_ID, QVariant(this->deviceId));
    record.setValue(DbStructure::FIELD_DEVICES_BUILDNAME, QVariant(this->versionName));
    record.setValue(DbStructure::FIELD_DEVICES_BUILDNUMBER, QVariant(this->versionNumber));
    record.setValue(DbStructure::FIELD_DEVICES_TMPSECUREKEY, QVariant(this->tmpSecureKey));
    record.setValue(DbStructure::FIELD_DEVICES_LASTUSAGETIME, QVariant((qlonglong) this->lastUsageTime));
    record.setValue(DbStructure::FIELD_DEVICES_ISTRUSTED, QVariant(this->isTrusted ? 1 : 0));
    record.setValue(DbStructure::FIELD_DEVICES_ISRESTRICTED, QVariant(this->isRestricted ? 1 : 0));
    record.setValue(DbStructure::FIELD_DEVICES_ISLOCALADDRESS, QVariant(this->isLocalAddress ? 1 : 0));

    return record;
}

SqlSelection *NetworkDevice::getWhere()
{
    auto *selection = new SqlSelection;

    selection
            ->setTableName(DbStructure::TABLE_DEVICES)
            ->setWhere(QString("`%1` = ?").arg(DbStructure::FIELD_DEVICES_ID));

    selection->whereArgs << this->deviceId;

    return selection;
}

void NetworkDevice::onGeneratingValues(const QSqlRecord &record)
{
    this->brand = record.value(DbStructure::FIELD_DEVICES_BRAND).toString();
    this->model = record.value(DbStructure::FIELD_DEVICES_MODEL).toString();
    this->nickname = record.value(DbStructure::FIELD_DEVICES_USER).toString();
    this->deviceId = record.value(DbStructure::FIELD_DEVICES_ID).toString();
    this->versionName = record.value(DbStructure::FIELD_DEVICES_BUILDNAME).toString();
    this->versionNumber = record.value(DbStructure::FIELD_DEVICES_BUILDNUMBER).toInt();
    this->tmpSecureKey = record.value(DbStructure::FIELD_DEVICES_TMPSECUREKEY).toInt();
    this->lastUsageTime = static_cast<time_t>(record.value(DbStructure::FIELD_DEVICES_LASTUSAGETIME).toLongLong());
    this->isTrusted = record.value(DbStructure::FIELD_DEVICES_ISTRUSTED).toInt() == 1;
    this->isRestricted = record.value(DbStructure::FIELD_DEVICES_ISRESTRICTED) == 1;
    this->isLocalAddress = record.value(DbStructure::FIELD_DEVICES_ISLOCALADDRESS) == 1;
}

DeviceConnection::DeviceConnection(QObject *parent) : DatabaseObject(parent)
{

}

DeviceConnection::DeviceConnection(const QString &deviceId, const QString &adapterName, QObject *parent)
        : DatabaseObject(parent)
{
    this->deviceId = deviceId;
    this->adapterName = adapterName;
}

DeviceConnection::DeviceConnection(const QHostAddress &hostAddress, QObject *parent)
        : DatabaseObject(parent)
{
    this->hostAddress = hostAddress;
}

SqlSelection *DeviceConnection::getWhere()
{
    auto selection = (new SqlSelection)
            ->setTableName(DbStructure::TABLE_DEVICECONNECTION);

    if (hostAddress.isNull()) {
        selection->setWhere(QString("`%1` = ? AND `%2` = ?")
                                    .arg(DbStructure::FIELD_DEVICECONNECTION_DEVICEID)
                                    .arg(DbStructure::FIELD_DEVICECONNECTION_ADAPTERNAME));

        selection->whereArgs << QVariant(this->deviceId)
                             << QVariant(this->adapterName);
    } else {
        selection->setWhere(QString("`%1` = ?")
                                    .arg(DbStructure::FIELD_DEVICECONNECTION_IPADDRESS));

        selection->whereArgs << QVariant(this->hostAddress.toString());
    }

    return selection;
}

QSqlRecord DeviceConnection::getValues(AccessDatabase *db)
{
    QSqlRecord record = DbStructure::gatherTableModel(db, this)->record();

    record.setValue(DbStructure::FIELD_DEVICECONNECTION_DEVICEID, deviceId);
    record.setValue(DbStructure::FIELD_DEVICECONNECTION_ADAPTERNAME, adapterName);
    record.setValue(DbStructure::FIELD_DEVICECONNECTION_IPADDRESS,
            NetworkDeviceLoader::convertToInet4Address(hostAddress.toIPv4Address()));
    record.setValue(DbStructure::FIELD_DEVICECONNECTION_LASTCHECKEDDATE, QVariant((qlonglong) lastCheckedDate));

    return record;
}

void DeviceConnection::onGeneratingValues(const QSqlRecord &record)
{
    this->deviceId = record.value(DbStructure::FIELD_DEVICECONNECTION_DEVICEID).toString();
    this->adapterName = record.value(DbStructure::FIELD_DEVICECONNECTION_ADAPTERNAME).toString();
    this->hostAddress = QHostAddress(record.value(DbStructure::FIELD_DEVICECONNECTION_IPADDRESS).toString());
    this->lastCheckedDate = static_cast<time_t>(record.value(DbStructure::FIELD_DEVICECONNECTION_LASTCHECKEDDATE).toLongLong());
}
