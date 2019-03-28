//
// Created by veli on 9/25/18.
//
#include "NetworkDevice.h"
#include "TransferGroup.h"
#include <src/util/NetworkDeviceLoader.h>

NetworkDevice::NetworkDevice(const QString &id)
        : DatabaseObject()
{
    this->id = id;
}

DbObjectMap NetworkDevice::getValues() const
{
    return DbObjectMap{
            {DB_FIELD_DEVICES_ID,             this->id},
            {DB_FIELD_DEVICES_BRAND,          this->brand},
            {DB_FIELD_DEVICES_MODEL,          this->model},
            {DB_FIELD_DEVICES_USER,           this->nickname},
            {DB_FIELD_DEVICES_BUILDNAME,      this->versionName},
            {DB_FIELD_DEVICES_BUILDNUMBER,    this->versionNumber},
            {DB_FIELD_DEVICES_TMPSECUREKEY,   this->tmpSecureKey},
            {DB_FIELD_DEVICES_LASTUSAGETIME,  (qlonglong) this->lastUsageTime},
            {DB_FIELD_DEVICES_ISTRUSTED,      this->isTrusted ? 1 : 0},
            {DB_FIELD_DEVICES_ISRESTRICTED,   this->isRestricted ? 1 : 0},
            {DB_FIELD_DEVICES_ISLOCALADDRESS, this->isLocalAddress ? 1 : 0}
    };
}

SqlSelection NetworkDevice::getWhere() const
{
    SqlSelection selection;

    selection.setTableName(DB_TABLE_DEVICES);
    selection.setWhere(QString("`%1` = ?").arg(DB_FIELD_DEVICES_ID));

    selection.whereArgs << this->id;

    return selection;
}

void NetworkDevice::onGeneratingValues(const DbObjectMap &record)
{
    this->id = record.value(DB_FIELD_DEVICES_ID).toString();
    this->brand = record.value(DB_FIELD_DEVICES_BRAND).toString();
    this->model = record.value(DB_FIELD_DEVICES_MODEL).toString();
    this->nickname = record.value(DB_FIELD_DEVICES_USER).toString();
    this->versionName = record.value(DB_FIELD_DEVICES_BUILDNAME).toString();
    this->versionNumber = record.value(DB_FIELD_DEVICES_BUILDNUMBER).toInt();
    this->tmpSecureKey = record.value(DB_FIELD_DEVICES_TMPSECUREKEY).toInt();
    this->lastUsageTime = static_cast<time_t>(record.value(DB_FIELD_DEVICES_LASTUSAGETIME).toLongLong());
    this->isTrusted = record.value(DB_FIELD_DEVICES_ISTRUSTED).toInt() == 1;
    this->isRestricted = record.value(DB_FIELD_DEVICES_ISRESTRICTED) == 1;
    this->isLocalAddress = record.value(DB_FIELD_DEVICES_ISLOCALADDRESS) == 1;
}

void NetworkDevice::onRemovingObject(AccessDatabase *db, DatabaseObject *parent)
{
    DatabaseObject::onRemovingObject(db, parent);

    SqlSelection connection;
    connection.setTableName(DB_TABLE_DEVICECONNECTION);
    connection.setWhere(QString("%1 = ?").arg(DB_FIELD_DEVICECONNECTION_DEVICEID));
    connection.whereArgs << id;

    db->remove(connection);

    SqlSelection assignee;
    assignee.setTableName(DB_TABLE_TRANSFERASSIGNEE);
    assignee.setWhere(QString("%1 = ?").arg(DB_FIELD_TRANSFERASSIGNEE_DEVICEID));
    assignee.whereArgs << id;

	QList<TransferAssignee> assigneeList;
		
	db->castQuery(assignee, assigneeList);

    for (auto thisAssignee : assigneeList) {
        db->remove(thisAssignee);
        TransferGroup group(thisAssignee.groupId);

        if (db->reconstructSilently(group))
        {
            SqlSelection leftAssignee;
            leftAssignee.setTableName(DB_TABLE_TRANSFERASSIGNEE);
            leftAssignee.setWhere(QString("%1 = ?").arg(DB_FIELD_TRANSFERASSIGNEE_GROUPID));
            leftAssignee.whereArgs << group.id;

            if (!db->contains(leftAssignee))
                db->remove(group);
        }
    }
}

DeviceConnection::DeviceConnection(const QString &deviceId, const QString &adapterName)
        : DatabaseObject()
{
    this->deviceId = deviceId;
    this->adapterName = adapterName;
}

DeviceConnection::DeviceConnection(const QHostAddress &hostAddress)
        : DatabaseObject()
{
    this->hostAddress = hostAddress;
}

SqlSelection DeviceConnection::getWhere() const
{
    SqlSelection selection;

    selection.setTableName(DB_TABLE_DEVICECONNECTION);

    if (hostAddress.isNull()) {
        selection.setWhere(QString("`%1` = ? AND `%2` = ?")
                                   .arg(DB_FIELD_DEVICECONNECTION_DEVICEID)
                                   .arg(DB_FIELD_DEVICECONNECTION_ADAPTERNAME));

        selection.whereArgs << QVariant(this->deviceId)
                            << QVariant(this->adapterName);
    } else {
        selection.setWhere(QString("`%1` = ?")
                                   .arg(DB_FIELD_DEVICECONNECTION_IPADDRESS));

        selection.whereArgs << QVariant(this->hostAddress.toString());
    }

    return selection;
}

DbObjectMap DeviceConnection::getValues() const
{
    return DbObjectMap{
            {DB_FIELD_DEVICECONNECTION_DEVICEID,        deviceId},
            {DB_FIELD_DEVICECONNECTION_ADAPTERNAME,     adapterName},
            {DB_FIELD_DEVICECONNECTION_IPADDRESS,       NetworkDeviceLoader::convertToInet4Address(
                    hostAddress.toIPv4Address())},
            {DB_FIELD_DEVICECONNECTION_LASTCHECKEDDATE, (qlonglong) lastCheckedDate},
    };
}

void DeviceConnection::onGeneratingValues(const DbObjectMap &record)
{
    this->deviceId = record.value(DB_FIELD_DEVICECONNECTION_DEVICEID).toString();
    this->adapterName = record.value(DB_FIELD_DEVICECONNECTION_ADAPTERNAME).toString();
    this->hostAddress = QHostAddress(record.value(DB_FIELD_DEVICECONNECTION_IPADDRESS).toString());
    this->lastCheckedDate = static_cast<time_t>(record.value(DB_FIELD_DEVICECONNECTION_LASTCHECKEDDATE).toLongLong());
}
