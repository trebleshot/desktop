#include "TransferObject.h"

TransferObject::TransferObject(requestid id, const QString &deviceId, const Type &type)
        : DatabaseObject()
{
    this->id = id;
    this->deviceId = deviceId;
    this->type = type;
}

DbObjectMap TransferObject::getValues() const
{
    return DbObjectMap{
            {DB_FIELD_TRANSFER_ACCESSPORT,   QVariant(accessPort)},
            {DB_FIELD_TRANSFER_DIRECTORY,    QVariant(directory)},
            {DB_FIELD_TRANSFER_FILE,         QVariant(file)},
            {DB_FIELD_TRANSFER_FLAG,         QVariant(flag)},
            {DB_FIELD_TRANSFER_GROUPID,      QVariant(groupId)},
            {DB_FIELD_TRANSFER_ID,           QVariant(id)},
            {DB_FIELD_TRANSFER_MIME,         QVariant(fileMimeType)},
            {DB_FIELD_TRANSFER_NAME,         QVariant(friendlyName)},
            {DB_FIELD_TRANSFER_SIZE,         QVariant((uint) fileSize)},
            {DB_FIELD_TRANSFER_SKIPPEDBYTES, QVariant((uint) skippedBytes)},
            {DB_FIELD_TRANSFER_TYPE,         QVariant(type)},
            {DB_FIELD_TRANSFER_DEVICEID,     QVariant(deviceId)}
    };
}

SqlSelection TransferObject::getWhere() const
{
    SqlSelection selection;

    if (isDivisionObject()) {
        selection.setTableName(DB_DIVIS_TRANSFER);
        selection.setWhere(QString("`%1` = ? AND `%2` = ?")
                                   .arg(DB_FIELD_TRANSFER_ID)
                                   .arg(DB_FIELD_TRANSFER_TYPE));
    } else {
        selection.setTableName(DB_TABLE_TRANSFER);
        selection.setWhere(QString("`%1` = ? AND `%2` = ? AND `%3` = ?")
                                   .arg(DB_FIELD_TRANSFER_ID)
                                   .arg(DB_FIELD_TRANSFER_TYPE)
                                   .arg(DB_FIELD_TRANSFER_DEVICEID));
    }

    selection.whereArgs << this->id
                        << this->type
                        << this->deviceId;

    return selection;
}

void TransferObject::onGeneratingValues(const DbObjectMap &record)
{
    accessPort = record.value(DB_FIELD_TRANSFER_ACCESSPORT).toInt();
    file = record.value(DB_FIELD_TRANSFER_FILE).toString();
    directory = record.value(DB_FIELD_TRANSFER_DIRECTORY).toString();
    flag = (Flag) record.value(DB_FIELD_TRANSFER_FLAG).toInt();
    groupId = record.value(DB_FIELD_TRANSFER_GROUPID).toUInt();
    deviceId = record.value(DB_FIELD_TRANSFER_DEVICEID).toString();
    id = record.value(DB_FIELD_TRANSFER_ID).toUInt();
    fileMimeType = record.value(DB_FIELD_TRANSFER_MIME).toString();
    friendlyName = record.value(DB_FIELD_TRANSFER_NAME).toString();
    fileSize = record.value(DB_FIELD_TRANSFER_SIZE).toUInt();
    skippedBytes = record.value(DB_FIELD_TRANSFER_SKIPPEDBYTES).toUInt();
    type = (Type) record.value(DB_FIELD_TRANSFER_TYPE).toInt();
}

bool TransferObject::isDivisionObject() const
{
    return deviceId == nullptr;
}
