/*
* Copyright (C) 2019 Veli Tasalı
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

#include <src/util/AppUtils.h>
#include "ReconstructionException.h"
#include "AccessDatabase.h"

using DbStructure::generateField;
using DbStructure::generateTableCreationSql;
using DbStructure::transformType;

AccessDatabase::AccessDatabase(QSqlDatabase *db, QObject *parent)
        : QObject(parent), db(db)
{
    connect(this, &AccessDatabase::signalPublish, this, &AccessDatabase::publish);
}

QSqlDatabase *AccessDatabase::getDatabase()
{
    return this->db;
}

void AccessDatabase::initialize()
{
    QSqlQuery queryExecutor(*getDatabase());
    QMap<QString, QSqlRecord> tables = getPassiveTables();

    for (const QString &dbTableKey : tables.keys()) {
        QSqlRecord dbRecord = tables.value(dbTableKey);
        QString sql = generateTableCreationSql(dbTableKey, dbRecord, true);

        queryExecutor.exec(sql);
    }
}

QMap<QString, QSqlRecord> AccessDatabase::getPassiveTables()
{
    static QMap<QString, QSqlRecord> dbMap;

    if (dbMap.count() <= 0) {
        QSqlRecord tableTransfer;
        tableTransfer.append(generateField(DB_FIELD_TRANSFER_ID, QVariant::Int, false));
        tableTransfer.append(generateField(DB_FIELD_TRANSFER_GROUPID, QVariant::Int, false));
        tableTransfer.append(generateField(DB_FIELD_TRANSFER_DEVICEID, QVariant::String, true));
        tableTransfer.append(generateField(DB_FIELD_TRANSFER_FILE, QVariant::String, true));
        tableTransfer.append(generateField(DB_FIELD_TRANSFER_NAME, QVariant::String, false));
        tableTransfer.append(generateField(DB_FIELD_TRANSFER_SIZE, QVariant::Int, true));
        tableTransfer.append(generateField(DB_FIELD_TRANSFER_MIME, QVariant::String, true));
        tableTransfer.append(generateField(DB_FIELD_TRANSFER_TYPE, QVariant::String, false));
        tableTransfer.append(generateField(DB_FIELD_TRANSFER_DIRECTORY, QVariant::String, true));
        tableTransfer.append(generateField(DB_FIELD_TRANSFER_ACCESSPORT, QVariant::Int, true));
        tableTransfer.append(generateField(DB_FIELD_TRANSFER_SKIPPEDBYTES, QVariant::Size, false));
        tableTransfer.append(generateField(DB_FIELD_TRANSFER_FLAG, QVariant::Int, true));

        QSqlRecord tableGroup;
        tableGroup.append(generateField(DB_FIELD_TRANSFERGROUP_ID, QVariant::Int, false));
        tableGroup.append(generateField(DB_FIELD_TRANSFERGROUP_DATECREATED, QVariant::Time, false));
        tableGroup.append(generateField(DB_FIELD_TRANSFERGROUP_SAVEPATH, QVariant::String, true));

        QSqlRecord tableDevices;
        tableDevices.append(generateField(DB_FIELD_DEVICES_ID, QVariant::String, false));
        tableDevices.append(generateField(DB_FIELD_DEVICES_USER, QVariant::String, false));
        tableDevices.append(generateField(DB_FIELD_DEVICES_BRAND, QVariant::String, false));
        tableDevices.append(generateField(DB_FIELD_DEVICES_MODEL, QVariant::String, false));
        tableDevices.append(generateField(DB_FIELD_DEVICES_BUILDNAME, QVariant::String, false));
        tableDevices.append(generateField(DB_FIELD_DEVICES_BUILDNUMBER, QVariant::Int, false));
        tableDevices.append(generateField(DB_FIELD_DEVICES_LASTUSAGETIME, QVariant::Time, false));
        tableDevices.append(generateField(DB_FIELD_DEVICES_ISRESTRICTED, QVariant::Bool, false));
        tableDevices.append(generateField(DB_FIELD_DEVICES_ISTRUSTED, QVariant::Bool, false));
        tableDevices.append(generateField(DB_FIELD_DEVICES_ISLOCALADDRESS, QVariant::Bool, false));
        tableDevices.append(generateField(DB_FIELD_DEVICES_TMPSECUREKEY, QVariant::Int, true));

        QSqlRecord tableConnection;
        tableConnection.append(generateField(DB_FIELD_DEVICECONNECTION_IPADDRESS, QVariant::String, false));
        tableConnection.append(generateField(DB_FIELD_DEVICECONNECTION_DEVICEID, QVariant::String, false));
        tableConnection.append(generateField(DB_FIELD_DEVICECONNECTION_ADAPTERNAME, QVariant::String, false));
        tableConnection.append(generateField(DB_FIELD_DEVICECONNECTION_LASTCHECKEDDATE, QVariant::Time, false));

        QSqlRecord tableClipboard;
        tableClipboard.append(generateField(DB_FIELD_CLIPBOARD_ID, QVariant::Int, false));
        tableClipboard.append(generateField(DB_FIELD_CLIPBOARD_TEXT, QVariant::String, false));
        tableClipboard.append(generateField(DB_FIELD_CLIPBOARD_TIME, QVariant::Time, false));

        QSqlRecord tableAssignee;
        tableAssignee.append(generateField(DB_FIELD_TRANSFERASSIGNEE_GROUPID, QVariant::Int, false));
        tableAssignee.append(generateField(DB_FIELD_TRANSFERASSIGNEE_DEVICEID, QVariant::String, false));
        tableAssignee.append(generateField(DB_FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER, QVariant::String, true));
        tableAssignee.append(generateField(DB_FIELD_TRANSFERASSIGNEE_ISCLONE, QVariant::Bool, true));

        dbMap.insert(QString(DB_TABLE_TRANSFER), tableTransfer);
        dbMap.insert(QString(DB_DIVIS_TRANSFER), tableTransfer); // Generate division table
        dbMap.insert(QString(DB_TABLE_TRANSFERGROUP), tableGroup);
        dbMap.insert(QString(DB_TABLE_DEVICES), tableDevices);
        dbMap.insert(QString(DB_TABLE_CLIPBOARD), tableClipboard);
        dbMap.insert(QString(DB_TABLE_TRANSFERASSIGNEE), tableAssignee);
        dbMap.insert(QString(DB_TABLE_DEVICECONNECTION), tableConnection);
    }

    return dbMap;
}

bool AccessDatabase::commit()
{
    return getDatabase()->commit();
}

bool AccessDatabase::contains(const DatabaseObject &dbObject)
{
    return contains(dbObject.getWhere());
}

bool AccessDatabase::contains(const SqlSelection &selection)
{
    QSqlQuery query = selection.toSelectionQuery();
    query.exec();

    const auto &lastError = query.lastError();

    if (lastError.type() != QSqlError::ErrorType::NoError)
        qDebug() << lastError << endl << query.executedQuery();

    return query.next();
}

bool AccessDatabase::insert(DatabaseObject &dbObject)
{
    dbObject.onInsertingObject(this);

    const auto &selection = dbObject.getWhere();
    QSqlTableModel *model = DbStructure::gatherTableModel(selection.tableName);
    bool state = model->insertRecord(-1, record(dbObject, model));

    emit databaseChanged(selection, ChangeType::Insert);

    delete model;
    return state;
}

bool AccessDatabase::publish(DatabaseObject &dbObject)
{
    return (this->contains(dbObject) && this->update(dbObject))
           || this->insert(dbObject);
}

bool AccessDatabase::reconstructSilently(DatabaseObject &dbObject)
{
    try {
        reconstruct(dbObject);
        return true;
    } catch (...) {
        // do nothing here
    }

    return false;
}

void AccessDatabase::reconstruct(DatabaseObject &dbObject)
{
    QSqlQuery query = dbObject.getWhere().toSelectionQuery();

    query.exec();

    if (!query.first())
        throw ReconstructionException();

    dbObject.generateValues(query.record());
}

bool AccessDatabase::remove(const SqlSelection &selection)
{
    bool result = selection.toDeletionQuery().exec();
    emit databaseChanged(selection, ChangeType::Delete);

    return result;
}

bool AccessDatabase::remove(DatabaseObject &dbObject)
{
    if (remove(dbObject.getWhere())) {
        dbObject.onRemovingObject(this, nullptr);
        return true;
    }

    return false;
}

bool AccessDatabase::transaction()
{
    return getDatabase()->transaction();
}

bool AccessDatabase::update(DatabaseObject &dbObject)
{
    dbObject.onUpdatingObject(this);
    return update(dbObject.getWhere(), dbObject.getValues());
}

bool AccessDatabase::update(const SqlSelection &selection, const DbObjectMap &map)
{
    bool result = selection.toUpdateQuery(map).exec();
    emit databaseChanged(selection, ChangeType::Update);
    return result;
}

QSqlRecord AccessDatabase::record(const DatabaseObject &object, QSqlTableModel *tableModel)
{
    return record(object.getValues(), tableModel);
}

QSqlRecord AccessDatabase::record(const DbObjectMap &objectMap, QSqlTableModel *tableModel)
{
    QSqlRecord record = tableModel->record();

    for (const auto &key : objectMap.keys())
        record.setValue(key, objectMap.value(key));

    return record;
}

QSqlRecord AccessDatabase::record(const DatabaseObject &object)
{
    return QSqlRecord();
}

QSqlField DbStructure::generateField(const QString &key, const QVariant::Type &type, bool nullable)
{
    QSqlField field(key, type);
    field.setRequired(!nullable);

    return field;
}

QString DbStructure::transformType(const QVariant::Type &type)
{
    switch (type) {
        case QVariant::Bool:
        case QVariant::Int:
        case QVariant::LongLong:
        case QVariant::UInt:
        case QVariant::ULongLong:
            return "integer";
        case QVariant::Double:
            return "double";
        case QVariant::Time:
            return "time";
        case QVariant::Date:
            return "date";
        case QVariant::DateTime:
            return "datetime";
        case QVariant::String:
        case QVariant::Url:
            return "text";
        default:
            return QVariant::typeToName(type);
    }
}

QString DbStructure::generateTableCreationSql(const QString &tableName, const QSqlRecord &record, bool mayExist)
{
    QString sql("create table ");

    if (mayExist)
        sql.append("if not exists ");

    sql.append("`");
    sql.append(tableName);
    sql.append("` (");

    for (int iterate = 0; iterate < record.count(); iterate++) {
        QSqlField thisField = record.field(iterate);

        if (iterate > 0)
            sql.append(", ");

        sql.append("`");
        sql.append(thisField.name());
        sql.append("` ");
        sql.append(transformType(thisField.type()));
        sql.append(" ");
        sql.append(thisField.requiredStatus() == QSqlField::RequiredStatus::Required ? "not null" : "null");
    }

    sql.append(")");

    return sql;
}

QSqlField DbStructure::generateField(const QString &key, const QVariant &value)
{
    QSqlField field(key);
    field.setValue(value);

    return field;
}

QSqlTableModel *DbStructure::gatherTableModel(const DatabaseObject &dbObject)
{
    return gatherTableModel(dbObject.getWhere().tableName);
}

QSqlTableModel *DbStructure::gatherTableModel(const QString &tableName)
{
    auto *model = new QSqlTableModel(gDatabase, *gDatabase->getDatabase());

    model->setTable(tableName);

    return model;
}

void SqlSelection::bindWhereClause(QSqlQuery &query) const
{
    for (const QVariant &whereArg : this->whereArgs)
        query.addBindValue(whereArg);
}

QString SqlSelection::generateSpecifierClause(bool fromStatement) const
{
    QString queryString;

    if (fromStatement) {
        queryString.append(" from `");
        queryString.append(this->tableName);
        queryString.append("`");
    }

    if (this->where.length() > 0) {
        queryString.append(" where ");
        queryString.append(this->where);
    }

    if (this->groupBy.length() > 0) {
        queryString.append(" group by ");
        queryString.append(this->groupBy);
    }

    if (this->orderBy.length() > 0) {
        queryString.append(" order by ");
        queryString.append(this->orderBy);
    }

    if (this->limit != -1) {
        queryString.append(" limit ");
        queryString.append(QVariant(this->limit).toString());
    }

    return queryString;
}

void SqlSelection::setHaving(const QString &having)
{
    this->having = having;
}

void SqlSelection::setGroupBy(const QString &field, bool ascending)
{
    this->groupBy = "`";
    this->groupBy.append(field);
    this->groupBy.append("`");
    this->groupBy.append(ascending ? " asc" : " desc");
}

void SqlSelection::setGroupBy(const QString &orderBy)
{
    this->orderBy = orderBy;
}

void SqlSelection::setLimit(int limit)
{
    this->limit = limit;
}

void SqlSelection::setOrderBy(const QString &field, bool ascending)
{
    this->orderBy = "`";
    this->orderBy.append(field);
    this->orderBy.append("`");
    this->orderBy.append(ascending ? " asc" : " desc");
}

void SqlSelection::setOrderBy(const QString &field)
{
    this->orderBy = field;
}

void SqlSelection::setTableName(const QString &tableName)
{
    this->tableName = tableName;
}

void SqlSelection::setWhere(const QString &whereString)
{
    this->where = whereString;
}

QSqlQuery SqlSelection::toDeletionQuery() const
{
    QString queryString = "delete";
    queryString.append(generateSpecifierClause());

    QSqlQuery query;
    query.prepare(queryString);

    bindWhereClause(query);

    return query;
}

QSqlQuery SqlSelection::toInsertionQuery() const
{
    // Do not implement since not required
    return QSqlQuery();
}

QSqlQuery SqlSelection::toSelectionQuery() const
{
    QString queryString = "select ";
    queryString.append(this->toSelectionColumns());
    queryString.append(generateSpecifierClause());

    QSqlQuery query;
    query.prepare(queryString);

    bindWhereClause(query);

    return query;
}

QString SqlSelection::toSelectionColumns() const
{
    QString output = QString();

    if (this->columns.empty())
        output.append("*");
    else if (!this->columns.empty())
        for (const QString &item : this->columns) {
            if (output.length() > 0)
                output.append(", ");

            output.append("`");
            output.append(item);
            output.append("`");
        }

    return output;
}

QSqlQuery SqlSelection::toUpdateQuery(const DbObjectMap &map) const
{
    QSqlQuery query;
    QString queryString = "update `";
    queryString.append(this->tableName);
    queryString.append("` set ");
    QString updateIndex;

    for (const auto& key : map.keys()) {
        if (updateIndex.length() > 0)
            updateIndex.append(", ");

        updateIndex.append("`");
        updateIndex.append(key);
        updateIndex.append("` = ?");
    }

    queryString.append(updateIndex);
    queryString.append(generateSpecifierClause(false));

    query.prepare(queryString);

    for (const auto& key : map.keys())
        query.addBindValue(map.value(key));

    bindWhereClause(query);

    return query;
}

bool SqlSelection::valid() const
{
    return tableName != nullptr;
}

/***
     * This method generates another mapping just to load this class.
     *
     * A better way would be;
     * A- A method that accepts field name and its value and loading them in a switch statement
     * B- Passive mapping that always exists and have a reference for an object like Q_VARIABLE
     * During compilation they are all created so that they can always accept map
     *
     * (key (readable name) => value (reference)).
     */
void DatabaseObject::generateValues(const QSqlRecord &record)
{
    DbObjectMap map;

    //todo: This is not the best way to handle this
    for (int pos = 0; pos < record.count(); pos++) {
        const QSqlField &thisField = record.field(pos);
        map.insert(thisField.name(), thisField.value());
    }

    onGeneratingValues(map);
}