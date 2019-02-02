


//
// Created by veli on 9/25/18.
//

#include "NetworkDevice.h"

NetworkDevice::NetworkDevice(QString deviceId, QObject *parent)
        : DatabaseObject(parent)
{
    this->deviceId = std::move(deviceId);
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
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_LASTUSAGETIME, QVariant((long long) this->lastUsageTime));
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
            ->setWhere(QString::asprintf("`%s` = ?", AccessDatabaseStructure::FIELD_DEVICES_ID.toStdString().c_str()));

    selection->whereArgs << QVariant(this->deviceId);

    return selection;
}

void NetworkDevice::onGeneratingValues(QSqlRecord record)
{
    this->brand = record.value(AccessDatabaseStructure::FIELD_DEVICES_BRAND).toString();
    this->model = record.value(AccessDatabaseStructure::FIELD_DEVICES_MODEL).toString();
    this->nickname = record.value(AccessDatabaseStructure::FIELD_DEVICES_USER).toString();
    this->deviceId = record.value(AccessDatabaseStructure::FIELD_DEVICES_ID).toString();
    this->versionName = record.value(AccessDatabaseStructure::FIELD_DEVICES_BUILDNAME).toString();
    this->versionNumber = record.value(AccessDatabaseStructure::FIELD_DEVICES_BUILDNUMBER).toInt();
    this->tmpSecureKey = record.value(AccessDatabaseStructure::FIELD_DEVICES_TMPSECUREKEY).toInt();
    this->lastUsageTime = record.value(AccessDatabaseStructure::FIELD_DEVICES_LASTUSAGETIME).toInt();
    this->isTrusted = record.value(AccessDatabaseStructure::FIELD_DEVICES_ISTRUSTED).toInt() == 1;
    this->isRestricted = record.value(AccessDatabaseStructure::FIELD_DEVICES_ISRESTRICTED) == 1;
    this->isLocalAddress = record.value(AccessDatabaseStructure::FIELD_DEVICES_ISLOCALADDRESS) == 1;
}

DeviceConnection::DeviceConnection(QObject *parent) : DatabaseObject(parent)
{

}

DeviceConnection::DeviceConnection(QString deviceId, QString adapterName, QObject *parent)
        : DatabaseObject(parent)
{
    this->deviceId = std::move(deviceId);
    this->adapterName = std::move(adapterName);
}

DeviceConnection::DeviceConnection(QString ipAddress, QObject *parent)
        : DatabaseObject(parent)
{
    this->ipAddress = std::move(ipAddress);
}

SqlSelection *DeviceConnection::getWhere()
{
    auto selection = (new SqlSelection)
            ->setTableName(AccessDatabaseStructure::TABLE_DEVICECONNECTION);

    if (ipAddress == nullptr) {
        selection->setWhere(QString::asprintf("`%s` = ? AND `%s` = ?",
                                              AccessDatabaseStructure::FIELD_DEVICECONNECTION_DEVICEID.toStdString().c_str(),
                                              AccessDatabaseStructure::FIELD_DEVICECONNECTION_ADAPTERNAME.toStdString().c_str()));
        selection->whereArgs << QVariant(this->deviceId)
                             << QVariant(this->adapterName);
    } else {
        selection->setWhere(QString::asprintf("`%s` = ?", AccessDatabaseStructure::FIELD_DEVICECONNECTION_IPADDRESS.toStdString().c_str()));

        selection->whereArgs << QVariant(this->ipAddress);
    }

    return selection;
}

QSqlRecord DeviceConnection::getValues(AccessDatabase *db)
{
    QSqlRecord record = AccessDatabaseStructure::gatherTableModel(db, this)->record();

    record.setValue(AccessDatabaseStructure::FIELD_DEVICECONNECTION_DEVICEID, QVariant(deviceId));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICECONNECTION_ADAPTERNAME, QVariant(adapterName));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICECONNECTION_IPADDRESS, QVariant(ipAddress));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICECONNECTION_LASTCHECKEDDATE, QVariant((long long) lastCheckedDate));

    return record;
}

void DeviceConnection::onGeneratingValues(QSqlRecord record)
{
    this->deviceId = record.value(AccessDatabaseStructure::FIELD_DEVICECONNECTION_DEVICEID).toString();
    this->adapterName = record.value(AccessDatabaseStructure::FIELD_DEVICECONNECTION_ADAPTERNAME).toString();
    this->ipAddress = record.value(AccessDatabaseStructure::FIELD_DEVICECONNECTION_IPADDRESS).toString();
    this->lastCheckedDate = record.value(AccessDatabaseStructure::FIELD_DEVICECONNECTION_LASTCHECKEDDATE).toULongLong();
}
