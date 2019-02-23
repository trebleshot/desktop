#ifndef ACCESSDATABASE_H
#define ACCESSDATABASE_H

#define gDatabase AppUtils::getDatabase()
#define gDbSignal emit AppUtils::getDatabaseSignaller()

#include <QDebug>
#include <QMimeData>
#include <QSqlDatabase>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>
#include <QSqlTableModel>
#include <QVariant>
#include <iostream>

#define DB_TABLE_TRANSFER QString("transfer")
#define DB_DIVIS_TRANSFER QString("divisTansfer")
#define DB_FIELD_TRANSFER_ID QString("id")
#define DB_FIELD_TRANSFER_FILE QString("file")
#define DB_FIELD_TRANSFER_NAME QString("name")
#define DB_FIELD_TRANSFER_SIZE QString("size")
#define DB_FIELD_TRANSFER_MIME QString("mime")
#define DB_FIELD_TRANSFER_TYPE QString("type")
#define DB_FIELD_TRANSFER_DIRECTORY QString("directory")
#define DB_FIELD_TRANSFER_SKIPPEDBYTES QString("skippedBytes")
#define DB_FIELD_TRANSFER_DEVICEID QString("deviceId")
#define DB_FIELD_TRANSFER_GROUPID QString("groupId")
#define DB_FIELD_TRANSFER_FLAG QString("flag")
#define DB_FIELD_TRANSFER_ACCESSPORT QString("accessPort")

#define DB_TABLE_TRANSFERGROUP QString("transferGroup")
#define DB_FIELD_TRANSFERGROUP_ID QString("id")
#define DB_FIELD_TRANSFERGROUP_SAVEPATH QString("savePath")
#define DB_FIELD_TRANSFERGROUP_DATECREATED QString("dateCreated")

#define DB_TABLE_DEVICES QString("devices")
#define DB_FIELD_DEVICES_ID QString("deviceId")
#define DB_FIELD_DEVICES_USER QString("user")
#define DB_FIELD_DEVICES_BRAND QString("brand")
#define DB_FIELD_DEVICES_MODEL QString("model")
#define DB_FIELD_DEVICES_BUILDNAME QString("buildName")
#define DB_FIELD_DEVICES_BUILDNUMBER QString("buildNumber")
#define DB_FIELD_DEVICES_LASTUSAGETIME QString("lastUsedTime")
#define DB_FIELD_DEVICES_ISRESTRICTED QString("isRestricted")
#define DB_FIELD_DEVICES_ISTRUSTED QString("isTrusted")
#define DB_FIELD_DEVICES_ISLOCALADDRESS QString("isLocalAddress")
#define DB_FIELD_DEVICES_TMPSECUREKEY QString("tmpSecureKey")

#define DB_TABLE_DEVICECONNECTION QString("deviceConnection")
#define DB_FIELD_DEVICECONNECTION_IPADDRESS QString("ipAddress")
#define DB_FIELD_DEVICECONNECTION_DEVICEID QString("deviceId")
#define DB_FIELD_DEVICECONNECTION_ADAPTERNAME QString("adapterName")
#define DB_FIELD_DEVICECONNECTION_LASTCHECKEDDATE QString("lastCheckedDate")

#define DB_TABLE_CLIPBOARD QString("clipboard")
#define DB_FIELD_CLIPBOARD_ID QString("id")
#define DB_FIELD_CLIPBOARD_TEXT QString("text")
#define DB_FIELD_CLIPBOARD_TIME QString("time")

#define DB_TABLE_TRANSFERASSIGNEE QString("transferAssignee")
#define DB_FIELD_TRANSFERASSIGNEE_GROUPID QString("groupId")
#define DB_FIELD_TRANSFERASSIGNEE_DEVICEID QString("deviceId")
#define DB_FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER QString("connectionAdapter")
#define DB_FIELD_TRANSFERASSIGNEE_ISCLONE QString("isClone")

using namespace std;

typedef QMap<QString, QVariant> DbObjectMap;

class AccessDatabase;

class DatabaseObject;

class SqlSelection;

namespace DbStructure {
    extern QSqlField generateField(const QString &key, const QVariant::Type &type, bool nullable = true);

    extern QSqlField generateField(const QString &key, const QVariant &type);

    extern QString generateTableCreationSql(const QString &tableName, const QSqlRecord &record, bool mayExist = false);

    extern const char *transformType(const QVariant::Type &type);

    extern QSqlTableModel *gatherTableModel(const DatabaseObject &dbObject);

    extern QSqlTableModel *gatherTableModel(const QString &tableName);
}

class SqlSelection {
public:
    QString tag;
    QString tableName;
    QStringList columns;
    QString where;
    QList<QVariant> whereArgs;
    QString groupBy;
    QString having;
    QString orderBy;
    int limit = -1;

    explicit SqlSelection() = default;

    void bindWhereClause(QSqlQuery &query) const;

    QString generateSpecifierClause(bool fromStatement = true) const;

    void setHaving(const QString &having);

    void setGroupBy(const QString &field, bool ascending);

    void setGroupBy(const QString &orderBy);

    void setLimit(int limit);

    void setOrderBy(const QString &field, bool ascending);

    void setOrderBy(const QString &limit);

    void setTableName(const QString &tableName);

    void setWhere(const QString &whereString);

    QSqlQuery toDeletionQuery() const;

    QSqlQuery toInsertionQuery() const;

    QSqlQuery toSelectionQuery() const;

    QString toSelectionColumns() const;

    QSqlQuery toUpdateQuery(const QSqlRecord &query) const;
};

class DatabaseObject {
public:

    DatabaseObject() = default;

    virtual ~DatabaseObject() = default;

    void generateValues(const QSqlRecord &record);

    virtual SqlSelection getWhere() const = 0;

    virtual DbObjectMap getValues() const = 0;

    virtual void onGeneratingValues(const DbObjectMap &record) = 0;

    virtual void onUpdatingObject(AccessDatabase *db)
    {
        // Implement by overriding
    }

    virtual void onInsertingObject(AccessDatabase *db)
    {
        // Implement by overriding
    }

    virtual void onRemovingObject(AccessDatabase *db)
    {
        // Implement by overriding
    }
};

class AccessDatabase : public QObject {
Q_OBJECT
    QSqlDatabase *db;

public:
    explicit AccessDatabase(QSqlDatabase *db, QObject *parent = nullptr);

    static QMap<QString, QSqlRecord> getPassiveTables();

    QSqlDatabase *getDatabase();

    template<typename T>
    QList<T> castQuery(const SqlSelection &sqlSelection, const T &classInstance)
    {
        QList<T> resultList;

        if (std::is_base_of<DatabaseObject, T>().value) {
            QSqlQuery query = sqlSelection.toSelectionQuery();

            query.exec();

            if (query.first())
                do {
                    T dbObject;
                    dbObject.generateValues(query.record());
                    resultList.append(dbObject);
                } while (query.next());
        } else
            qDebug() << "Received an unknown class type which should have been a base DatabaseObject";

        return resultList;
    }

    void initialize();

public slots:

    bool contains(const DatabaseObject &dbObject);

    void doSynchronized(const std::function<void(AccessDatabase *)> &listener)
    {
        listener(this);
    }

    bool insert(DatabaseObject &dbObject);

    bool publish(DatabaseObject &dbObject);

    bool reconstructRemote(DatabaseObject &dbObject);

    void reconstruct(DatabaseObject &dbObject);

    QSqlRecord record(const DatabaseObject &object);

    QSqlRecord record(const DatabaseObject &object, const QSqlTableModel &tableModel);

    QSqlRecord record(const DbObjectMap &objectMap, const QSqlTableModel &tableModel);

    bool remove(const SqlSelection &selection);

    bool remove(DatabaseObject &dbObject);

    bool update(DatabaseObject &dbObject);

    bool update(const SqlSelection &selection, const DbObjectMap &record);

signals:

    bool signalPublish(DatabaseObject &);
};

class AccessDatabaseSignaller : public QObject {
Q_OBJECT

public:
    explicit AccessDatabaseSignaller(AccessDatabase *db, QObject *parent = nullptr)
            : QObject(parent)
    {
        connect(this, &AccessDatabaseSignaller::contains, db, &AccessDatabase::contains, Qt::BlockingQueuedConnection);
        connect(this, &AccessDatabaseSignaller::doNonDirect, db, &AccessDatabase::doSynchronized);
        connect(this, &AccessDatabaseSignaller::doSynchronized, db, &AccessDatabase::doSynchronized,
                Qt::BlockingQueuedConnection);
        connect(this, &AccessDatabaseSignaller::insert, db, &AccessDatabase::insert, Qt::BlockingQueuedConnection);
        connect(this, &AccessDatabaseSignaller::publish, db, &AccessDatabase::publish, Qt::BlockingQueuedConnection);
        connect(this, &AccessDatabaseSignaller::reconstruct,
                db, &AccessDatabase::reconstructRemote, Qt::BlockingQueuedConnection);

        connect(this, SIGNAL(remove(
                                     const SqlSelection & )),
                db, SLOT(remove(
                                 const SqlSelection & )), Qt::BlockingQueuedConnection);

        connect(this, SIGNAL(remove(DatabaseObject & )),
                db, SLOT(remove(DatabaseObject & )), Qt::BlockingQueuedConnection);

        connect(this, SIGNAL(update(
                                     const SqlSelection &, const DbObjectMap & )),
                db, SLOT(update(
                                 const SqlSelection &, const DbObjectMap & )), Qt::BlockingQueuedConnection);

        connect(this, SIGNAL(update(DatabaseObject & )),
                db, SLOT(update(DatabaseObject & )), Qt::BlockingQueuedConnection);
    }

    void operator<<(const std::function<void(AccessDatabase *)> &listener)
    {
        emit doSynchronized(listener);
    }

signals:

    bool contains(const DatabaseObject &dbObject);

    void doNonDirect(const std::function<void(AccessDatabase *)> &listener);

    void doSynchronized(const std::function<void(AccessDatabase *)> &listener);

    bool insert(DatabaseObject &dbObject);

    bool publish(DatabaseObject &dbObject);

    bool reconstruct(DatabaseObject &dbObject);

    bool remove(const SqlSelection &selection);

    bool remove(DatabaseObject &dbObject);

    bool update(DatabaseObject &dbObject);

    bool update(const SqlSelection &selection, const DbObjectMap &values);
};

#endif // ACCESSDATABASE_H
