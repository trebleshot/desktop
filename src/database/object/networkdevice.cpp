

//
// Created by veli on 9/25/18.
//

#include "networkdevice.h"

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
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_LASTUSAGETIME, QVariant(this->lastUsageTime));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_ISTRUSTED, QVariant(this->isTrusted ? 1 : 0));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_ISRESTRICTED, QVariant(this->isRestricted ? 1 : 0));
    record.setValue(AccessDatabaseStructure::FIELD_DEVICES_ISLOCALADDRESS, QVariant(this->isLocalAddress ? 1 : 0));

    return record;
}

SqlSelection *NetworkDevice::getWhere()
{
    SqlSelection *selection = new SqlSelection;

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
