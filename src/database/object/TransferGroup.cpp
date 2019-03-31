/*
* Copyright (C) 2019 Veli Tasalı, created on 9/25/18
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <utility>
#include <src/util/TransferUtils.h>

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

void TransferGroup::onInsertingObject(AccessDatabase *db)
{
    DatabaseObject::onInsertingObject(db);
    time(&this->dateCreated);
}

void TransferGroup::onRemovingObject(AccessDatabase *db, DatabaseObject *parent)
{
    DatabaseObject::onRemovingObject(db, parent);

    SqlSelection assignee;
    assignee.setTableName(DB_TABLE_TRANSFERASSIGNEE);
    assignee.setWhere(QString("%1 = ?").arg(DB_FIELD_TRANSFERASSIGNEE_GROUPID));
    assignee.whereArgs << id;

    SqlSelection divTransfer;
    divTransfer.setTableName(DB_DIVIS_TRANSFER);
    divTransfer.setWhere(QString("%1 = ?").arg(DB_FIELD_TRANSFER_GROUPID));
    divTransfer.whereArgs << id;

    SqlSelection transfer;
    transfer.setTableName(DB_TABLE_TRANSFER);
    transfer.setWhere(QString("%1 = ?").arg(DB_FIELD_TRANSFER_GROUPID));
    transfer.whereArgs << id;

    db->remove(assignee);
    db->remove(divTransfer);
    db->removeAsObject(transfer, TransferObject(), this);
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

void TransferAssignee::onRemovingObject(AccessDatabase *db, DatabaseObject *parent)
{
    DatabaseObject::onRemovingObject(db, parent);

    TransferGroup group(groupId);
    const SqlSelection &selection = TransferUtils::createSqlSelection(groupId, deviceId, TransferObject::Flag::Any);

    if (db->reconstructSilently(group))
        db->removeAsObject(selection, TransferObject(), &group);
    else
        db->remove(selection);
}
