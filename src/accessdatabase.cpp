#include "accessdatabase.h"

#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>

using AccessDatabaseStructure::generateField;
using AccessDatabaseStructure::generateTableCreationSql;
using AccessDatabaseStructure::transformType;

AccessDatabase::AccessDatabase(QSqlDatabase* db)
{
    this->db = db;
}

void AccessDatabase::initialize()
{
    QSqlQuery queryExecutor(*db);
    QMap<QString, QSqlRecord>* tables = getPassiveTables();

    for (QString dbTableKey : tables->keys()) {
        QSqlRecord dbRecord = tables->take(dbTableKey);
        QString sql = generateTableCreationSql(dbTableKey, dbRecord, true);

        cout << sql.toStdString() << endl;

        queryExecutor.exec(sql);
    }

    delete tables;
}

QMap<QString, QSqlRecord>* AccessDatabase::getPassiveTables()
{
    QSqlRecord tableTransfer;
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_ID, QVariant::Int, false));
    tableTransfer.append(generateField(AccessDatabaseStructure::FIELD_TRANSFER_GROUPID, QVariant::Int, false));
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

    QMap<QString, QSqlRecord>* list = new QMap<QString, QSqlRecord>();

    list->insert(QString(AccessDatabaseStructure::TABLE_TRANSFER), tableTransfer);
    list->insert(QString(AccessDatabaseStructure::TABLE_TRANSFERGROUP), tableGroup);
    list->insert(QString(AccessDatabaseStructure::TABLE_DEVICES), tableDevices);
    list->insert(QString(AccessDatabaseStructure::TABLE_CLIPBOARD), tableClipboard);
    list->insert(QString(AccessDatabaseStructure::TABLE_TRANSFERASSIGNEE), tableAssignee);
    list->insert(QString(AccessDatabaseStructure::TABLE_DEVICECONNECTION), tableConnection);

    return list;
}

void AccessDatabase::insertObject(DatabaseObject* dbObject)
{
    QList<QSqlField>* values = dbObject->getValues();
}

QSqlField AccessDatabaseStructure::generateField(const QString& key, const QVariant::Type type, bool nullable)
{
    QSqlField field(key, type);
    field.setRequired(!nullable);

    return field;
}

const char* AccessDatabaseStructure::transformType(QVariant::Type type)
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
    case QVariant::Size:
    case QVariant::SizeF:
        return "size";
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

QString AccessDatabaseStructure::generateTableCreationSql(QString& tableName, QSqlRecord& record, bool mayExist)
{
    QString sql("CREATE TABLE ");

    if (mayExist)
        sql.append("IF NOT EXISTS ");

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
