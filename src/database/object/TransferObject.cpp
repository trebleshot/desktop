#include "TransferObject.h"

TransferObject::TransferObject(quint32 requestId, const QString &deviceId, const Type &type)
        : DatabaseObject()
{
    this->requestId = requestId;
    this->deviceId = deviceId;
    this->type = type;
}

DbObjectMap TransferObject::getValues() const
{
    return DbObjectMap{
            {DbStructure::FIELD_TRANSFER_ACCESSPORT,   QVariant(accessPort)},
            {DbStructure::FIELD_TRANSFER_DIRECTORY,    QVariant(directory)},
            {DbStructure::FIELD_TRANSFER_FILE,         QVariant(file)},
            {DbStructure::FIELD_TRANSFER_FLAG,         QVariant(flag)},
            {DbStructure::FIELD_TRANSFER_GROUPID,      QVariant(groupId)},
            {DbStructure::FIELD_TRANSFER_ID,           QVariant(requestId)},
            {DbStructure::FIELD_TRANSFER_MIME,         QVariant(fileMimeType)},
            {DbStructure::FIELD_TRANSFER_NAME,         QVariant(friendlyName)},
            {DbStructure::FIELD_TRANSFER_SIZE,         QVariant((uint) fileSize)},
            {DbStructure::FIELD_TRANSFER_SKIPPEDBYTES, QVariant((uint) skippedBytes)},
            {DbStructure::FIELD_TRANSFER_TYPE,         QVariant(type)},
            {DbStructure::FIELD_TRANSFER_DEVICEID,     QVariant(deviceId)}
    };
}

SqlSelection TransferObject::getWhere() const
{
    SqlSelection selection;

    if (isDivisionObject()) {
        selection.setTableName(DbStructure::DIVIS_TRANSFER);
        selection.setWhere(QString("`%1` = ? AND `%2` = ?")
                                   .arg(DbStructure::FIELD_TRANSFER_ID)
                                   .arg(DbStructure::FIELD_TRANSFER_TYPE));
    } else {
        selection.setTableName(DbStructure::TABLE_TRANSFER);
        selection.setWhere(QString("`%1` = ? AND `%2` = ? AND `%3` = ?")
                                   .arg(DbStructure::FIELD_TRANSFER_ID)
                                   .arg(DbStructure::FIELD_TRANSFER_TYPE)
                                   .arg(DbStructure::FIELD_TRANSFER_DEVICEID));
    }

    selection.whereArgs << this->requestId
                        << this->type
                        << this->deviceId;

    return selection;
}

void TransferObject::onGeneratingValues(const DbObjectMap &record)
{
    accessPort = record.value(DbStructure::FIELD_TRANSFER_ACCESSPORT).toInt();
    file = record.value(DbStructure::FIELD_TRANSFER_FILE).toString();
    directory = record.value(DbStructure::FIELD_TRANSFER_DIRECTORY).toString();
    flag = (Flag) record.value(DbStructure::FIELD_TRANSFER_FLAG).toInt();
    groupId = record.value(DbStructure::FIELD_TRANSFER_GROUPID).toUInt();
    deviceId = record.value(DbStructure::FIELD_TRANSFER_DEVICEID).toString();
    requestId = record.value(DbStructure::FIELD_TRANSFER_ID).toUInt();
    fileMimeType = record.value(DbStructure::FIELD_TRANSFER_MIME).toString();
    friendlyName = record.value(DbStructure::FIELD_TRANSFER_NAME).toString();
    fileSize = record.value(DbStructure::FIELD_TRANSFER_SIZE).toUInt();
    skippedBytes = record.value(DbStructure::FIELD_TRANSFER_SKIPPEDBYTES).toUInt();
    type = (Type) record.value(DbStructure::FIELD_TRANSFER_TYPE).toInt();
}

bool TransferObject::isDivisionObject() const
{
    return deviceId == nullptr;
}
