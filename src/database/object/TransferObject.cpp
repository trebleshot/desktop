#include "TransferObject.h"

TransferObject::TransferObject(quint32 requestId, const QString &deviceId, const Type &type, QObject *parent)
        : DatabaseObject(parent)
{
    this->requestId = requestId;
    this->deviceId = deviceId;
    this->type = type;
}

DbObjectMap TransferObject::getValues() const
{
    return DbObjectMap{
            {DbStructure::FIELD_TRANSFER_ACCESSPORT, QVariant(accessPort)},
            {DbStructure::FIELD_TRANSFER_DIRECTORY, QVariant(directory)},
            {DbStructure::FIELD_TRANSFER_FILE, QVariant(file)},
            {DbStructure::FIELD_TRANSFER_FLAG, QVariant(flag)},
            {DbStructure::FIELD_TRANSFER_GROUPID, QVariant(groupId)},
            {DbStructure::FIELD_TRANSFER_ID, QVariant(requestId)},
            {DbStructure::FIELD_TRANSFER_MIME, QVariant(fileMimeType)},
            {DbStructure::FIELD_TRANSFER_NAME, QVariant(friendlyName)},
            {DbStructure::FIELD_TRANSFER_SIZE, QVariant((uint) fileSize)},
            {DbStructure::FIELD_TRANSFER_SKIPPEDBYTES, QVariant((uint) skippedBytes)},
            {DbStructure::FIELD_TRANSFER_TYPE, QVariant(type)},
            {DbStructure::FIELD_TRANSFER_DEVICEID, QVariant(deviceId)}
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

void TransferObject::onGeneratingValues(const QSqlRecord &record)
{
    accessPort = record.field(DbStructure::FIELD_TRANSFER_ACCESSPORT).value().toInt();
    file = record.field(DbStructure::FIELD_TRANSFER_FILE).value().toString();
    directory = record.field(DbStructure::FIELD_TRANSFER_DIRECTORY).value().toString();
    flag = (Flag) record.field(DbStructure::FIELD_TRANSFER_FLAG).value().toInt();
    groupId = record.field(DbStructure::FIELD_TRANSFER_GROUPID).value().toUInt();
    deviceId = record.field(DbStructure::FIELD_TRANSFER_DEVICEID).value().toString();
    requestId = record.field(DbStructure::FIELD_TRANSFER_ID).value().toUInt();
    fileMimeType = record.field(DbStructure::FIELD_TRANSFER_MIME).value().toString();
    friendlyName = record.field(DbStructure::FIELD_TRANSFER_NAME).value().toString();
    fileSize = record.field(DbStructure::FIELD_TRANSFER_SIZE).value().toUInt();
    skippedBytes = record.field(DbStructure::FIELD_TRANSFER_SKIPPEDBYTES).value().toUInt();
    type = (Type) record.field(DbStructure::FIELD_TRANSFER_TYPE).value().toInt();
}

bool TransferObject::isDivisionObject() const
{
    return deviceId == nullptr;
}
