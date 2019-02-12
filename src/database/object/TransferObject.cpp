#include "TransferObject.h"

TransferObject::TransferObject(quint32 requestId, const QString &deviceId, const Type &type, QObject *parent)
        : DatabaseObject(parent)
{
    this->requestId = requestId;
    this->deviceId = deviceId;
    this->type = type;
}

QSqlRecord TransferObject::getValues(AccessDatabase *db)
{
    QSqlRecord record = DbStructure::gatherTableModel(db, this)->record();

    record.setValue(DbStructure::FIELD_TRANSFER_ACCESSPORT, QVariant(accessPort));
    record.setValue(DbStructure::FIELD_TRANSFER_DIRECTORY, QVariant(directory));
    record.setValue(DbStructure::FIELD_TRANSFER_FILE, QVariant(file));
    record.setValue(DbStructure::FIELD_TRANSFER_FLAG, QVariant(flag));
    record.setValue(DbStructure::FIELD_TRANSFER_GROUPID, QVariant(groupId));
    record.setValue(DbStructure::FIELD_TRANSFER_ID, QVariant(requestId));
    record.setValue(DbStructure::FIELD_TRANSFER_MIME, QVariant(fileMimeType));
    record.setValue(DbStructure::FIELD_TRANSFER_NAME, QVariant(friendlyName));
    record.setValue(DbStructure::FIELD_TRANSFER_SIZE, QVariant((uint) fileSize));
    record.setValue(DbStructure::FIELD_TRANSFER_SKIPPEDBYTES, QVariant((uint) skippedBytes));
    record.setValue(DbStructure::FIELD_TRANSFER_TYPE, QVariant(type));
    record.setValue(DbStructure::FIELD_TRANSFER_DEVICEID, QVariant(deviceId));

    return record;
}

SqlSelection *TransferObject::getWhere()
{
    auto *selection = new SqlSelection;

    if (isDivisionObject())
        selection->setTableName(DbStructure::DIVIS_TRANSFER)
                ->setWhere(QString("`%1` = ? AND `%2` = ?")
                                   .arg(DbStructure::FIELD_TRANSFER_ID)
                                   .arg(DbStructure::FIELD_TRANSFER_TYPE));
    else
        selection->setTableName(DbStructure::TABLE_TRANSFER)
                ->setWhere(QString("`%1` = ? AND `%2` = ? AND `%3` = ?")
                                   .arg(DbStructure::FIELD_TRANSFER_ID)
                                   .arg(DbStructure::FIELD_TRANSFER_TYPE)
                                   .arg(DbStructure::FIELD_TRANSFER_DEVICEID));

    selection->whereArgs << this->requestId
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

bool TransferObject::isDivisionObject()
{
    return deviceId == nullptr;
}
