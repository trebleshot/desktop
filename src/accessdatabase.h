#ifndef ACCESSDATABASE_H
#define ACCESSDATABASE_H

#include <QSqlDatabase>
#include <QSqlTableModel>
#include <iostream>

using namespace std;

class AccessDatabase;
class DatabaseObject;

namespace AccessDatabaseStructure {
const QString TABLE_TRANSFER = "transfer";
const QString FIELD_TRANSFER_ID = "id";
const QString FIELD_TRANSFER_FILE = "file";
const QString FIELD_TRANSFER_NAME = "name";
const QString FIELD_TRANSFER_SIZE = "size";
const QString FIELD_TRANSFER_MIME = "mime";
const QString FIELD_TRANSFER_TYPE = "type";
const QString FIELD_TRANSFER_DIRECTORY = "directory";
const QString FIELD_TRANSFER_SKIPPEDBYTES = "skippedBytes";
const QString FIELD_TRANSFER_GROUPID = "groupId";
const QString FIELD_TRANSFER_FLAG = "flag";
const QString FIELD_TRANSFER_ACCESSPORT = "accessPort";

const QString TABLE_TRANSFERGROUP = "transferGroup";
const QString FIELD_TRANSFERGROUP_ID = "id";
const QString FIELD_TRANSFERGROUP_SAVEPATH = "savePath";
const QString FIELD_TRANSFERGROUP_DATECREATED = "dateCreated";

const QString TABLE_DEVICES = "devices";
const QString FIELD_DEVICES_ID = "deviceId";
const QString FIELD_DEVICES_USER = "user";
const QString FIELD_DEVICES_BRAND = "brand";
const QString FIELD_DEVICES_MODEL = "model";
const QString FIELD_DEVICES_BUILDNAME = "buildName";
const QString FIELD_DEVICES_BUILDNUMBER = "buildNumber";
const QString FIELD_DEVICES_LASTUSAGETIME = "lastUsedTime";
const QString FIELD_DEVICES_ISRESTRICTED = "isRestricted";
const QString FIELD_DEVICES_ISTRUSTED = "isTrusted";
const QString FIELD_DEVICES_ISLOCALADDRESS = "isLocalAddress";
const QString FIELD_DEVICES_TMPSECUREKEY = "tmpSecureKey";

const QString TABLE_DEVICECONNECTION = "deviceConnection";
const QString FIELD_DEVICECONNECTION_IPADDRESS = "ipAddress";
const QString FIELD_DEVICECONNECTION_DEVICEID = "deviceId";
const QString FIELD_DEVICECONNECTION_ADAPTERNAME = "adapterName";
const QString FIELD_DEVICECONNECTION_LASTCHECKEDDATE = "lastCheckedDate";

const QString TABLE_CLIPBOARD = "clipboard";
const QString FIELD_CLIPBOARD_ID = "id";
const QString FIELD_CLIPBOARD_TEXT = "text";
const QString FIELD_CLIPBOARD_TIME = "time";

const QString TABLE_WRITABLEPATH = "writablePath";
const QString FIELD_WRITABLEPATH_TITLE = "title";
const QString FIELD_WRITABLEPATH_PATH = "path";

const QString TABLE_TRANSFERASSIGNEE = "transferAssignee";
const QString FIELD_TRANSFERASSIGNEE_GROUPID = "groupId";
const QString FIELD_TRANSFERASSIGNEE_DEVICEID = "deviceId";
const QString FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER = "connectionAdapter";
const QString FIELD_TRANSFERASSIGNEE_ISCLONE = "isClone";

extern QSqlField generateField(const QString& key, const QVariant::Type type, bool nullable = true);

extern QString generateTableCreationSql(QString& tableName, QSqlRecord& record, bool mayExist = false);

extern const char* transformType(QVariant::Type type);
}

class DatabaseObject : public QObject {
    Q_OBJECT

public:
    virtual QSqlQuery getWhere() = 0;

    virtual QList<QSqlField>* getValues() = 0;

    virtual void onGeneratingValues(QList<QSqlField>* db) = 0;

    virtual void onUpdatingObject(AccessDatabase* db) {}

    virtual void onInsertingObject(AccessDatabase* db) {}

    virtual void onRemovingObject(AccessDatabase* db) {}
};

class AccessDatabase : public QObject {
    QSqlDatabase* db;

public:
    AccessDatabase(QSqlDatabase* db);
    void initialize();

    static QMap<QString, QSqlRecord>* getPassiveTables();

    void insertObject(DatabaseObject* dbObject);
};

#endif // ACCESSDATABASE_H
