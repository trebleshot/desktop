//
// Created by veli on 9/25/18.
//
#include "NetworkDevice.h"

NetworkDevice::NetworkDevice(const QString &deviceId, QObject *parent)
        : DatabaseObject(parent)
{
    this->deviceId = deviceId;
}

QSqlRecord NetworkDevice::getValues(AccessDatabase *db)
{
    QSqlRecord record = AccessDatabaseStructure::gatherTableModel(db, this)->record();

    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_BRAND, QVariant(this->brand));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_MODEL, QVariant(this->model));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_USER, QVariant(this->nickname));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_ID, QVariant(this->deviceId));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_BUILDNAME, QVariant(this->versionName));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_BUILDNUMBER, QVariant(this->versionNumber));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_TMPSECUREKEY, QVariant(this->tmpSecureKey));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_LASTUSAGETIME, QVariant((qlonglong) this->lastUsageTime));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_ISTRUSTED, QVariant(this->isTrusted ? 1 : 0));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_ISRESTRICTED, QVariant(this->isRestricted ? 1 : 0));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_ISLOCALADDRESS, QVariant(this->isLocalAddress ? 1 : 0));

    return record;
}

SqlSelection *NetworkDevice::getWhere()
{
    auto *selection = new SqlSelection;

    selection
            ->setTableName(AccessDatabaseStructure::TABLE_DEVICES)
            ->setWhere(QString("`%1` = ?").arg(AccessDatabaseStructure::FIELD_DEVICES_ID));

    selection->whereArgs << this->deviceId;

    return selection;
}

void NetworkDevice::onGeneratingValues(const QSqlRecord &record)
{
    this->brand = record.value(AccessDatabaseStructure::FIELD_DEVICES_BRAND).toString();
    this->model = record.value(AccessDatabaseStructure::FIELD_DEVICES_MODEL).toString();
    this->nickname = record.value(AccessDatabaseStructure::FIELD_DEVICES_USER).toString();
    this->deviceId = record.value(AccessDatabaseStructure::FIELD_DEVICES_ID).toString();
    this->versionName = record.value(AccessDatabaseStructure::FIELD_DEVICES_BUILDNAME).toString();
    this->versionNumber = record.value(AccessDatabaseStructure::FIELD_DEVICES_BUILDNUMBER).toInt();
    this->tmpSecureKey = record.value(AccessDatabaseStructure::FIELD_DEVICES_TMPSECUREKEY).toInt();
    this->lastUsageTime = static_cast<time_t>(record.value(AccessDatabaseStructure::FIELD_DEVICES_LASTUSAGETIME).toLongLong());
    this->isTrusted = record.value(AccessDatabaseStructure::FIELD_DEVICES_ISTRUSTED).toInt() == 1;
    this->isRestricted = record.value(AccessDatabaseStructure::FIELD_DEVICES_ISRESTRICTED) == 1;
    this->isLocalAddress = record.value(AccessDatabaseStructure::FIELD_DEVICES_ISLOCALADDRESS) == 1;
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
            ->setTableName(AccessDatabaseStructure::TABLE_DEVICECONNECTION);

    if (hostAddress.isNull()) {
        selection->setWhere(QString("`%1` = ? AND `%2` = ?")
                                    .arg(AccessDatabaseStructure::FIELD_DEVICECONNECTION_DEVICEID)
                                    .arg(AccessDatabaseStructure::FIELD_DEVICECONNECTION_ADAPTERNAME));

        selection->whereArgs << QVariant(this->deviceId)
                             << QVariant(this->adapterName);
    } else {
        selection->setWhere(QString("`%1` = ?")
                                    .arg(AccessDatabaseStructure::FIELD_DEVICECONNECTION_IPADDRESS));

        selection->whereArgs << QVariant(this->hostAddress.toString());
    }

    return selection;
}

QSqlRecord DeviceConnection::getValues(AccessDatabase *db)
{
    QSqlRecord record = AccessDatabaseStructure::gatherTableModel(db, this)->record();

    record.setValue(AccessDatabaseStructure::FIELD_DEVICECONNECTION_DEVICEID, deviceId);
    record.setValue(AccessDatabaseStructure::FIELD_DEVICECONNECTION_ADAPTERNAME, adapterName);
    record.setValue(AccessDatabaseStructure::FIELD_DEVICECONNECTION_IPADDRESS, hostAddress.toString());
    record.setValue(AccessDatabaseStructure::FIELD_DEVICECONNECTION_LASTCHECKEDDATE, QVariant((qlonglong) lastCheckedDate));

    return record;
}

void DeviceConnection::onGeneratingValues(const QSqlRecord &record)
{
    this->deviceId = record.value(AccessDatabaseStructure::FIELD_DEVICECONNECTION_DEVICEID).toString();
    this->adapterName = record.value(AccessDatabaseStructure::FIELD_DEVICECONNECTION_ADAPTERNAME).toString();
    this->hostAddress = QHostAddress(record.value(AccessDatabaseStructure::FIELD_DEVICECONNECTION_IPADDRESS).toString());
    this->lastCheckedDate = static_cast<time_t>(record.value(AccessDatabaseStructure::FIELD_DEVICECONNECTION_LASTCHECKEDDATE).toLongLong());
}
