#include <utility>

//
// Created by veli on 9/25/18.
//

#include "TransferGroup.h"

TransferGroup::TransferGroup(groupid groupId) : DatabaseObject()
{
    this->id = groupId;
}

SqlSelection TransferGroup::getWhere() const
{
    SqlSelection selection;

    selection.setTableName(DB_TABLE_TRANSFERGROUP);
    selection.setWhere(QString("`%1` = ?").arg(DB_FIELD_TRANSFERGROUP_ID));

    selection.whereArgs << QVariant(this->id);

    return selection;
}

DbObjectMap TransferGroup::getValues() const
{
    return DbObjectMap{
            {DB_FIELD_TRANSFERGROUP_ID,          id},
            {DB_FIELD_TRANSFERGROUP_DATECREATED, (qlonglong) dateCreated},
            {DB_FIELD_TRANSFERGROUP_SAVEPATH,    savePath}
    };
}

void TransferGroup::onGeneratingValues(const DbObjectMap &record)
{
    id = record.value(DB_FIELD_TRANSFERGROUP_ID).toUInt();
    dateCreated = record.value(DB_FIELD_TRANSFERGROUP_DATECREATED).toLongLong();
    savePath = record.value(DB_FIELD_TRANSFERGROUP_SAVEPATH).toString();
}

TransferAssignee::TransferAssignee(groupid groupId, const QString &deviceId, const QString &connectionAdapter)
        : DatabaseObject()
{
    this->groupId = groupId;
    this->deviceId = deviceId;
    this->connectionAdapter = connectionAdapter;
}

SqlSelection TransferAssignee::getWhere() const
{
    SqlSelection selection;

    selection.setTableName(DB_TABLE_TRANSFERASSIGNEE);
    selection.setWhere(QString("`%1` = ? AND `%2` = ?")
                               .arg(DB_FIELD_TRANSFERASSIGNEE_DEVICEID)
                               .arg(DB_FIELD_TRANSFERASSIGNEE_GROUPID));

    selection.whereArgs << QVariant(this->deviceId)
                        << QVariant(this->groupId);

    return selection;
}

DbObjectMap TransferAssignee::getValues() const
{
    return DbObjectMap{
            {DB_FIELD_TRANSFERASSIGNEE_DEVICEID,          QVariant(deviceId)},
            {DB_FIELD_TRANSFERASSIGNEE_GROUPID,           QVariant(groupId)},
            {DB_FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER, QVariant(connectionAdapter)},
            {DB_FIELD_TRANSFERASSIGNEE_ISCLONE,           isClone ? 1 : 0}
    };
}

void TransferAssignee::onGeneratingValues(const DbObjectMap &record)
{
    this->deviceId = record.value(DB_FIELD_TRANSFERASSIGNEE_DEVICEID).toString();
    this->groupId = record.value(DB_FIELD_TRANSFERASSIGNEE_GROUPID).toUInt();
    this->connectionAdapter = record.value(DB_FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER).toString();
    this->isClone = record.value(DB_FIELD_TRANSFERASSIGNEE_ISCLONE).toInt() == 1;
}
