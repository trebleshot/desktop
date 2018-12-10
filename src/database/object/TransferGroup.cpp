#include <utility>

//
// Created by veli on 9/25/18.
//

#include "TransferGroup.h"

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

TransferAssignee::TransferAssignee(int groupId, QString deviceId, QString connectionAdapter, QObject *parent)
        : DatabaseObject(parent)
{
    this->groupId = groupId;
    this->deviceId = std::move(deviceId);
    this->connectionAdapter = std::move(connectionAdapter);
}

SqlSelection *TransferAssignee::getWhere()
{
    SqlSelection *selection = new SqlSelection;

    selection
            ->setTableName(AccessDatabaseStructure::TABLE_TRANSFERASSIGNEE)
            ->setWhere(QString::asprintf("`%s` = ? AND `%s` = ?",
                                         AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_DEVICEID.toStdString().c_str(),
                                         AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_GROUPID.toStdString().c_str()));

    selection->whereArgs << QVariant(this->deviceId)
                         << QVariant(this->groupId);

    return selection;
}

QSqlRecord TransferAssignee::getValues(AccessDatabase *db)
{
    QSqlRecord record = AccessDatabaseStructure::gatherTableModel(db, this)->record();

    record.setValue(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_DEVICEID, QVariant(deviceId));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_GROUPID, QVariant(groupId));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER, QVariant(connectionAdapter));
    record.setValue(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_ISCLONE, isClone ? 1 : 0);

    return record;
}

void TransferAssignee::onGeneratingValues(QSqlRecord record)
{
     this->deviceId = record.value(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_DEVICEID).toString();
     this->groupId = record.value(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_GROUPID).toInt();
     this->connectionAdapter = record.value(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER).toString();
     this->isClone = record.value(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_ISCLONE).toInt() == 1;
}
