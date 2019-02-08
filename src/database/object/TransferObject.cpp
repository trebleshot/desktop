#include "TransferObject.h"

TransferObject::TransferObject(ulong requestId, const QString &deviceId, const Type &type, QObject *parent)
        : DatabaseObject(parent)
{
    this->requestId = requestId;
    this->deviceId = deviceId;
    this->type = type;
}

QSqlRecord TransferObject::getValues(AccessDatabase *db)
{
    QSqlRecord record = AccessDatabaseStructure::gatherTableModel(db, this)->record();

    record.setValue(AccessDatabaseStructure::FIELD_TRANSFER_ACCESSPORT, QVariant(accessPort));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFER_DIRECTORY, QVariant(directory));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFER_FILE, QVariant(file));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFER_FLAG, QVariant(flag));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFER_GROUPID, QVariant(groupId));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFER_ID, QVariant(requestId));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFER_MIME, QVariant(fileMimeType));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFER_NAME, QVariant(friendlyName));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFER_SIZE, QVariant((uint) fileSize));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFER_SKIPPEDBYTES, QVariant((uint) skippedBytes));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFER_TYPE, QVariant(type));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFER_DEVICEID, QVariant(deviceId));

    return record;
}

SqlSelection *TransferObject::getWhere()
{
    auto *selection = new SqlSelection;

    selection
            ->setTableName(AccessDatabaseStructure::TABLE_TRANSFER)
            ->setWhere(isDivisionObject()
                       ? QString("`%1` = ? AND `%2` = ?")
                               .arg(AccessDatabaseStructure::FIELD_TRANSFER_ID)
                               .arg(type)
                       : QString("`%1` = ? AND `%2` = ? AND `%3` = ?")
                               .arg(AccessDatabaseStructure::FIELD_TRANSFER_ID)
                               .arg(type)
                               .arg(deviceId));

    selection->whereArgs << this->requestId
                         << this->type
                         << this->deviceId;

    return selection;
}

void TransferObject::onGeneratingValues(QSqlRecord record)
{
    accessPort = record.field(AccessDatabaseStructure::FIELD_TRANSFER_ACCESSPORT).value().toInt();
    file = record.field(AccessDatabaseStructure::FIELD_TRANSFER_FILE).value().toString();
    directory = record.field(AccessDatabaseStructure::FIELD_TRANSFER_DIRECTORY).value().toString();
    flag = (Flag) record.field(AccessDatabaseStructure::FIELD_TRANSFER_FLAG).value().toInt();
    groupId = record.field(AccessDatabaseStructure::FIELD_TRANSFER_GROUPID).value().toULongLong();
    deviceId = record.field(AccessDatabaseStructure::FIELD_TRANSFER_DEVICEID).value().toString();
    requestId = record.field(AccessDatabaseStructure::FIELD_TRANSFER_ID).value().toULongLong();
    fileMimeType = record.field(AccessDatabaseStructure::FIELD_TRANSFER_MIME).value().toString();
    friendlyName = record.field(AccessDatabaseStructure::FIELD_TRANSFER_NAME).value().toString();
    fileSize = record.field(AccessDatabaseStructure::FIELD_TRANSFER_SIZE).value().toUInt();
    skippedBytes = record.field(AccessDatabaseStructure::FIELD_TRANSFER_SKIPPEDBYTES).value().toUInt();
    type = (Type) record.field(AccessDatabaseStructure::FIELD_TRANSFER_TYPE).value().toInt();
}

bool TransferObject::isDivisionObject()
{
    return deviceId == nullptr;
}
