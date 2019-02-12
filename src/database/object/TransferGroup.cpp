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
            ->setTableName(DbStructure::TABLE_TRANSFERGROUP)
            ->setWhere(QString("`%1` = ?").arg(DbStructure::FIELD_TRANSFERGROUP_ID));

    selection->whereArgs << QVariant(this->groupId);

    return selection;
}

QSqlRecord TransferGroup::getValues(AccessDatabase *db)
{
    QSqlRecord record = DbStructure::gatherTableModel(db, this)->record();

    record.setValue(DbStructure::FIELD_TRANSFERGROUP_ID, QVariant(groupId));
    record.setValue(DbStructure::FIELD_TRANSFERGROUP_DATECREATED, QVariant((qlonglong) dateCreated));
    record.setValue(DbStructure::FIELD_TRANSFERGROUP_SAVEPATH, QVariant(savePath));

    return record;
}

void TransferGroup::onGeneratingValues(const QSqlRecord &record)
{
    groupId = record.field(DbStructure::FIELD_TRANSFERGROUP_ID).value().toUInt();
    dateCreated = record.field(DbStructure::FIELD_TRANSFERGROUP_DATECREATED).value().toLongLong();
    savePath = record.field(DbStructure::FIELD_TRANSFERGROUP_SAVEPATH).value().toString();
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
            ->setTableName(DbStructure::TABLE_TRANSFERASSIGNEE)
            ->setWhere(QString("`%1` = ? AND `%2` = ?")
                               .arg(DbStructure::FIELD_TRANSFERASSIGNEE_DEVICEID)
                               .arg(DbStructure::FIELD_TRANSFERASSIGNEE_GROUPID));

    selection->whereArgs << QVariant(this->deviceId)
                         << QVariant(this->groupId);

    return selection;
}

QSqlRecord TransferAssignee::getValues(AccessDatabase *db)
{
    QSqlRecord record = DbStructure::gatherTableModel(db, this)->record();

    record.setValue(DbStructure::FIELD_TRANSFERASSIGNEE_DEVICEID, QVariant(deviceId));
    record.setValue(DbStructure::FIELD_TRANSFERASSIGNEE_GROUPID, QVariant(groupId));
    record.setValue(DbStructure::FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER, QVariant(connectionAdapter));
    record.setValue(DbStructure::FIELD_TRANSFERASSIGNEE_ISCLONE, isClone ? 1 : 0);

    return record;
}

void TransferAssignee::onGeneratingValues(const QSqlRecord &record)
{
    this->deviceId = record.value(DbStructure::FIELD_TRANSFERASSIGNEE_DEVICEID).toString();
    this->groupId = record.value(DbStructure::FIELD_TRANSFERASSIGNEE_GROUPID).toUInt();
    this->connectionAdapter = record.value(DbStructure::FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER).toString();
    this->isClone = record.value(DbStructure::FIELD_TRANSFERASSIGNEE_ISCLONE).toInt() == 1;
}
