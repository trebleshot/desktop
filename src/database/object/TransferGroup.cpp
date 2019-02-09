#include <utility>

//
// Created by veli on 9/25/18.
//

#include "TransferGroup.h"

TransferGroup::TransferGroup(quint32 groupId, QObject *parent)
        : DatabaseObject(parent)
{
    this->groupId = groupId;
}

SqlSelection *TransferGroup::getWhere()
{
    auto *selection = new SqlSelection;

    selection
            ->setTableName(AccessDatabaseStructure::TABLE_TRANSFERGROUP)
            ->setWhere(QString("`%1` = ?").arg(AccessDatabaseStructure::FIELD_TRANSFERGROUP_ID));

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
    groupId = record.field(AccessDatabaseStructure::FIELD_TRANSFERGROUP_ID).value().toUInt();
    dateCreated = record.field(AccessDatabaseStructure::FIELD_TRANSFERGROUP_DATECREATED).value().toLongLong();
    savePath = record.field(AccessDatabaseStructure::FIELD_TRANSFERGROUP_SAVEPATH).value().toString();
}

TransferAssignee::TransferAssignee(quint32 groupId, const QString &deviceId, const QString &connectionAdapter,
                                   QObject *parent)
        : DatabaseObject(parent)
{
    this->groupId = groupId;
    this->deviceId = deviceId;
    this->connectionAdapter = connectionAdapter;
}

SqlSelection *TransferAssignee::getWhere()
{
    auto *selection = new SqlSelection;

    selection
            ->setTableName(AccessDatabaseStructure::TABLE_TRANSFERASSIGNEE)
            ->setWhere(QString("`%1` = ? AND `%2` = ?")
                                         .arg(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_DEVICEID)
                                         .arg(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_GROUPID));

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
    this->groupId = record.value(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_GROUPID).toUInt();
    this->connectionAdapter = record.value(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER).toString();
    this->isClone = record.value(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_ISCLONE).toInt() == 1;
}
