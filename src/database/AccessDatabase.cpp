#include <c++/7/bits/unique_ptr.h>
#include "AccessDatabase.h"
#include "ReconstructionException.h"

using DbStructure::generateField;
using DbStructure::generateTableCreationSql;
using DbStructure::transformType;

AccessDatabase::AccessDatabase(QSqlDatabase *db, QObject *parent)
        : QObject(parent)
{
    this->db = db;

    connect(this, &AccessDatabase::signalPublish, this, &AccessDatabase::publish);
}

QSqlDatabase *AccessDatabase::getDatabase()
{
    return this->db;
}

void AccessDatabase::initialize()
{
    QSqlQuery queryExecutor(*getDatabase());
    QMap<QString, QSqlRecord> *tables = getPassiveTables();

    for (const QString &dbTableKey : tables->keys()) {
        QSqlRecord dbRecord = tables->take(dbTableKey);
        QString sql = generateTableCreationSql(dbTableKey, dbRecord, true);

        queryExecutor.exec(sql);
    }

    delete tables;
}

QMap<QString, QSqlRecord> *AccessDatabase::getPassiveTables()
{
    QSqlRecord tableTransfer;
    tableTransfer.append(generateField(DbStructure::FIELD_TRANSFER_ID, QVariant::Int, false));
    tableTransfer.append(generateField(DbStructure::FIELD_TRANSFER_GROUPID, QVariant::Int, false));
    tableTransfer.append(generateField(DbStructure::FIELD_TRANSFER_DEVICEID, QVariant::String, true));
    tableTransfer.append(generateField(DbStructure::FIELD_TRANSFER_FILE, QVariant::String, true));
    tableTransfer.append(generateField(DbStructure::FIELD_TRANSFER_NAME, QVariant::String, false));
    tableTransfer.append(generateField(DbStructure::FIELD_TRANSFER_SIZE, QVariant::Int, true));
    tableTransfer.append(generateField(DbStructure::FIELD_TRANSFER_MIME, QVariant::String, true));
    tableTransfer.append(generateField(DbStructure::FIELD_TRANSFER_TYPE, QVariant::String, false));
    tableTransfer.append(generateField(DbStructure::FIELD_TRANSFER_DIRECTORY, QVariant::String, true));
    tableTransfer.append(generateField(DbStructure::FIELD_TRANSFER_ACCESSPORT, QVariant::Int, true));
    tableTransfer.append(generateField(DbStructure::FIELD_TRANSFER_SKIPPEDBYTES, QVariant::Size, false));
    tableTransfer.append(generateField(DbStructure::FIELD_TRANSFER_FLAG, QVariant::Int, true));

    QSqlRecord tableGroup;
    tableGroup.append(generateField(DbStructure::FIELD_TRANSFERGROUP_ID, QVariant::Int, false));
    tableGroup.append(generateField(DbStructure::FIELD_TRANSFERGROUP_DATECREATED, QVariant::Time, false));
    tableGroup.append(generateField(DbStructure::FIELD_TRANSFERGROUP_SAVEPATH, QVariant::String, true));

    QSqlRecord tableDevices;
    tableDevices.append(generateField(DbStructure::FIELD_DEVICES_ID, QVariant::String, false));
    tableDevices.append(generateField(DbStructure::FIELD_DEVICES_USER, QVariant::String, false));
    tableDevices.append(generateField(DbStructure::FIELD_DEVICES_BRAND, QVariant::String, false));
    tableDevices.append(generateField(DbStructure::FIELD_DEVICES_MODEL, QVariant::String, false));
    tableDevices.append(generateField(DbStructure::FIELD_DEVICES_BUILDNAME, QVariant::String, false));
    tableDevices.append(generateField(DbStructure::FIELD_DEVICES_BUILDNUMBER, QVariant::Int, false));
    tableDevices.append(generateField(DbStructure::FIELD_DEVICES_LASTUSAGETIME, QVariant::Time, false));
    tableDevices.append(generateField(DbStructure::FIELD_DEVICES_ISRESTRICTED, QVariant::Bool, false));
    tableDevices.append(generateField(DbStructure::FIELD_DEVICES_ISTRUSTED, QVariant::Bool, false));
    tableDevices.append(generateField(DbStructure::FIELD_DEVICES_ISLOCALADDRESS, QVariant::Bool, false));
    tableDevices.append(generateField(DbStructure::FIELD_DEVICES_TMPSECUREKEY, QVariant::Int, true));

    QSqlRecord tableConnection;
    tableConnection.append(generateField(DbStructure::FIELD_DEVICECONNECTION_IPADDRESS, QVariant::String, false));
    tableConnection.append(generateField(DbStructure::FIELD_DEVICECONNECTION_DEVICEID, QVariant::String, false));
    tableConnection.append(generateField(DbStructure::FIELD_DEVICECONNECTION_ADAPTERNAME, QVariant::String, false));
    tableConnection.append(generateField(DbStructure::FIELD_DEVICECONNECTION_LASTCHECKEDDATE, QVariant::Time, false));

    QSqlRecord tableClipboard;
    tableClipboard.append(generateField(DbStructure::FIELD_CLIPBOARD_ID, QVariant::Int, false));
    tableClipboard.append(generateField(DbStructure::FIELD_CLIPBOARD_TEXT, QVariant::String, false));
    tableClipboard.append(generateField(DbStructure::FIELD_CLIPBOARD_TIME, QVariant::Time, false));

    QSqlRecord tableAssignee;
    tableAssignee.append(generateField(DbStructure::FIELD_TRANSFERASSIGNEE_GROUPID, QVariant::Int, false));
    tableAssignee.append(generateField(DbStructure::FIELD_TRANSFERASSIGNEE_DEVICEID, QVariant::String, false));
    tableAssignee.append(generateField(DbStructure::FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER, QVariant::String, true));
    tableAssignee.append(generateField(DbStructure::FIELD_TRANSFERASSIGNEE_ISCLONE, QVariant::Bool, true));

    auto *list = new QMap<QString, QSqlRecord>();

    list->insert(QString(DbStructure::TABLE_TRANSFER), tableTransfer);
    list->insert(QString(DbStructure::DIVIS_TRANSFER), tableTransfer); // Generate division table
    list->insert(QString(DbStructure::TABLE_TRANSFERGROUP), tableGroup);
    list->insert(QString(DbStructure::TABLE_DEVICES), tableDevices);
    list->insert(QString(DbStructure::TABLE_CLIPBOARD), tableClipboard);
    list->insert(QString(DbStructure::TABLE_TRANSFERASSIGNEE), tableAssignee);
    list->insert(QString(DbStructure::TABLE_DEVICECONNECTION), tableConnection);

    return list;
}

bool AccessDatabase::contains(const DatabaseObject &dbObject)
{
    QSqlQuery query = dbObject.getWhere().toSelectionQuery();

    query.exec();

    return query.next();
}

bool AccessDatabase::insert(DatabaseObject &dbObject)
{
    QSqlTableModel *model = DbStructure::gatherTableModel(this, &dbObject);
    bool state = model->insertRecord(-1, record(dbObject, *model));

    if (state)
        dbObject.onInsertingObject(this);

    delete model;
    return state;
}

bool AccessDatabase::publish(DatabaseObject &dbObject)
{
    return (this->contains(dbObject) && this->update(dbObject))
           || this->insert(dbObject);
}

bool AccessDatabase::reconstructRemote(DatabaseObject &dbObject)
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
    return selection.toDeletionQuery().exec();
}

bool AccessDatabase::remove(DatabaseObject &dbObject)
{
    if (remove(dbObject.getWhere())) {
        dbObject.onRemovingObject(this);
        return true;
    }

    return false;
}

bool AccessDatabase::update(DatabaseObject &dbObject)
{
    if (update(dbObject.getWhere(), dbObject.getValues())) {
        dbObject.onUpdatingObject(this);
        return true;
    }

    return false;
}

bool AccessDatabase::update(const SqlSelection &selection, const DbObjectMap &map)
{
    auto *tableModel = DbStructure::gatherTableModel(this, selection.tableName);
    QSqlQuery updateQuery = selection.toUpdateQuery(record(map, *tableModel));

    delete tableModel;
    return updateQuery.exec();
}

QSqlRecord AccessDatabase::record(const DatabaseObject &object, const QSqlTableModel &tableModel)
{
    return record(object.getValues(), tableModel);
}

QSqlRecord AccessDatabase::record(const DbObjectMap &objectMap, const QSqlTableModel &tableModel)
{
    QSqlRecord record = tableModel.record();

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

const char *DbStructure::transformType(const QVariant::Type &type)
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

QSqlTableModel *DbStructure::gatherTableModel(AccessDatabase *db, DatabaseObject *dbObject)
{
    return gatherTableModel(db, dbObject->getWhere().tableName);
}

QSqlTableModel *DbStructure::gatherTableModel(AccessDatabase *db, const QString &tableName)
{
    auto *model = new QSqlTableModel(db, *db->getDatabase());

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
    QString queryString = QString();

    if (fromStatement) {
        queryString += " from `";
        queryString += this->tableName;
        queryString += "`";
    }

    if (this->where.length() > 0) {
        queryString += " where ";
        queryString += this->where;
    }

    if (this->groupBy.length() > 0) {
        queryString += " group by ";
        queryString += this->groupBy;
    }

    if (this->orderBy.length() > 0) {
        queryString += " order by ";
        queryString += this->orderBy;
    }

    if (this->limit != -1) {
        queryString += " limit ";
        queryString += std::to_string(this->limit).c_str();
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
    this->groupBy += field;
    this->groupBy += "`";
    this->groupBy += ascending ? " asc" : " desc";
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
    this->orderBy += field;
    this->orderBy += "`";
    this->orderBy += ascending ? " asc" : " desc";
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
    queryString += generateSpecifierClause();

    QSqlQuery query;

    query.prepare(queryString);

    bindWhereClause(query);

    return query;
}

QSqlQuery SqlSelection::toInsertionQuery() const
{
    return QSqlQuery();
}

QSqlQuery SqlSelection::toSelectionQuery() const
{
    QString queryString = "select ";
    queryString += this->toSelectionColumns();
    queryString += generateSpecifierClause();

    QSqlQuery query;

    query.prepare(queryString);

    bindWhereClause(query);

    return query;
}

QString SqlSelection::toSelectionColumns() const
{
    QString output = QString();

    if (this->columns.empty())
        output += "*";
    else if (!this->columns.empty())
        for (const QString &item : this->columns) {
            if (output.length() > 0)
                output += ", ";

            output += "`";
            output += item;
            output += "`";
        }

    return output;
}

QSqlQuery SqlSelection::toUpdateQuery(const QSqlRecord &record) const
{
    QSqlQuery query;
    QString queryString = "update `";
    queryString += this->tableName;
    queryString += "` set ";
    QString updateIndex = QString();

    for (int iterator = 0; iterator < record.count(); iterator++) {
        QSqlField currentField = record.field(iterator);

        if (updateIndex.length() > 0)
            updateIndex += ", ";

        updateIndex += "`";
        updateIndex += currentField.name();
        updateIndex += "` = ?";

        query.addBindValue(currentField.value());
    }

    queryString += updateIndex;
    queryString += generateSpecifierClause(false);

    query.prepare(queryString);

    for (int iterator = 0; iterator < record.count(); iterator++)
        query.addBindValue(record.value(iterator));

    bindWhereClause(query);

    return query;
}

DatabaseObject::DatabaseObject(QObject *parent)
        : QObject(parent)
{
}


/***
     * This method generates another mapping just to load this class.
     *
     * A better way would be;
     * A- A method that accepts field name and its name and loading in a switch statement
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
