#include <c++/7/bits/unique_ptr.h>
#include "AccessDatabase.h"

using AccessDatabaseStructure::generateField;
using AccessDatabaseStructure::generateTableCreationSql;
using AccessDatabaseStructure::transformType;

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

    for (QString dbTableKey : tables->keys()) {
        QSqlRecord dbRecord = tables->take(dbTableKey);
        QString sql = generateTableCreationSql(dbTableKey, dbRecord, true);

        queryExecutor.exec(sql);
    }

    delete tables;
}

QMap<QString, QSqlRecord> *AccessDatabase::getPassiveTables()
{
    QSqlRecord tableTransfer;
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_ID, QVariant::Int, false));
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_GROUPID, QVariant::Int, false));
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_DEVICEID, QVariant::String, true));
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_FILE, QVariant::String, true));
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_NAME, QVariant::String, false));
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_SIZE, QVariant::Int, true));
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_MIME, QVariant::String, true));
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_TYPE, QVariant::String, false));
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_DIRECTORY, QVariant::String, true));
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_ACCESSPORT, QVariant::Int, true));
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_SKIPPEDBYTES, QVariant::Size, false));
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_FLAG, QVariant::Int, true));

    QSqlRecord tableGroup;
    tableGroup.append(generateField(AccessDatabaseStructure::FIELD_TRANSFERGROUP_ID, QVariant::Int, false));
    tableGroup.append(generateField(AccessDatabaseStructure::FIELD_TRANSFERGROUP_DATECREATED, QVariant::Time, false));
    tableGroup.append(generateField(AccessDatabaseStructure::FIELD_TRANSFERGROUP_SAVEPATH, QVariant::String, true));

    QSqlRecord tableDevices;
    tableDevices.append(generateField(AccessDatabaseStructure::FIELD_DEVICES_ID, QVariant::String, false));
    tableDevices.append(generateField(AccessDatabaseStructure::FIELD_DEVICES_USER, QVariant::String, false));
    tableDevices.append(generateField(AccessDatabaseStructure::FIELD_DEVICES_BRAND, QVariant::String, false));
    tableDevices.append(generateField(AccessDatabaseStructure::FIELD_DEVICES_MODEL, QVariant::String, false));
    tableDevices.append(generateField(AccessDatabaseStructure::FIELD_DEVICES_BUILDNAME, QVariant::String, false));
    tableDevices.append(generateField(AccessDatabaseStructure::FIELD_DEVICES_BUILDNUMBER, QVariant::Int, false));
    tableDevices.append(generateField(AccessDatabaseStructure::FIELD_DEVICES_LASTUSAGETIME, QVariant::Time, false));
    tableDevices.append(generateField(AccessDatabaseStructure::FIELD_DEVICES_ISRESTRICTED, QVariant::Bool, false));
    tableDevices.append(generateField(AccessDatabaseStructure::FIELD_DEVICES_ISTRUSTED, QVariant::Bool, false));
    tableDevices.append(generateField(AccessDatabaseStructure::FIELD_DEVICES_ISLOCALADDRESS, QVariant::Bool, false));
    tableDevices.append(generateField(AccessDatabaseStructure::FIELD_DEVICES_TMPSECUREKEY, QVariant::Int, true));

    QSqlRecord tableConnection;
    tableConnection.append(generateField(AccessDatabaseStructure::FIELD_DEVICECONNECTION_IPADDRESS, QVariant::String, false));
    tableConnection.append(generateField(AccessDatabaseStructure::FIELD_DEVICECONNECTION_DEVICEID, QVariant::String, false));
    tableConnection.append(generateField(AccessDatabaseStructure::FIELD_DEVICECONNECTION_ADAPTERNAME, QVariant::String, false));
    tableConnection.append(generateField(AccessDatabaseStructure::FIELD_DEVICECONNECTION_LASTCHECKEDDATE, QVariant::Time, false));

    QSqlRecord tableClipboard;
    tableClipboard.append(generateField(AccessDatabaseStructure::FIELD_CLIPBOARD_ID, QVariant::Int, false));
    tableClipboard.append(generateField(AccessDatabaseStructure::FIELD_CLIPBOARD_TEXT, QVariant::String, false));
    tableClipboard.append(generateField(AccessDatabaseStructure::FIELD_CLIPBOARD_TIME, QVariant::Time, false));

    QSqlRecord tableAssignee;
    tableAssignee.append(generateField(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_GROUPID, QVariant::Int, false));
    tableAssignee.append(generateField(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_DEVICEID, QVariant::String, false));
    tableAssignee.append(generateField(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER, QVariant::String, true));
    tableAssignee.append(generateField(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_ISCLONE, QVariant::Bool, true));

    auto *list = new QMap<QString, QSqlRecord>();

    list->insert(QString(AccessDatabaseStructure::TABLE_TRANSFER), tableTransfer);
    list->insert(QString(AccessDatabaseStructure::DIVIS_TRANSFER), tableTransfer); // Generate division table
    list->insert(QString(AccessDatabaseStructure::TABLE_TRANSFERGROUP), tableGroup);
    list->insert(QString(AccessDatabaseStructure::TABLE_DEVICES), tableDevices);
    list->insert(QString(AccessDatabaseStructure::TABLE_CLIPBOARD), tableClipboard);
    list->insert(QString(AccessDatabaseStructure::TABLE_TRANSFERASSIGNEE), tableAssignee);
    list->insert(QString(AccessDatabaseStructure::TABLE_DEVICECONNECTION), tableConnection);

    return list;
}

bool AccessDatabase::contains(DatabaseObject *dbObject)
{
    QSqlQuery *query = dbObject->getWhere()->toSelectionQuery();
    bool wasSuccessful;

    query->exec();

    wasSuccessful = query->next();

    delete query;
    return wasSuccessful;
}

bool AccessDatabase::insert(DatabaseObject *dbObject)
{
    QSqlTableModel *model = AccessDatabaseStructure::gatherTableModel(this, dbObject);
    bool wasSuccessful;

    wasSuccessful = model->insertRecord(-1, dbObject->getValues(this));

    delete model;
    return wasSuccessful;
}

bool AccessDatabase::publish(DatabaseObject *dbObject)
{
    return (this->contains(dbObject) && this->update(dbObject))
           || this->insert(dbObject);
}

bool AccessDatabase::reconstructRemote(DatabaseObject *dbObject)
{
    try {
        reconstruct(dbObject);
        return true;
    } catch (...) {
        // do nothing here
    }

    return false;
}

void AccessDatabase::reconstruct(DatabaseObject *dbObject)
{
    QSqlQuery *query = dbObject->getWhere()->toSelectionQuery();
    bool wasSuccessful = false;

    query->exec();

    if (query->next()) {
        dbObject->onGeneratingValues(query->record());
        wasSuccessful = true;
    }

    delete query;

    if (!wasSuccessful)
        throw exception();
}

bool AccessDatabase::remove(SqlSelection *selection)
{
    return selection->toDeletionQuery()->exec();
}

bool AccessDatabase::remove(DatabaseObject *dbObject)
{
    return remove(dbObject->getWhere());
}

bool AccessDatabase::update(DatabaseObject *dbObject)
{
    SqlSelection *selection = dbObject->getWhere();
    QSqlQuery *updateQuery = selection->toUpdateQuery(dbObject->getValues(this));
    bool wasSuccessful;

    wasSuccessful = updateQuery->exec();

    delete selection;
    delete updateQuery;
    return wasSuccessful;
}

QSqlField AccessDatabaseStructure::generateField(const QString &key, const QVariant::Type &type, bool nullable)
{
    QSqlField field(key, type);
    field.setRequired(!nullable);

    return field;
}

const char *AccessDatabaseStructure::transformType(QVariant::Type type)
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

// todo: const ??
QString AccessDatabaseStructure::generateTableCreationSql(QString &tableName, QSqlRecord &record, bool mayExist)
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

QSqlField AccessDatabaseStructure::generateField(const QString &key, const QVariant &value)
{
    QSqlField field(key);
    field.setValue(value);

    return field;
}

QSqlTableModel *AccessDatabaseStructure::gatherTableModel(AccessDatabase *db, DatabaseObject *dbObject)
{
    auto *model = new QSqlTableModel(db, *db->getDatabase());

    model->setTable(dbObject->getWhere()->tableName);

    return model;
}

void SqlSelection::bindWhereClause(QSqlQuery &query)
{
    for (const QVariant &whereArg : this->whereArgs)
        query.addBindValue(whereArg);
}

QString SqlSelection::generateSpecifierClause(bool fromStatement)
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
        queryString += ", limit ";
        queryString += std::to_string(this->limit).c_str();
    }

    return queryString;
}

SqlSelection *SqlSelection::setHaving(QString having)
{
    this->having = std::move(having);
    return this;
}

SqlSelection *SqlSelection::setGroupBy(QString field, bool ascending)
{
    this->groupBy = "`";
    this->groupBy += field;
    this->groupBy += "`";
    this->groupBy += ascending ? " asc" : " desc";

    return this;
}

SqlSelection *SqlSelection::setGroupBy(QString orderBy)
{
    this->orderBy = std::move(orderBy);
    return this;
}

SqlSelection *SqlSelection::setLimit(int limit)
{
    this->limit = limit;
    return this;
}

SqlSelection *SqlSelection::setOrderBy(QString field, bool ascending)
{
    this->orderBy = "`";
    this->orderBy += field;
    this->orderBy += "`";
    this->orderBy += ascending ? " asc" : " desc";

    return this;
}

SqlSelection *SqlSelection::setOrderBy(QString field)
{
    this->orderBy = std::move(field);
    return this;
}

SqlSelection *SqlSelection::setTableName(QString tableName)
{
    this->tableName = std::move(tableName);
    return this;
}

SqlSelection *SqlSelection::setWhere(const QString &whereString)
{
    this->where = whereString;
    return this;
}

QSqlQuery *SqlSelection::toDeletionQuery()
{
    QString queryString = "delete";
    queryString += generateSpecifierClause();

    QSqlQuery *query = new QSqlQuery;

    query->prepare(queryString);

    bindWhereClause(*query);

    return query;
}

QSqlQuery *SqlSelection::toInsertionQuery()
{
}

QSqlQuery *SqlSelection::toSelectionQuery()
{
    QString queryString = "select ";
    queryString += this->toSelectionColumns();
    queryString += generateSpecifierClause();

    QSqlQuery *query = new QSqlQuery;

    query->prepare(queryString);

    bindWhereClause(*query);

    return query;
}

QString SqlSelection::toSelectionColumns()
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

QSqlQuery *SqlSelection::toUpdateQuery(QSqlRecord record)
{
    QSqlQuery *query = new QSqlQuery;
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

        query->addBindValue(currentField.value());
    }

    queryString += updateIndex;
    queryString += generateSpecifierClause(false);

    query->prepare(queryString);

    for (int iterator = 0; iterator < record.count(); iterator++)
        query->addBindValue(record.value(iterator));

    bindWhereClause(*query);

    return query;
}

DatabaseObject::DatabaseObject(QObject *parent)
        : QObject(parent)
{
}
