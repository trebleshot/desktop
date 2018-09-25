//
// Created by veli on 9/25/18.
//

#include "transfergroup.h"

TransferGroup::TransferGroup(int groupId, QObject *parent)
        : DatabaseObject(parent)
{
    this->groupId = groupId;
}

SqlSelection *TransferGroup::getWhere()
{
    SqlSelection *selection = new SqlSelection;

    selection
            ->setTableName(AccessDatabaseStructure::TABLE_TRANSFERGROUP)
            ->setWhere(QString::asprintf("`%s` = ?", AccessDatabaseStructure::FIELD_TRANSFERGROUP_ID.toStdString().c_str()));

    selection->whereArgs << QVariant(this->groupId);

    return selection;
}

QSqlRecord TransferGroup::getValues(AccessDatabase *db)
{
    QSqlRecord record = AccessDatabaseStructure::gatherTableModel(db, this)->record();

    record.setValue(AccessDatabaseStructure::FIELD_TRANSFERGROUP_ID, QVariant(groupId));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFERGROUP_DATECREATED, QVariant(dateCreated));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFERGROUP_SAVEPATH, QVariant(savePath));

    return record;
}

void TransferGroup::onGeneratingValues(QSqlRecord record)
{
    groupId = record.field(AccessDatabaseStructure::FIELD_TRANSFERGROUP_ID).value().toInt();
    dateCreated = record.field(AccessDatabaseStructure::FIELD_TRANSFERGROUP_DATECREATED).value().toInt();
    savePath = record.field(AccessDatabaseStructure::FIELD_TRANSFERGROUP_SAVEPATH).value().toString();
}