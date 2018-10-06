#include "transferobject.h"

TransferObject::TransferObject(int requestId, QObject *parent)
        : DatabaseObject(parent)
{
    this->requestId = requestId;
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

    return record;
}

SqlSelection *TransferObject::getWhere()
{
    auto *selection = new SqlSelection;

    selection
            ->setTableName(AccessDatabaseStructure::TABLE_TRANSFER)
            ->setWhere(QString::asprintf("`%s` = ?", AccessDatabaseStructure::FIELD_TRANSFER_ID.toStdString().c_str()));

    selection->whereArgs << QVariant(this->requestId);

    return selection;
}

void TransferObject::onGeneratingValues(QSqlRecord record)
{
    accessPort = record.field(AccessDatabaseStructure::FIELD_TRANSFER_ACCESSPORT).value().toInt();
    file = record.field(AccessDatabaseStructure::FIELD_TRANSFER_FILE).value().toString();
    directory = record.field(AccessDatabaseStructure::FIELD_TRANSFER_DIRECTORY).value().toString();
    flag = (Flag) record.field(AccessDatabaseStructure::FIELD_TRANSFER_FLAG).value().toInt();
    groupId = record.field(AccessDatabaseStructure::FIELD_TRANSFER_GROUPID).value().toInt();
    requestId = record.field(AccessDatabaseStructure::FIELD_TRANSFER_ID).value().toInt();
    fileMimeType = record.field(AccessDatabaseStructure::FIELD_TRANSFER_MIME).value().toString();
    friendlyName = record.field(AccessDatabaseStructure::FIELD_TRANSFER_NAME).value().toString();
    fileSize = record.field(AccessDatabaseStructure::FIELD_TRANSFER_SIZE).value().toUInt();
    skippedBytes = record.field(AccessDatabaseStructure::FIELD_TRANSFER_SKIPPEDBYTES).value().toUInt();
    type = (Type) record.field(AccessDatabaseStructure::FIELD_TRANSFER_TYPE).value().toInt();
}
