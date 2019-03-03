#ifndef ACCESSDATABASE_H
#define ACCESSDATABASE_H

#include <QDebug>
#include <QMimeData>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlResult>
#include <QSqlTableModel>
#include <QVariant>
#include <iostream>

#define DB_TABLE_TRANSFER "transfer"
#define DB_DIVIS_TRANSFER "divisTansfer"
#define DB_FIELD_TRANSFER_ID "id"
#define DB_FIELD_TRANSFER_FILE "file"
#define DB_FIELD_TRANSFER_NAME "name"
#define DB_FIELD_TRANSFER_SIZE "size"
#define DB_FIELD_TRANSFER_MIME "mime"
#define DB_FIELD_TRANSFER_TYPE "type"
#define DB_FIELD_TRANSFER_DIRECTORY "directory"
#define DB_FIELD_TRANSFER_SKIPPEDBYTES "skippedBytes"
#define DB_FIELD_TRANSFER_DEVICEID "deviceId"
#define DB_FIELD_TRANSFER_GROUPID "groupId"
#define DB_FIELD_TRANSFER_FLAG "flag"
#define DB_FIELD_TRANSFER_ACCESSPORT "accessPort"

#define DB_TABLE_TRANSFERGROUP "transferGroup"
#define DB_FIELD_TRANSFERGROUP_ID "id"
#define DB_FIELD_TRANSFERGROUP_SAVEPATH "savePath"
#define DB_FIELD_TRANSFERGROUP_DATECREATED "dateCreated"

#define DB_TABLE_DEVICES "devices"
#define DB_FIELD_DEVICES_ID "deviceId"
#define DB_FIELD_DEVICES_USER "user"
#define DB_FIELD_DEVICES_BRAND "brand"
#define DB_FIELD_DEVICES_MODEL "model"
#define DB_FIELD_DEVICES_BUILDNAME "buildName"
#define DB_FIELD_DEVICES_BUILDNUMBER "buildNumber"
#define DB_FIELD_DEVICES_LASTUSAGETIME "lastUsedTime"
#define DB_FIELD_DEVICES_ISRESTRICTED "isRestricted"
#define DB_FIELD_DEVICES_ISTRUSTED "isTrusted"
#define DB_FIELD_DEVICES_ISLOCALADDRESS "isLocalAddress"
#define DB_FIELD_DEVICES_TMPSECUREKEY "tmpSecureKey"

#define DB_TABLE_DEVICECONNECTION "deviceConnection"
#define DB_FIELD_DEVICECONNECTION_IPADDRESS "ipAddress"
#define DB_FIELD_DEVICECONNECTION_DEVICEID "deviceId"
#define DB_FIELD_DEVICECONNECTION_ADAPTERNAME "adapterName"
#define DB_FIELD_DEVICECONNECTION_LASTCHECKEDDATE "lastCheckedDate"

#define DB_TABLE_CLIPBOARD "clipboard"
#define DB_FIELD_CLIPBOARD_ID "id"
#define DB_FIELD_CLIPBOARD_TEXT "text"
#define DB_FIELD_CLIPBOARD_TIME "time"

#define DB_TABLE_TRANSFERASSIGNEE "transferAssignee"
#define DB_FIELD_TRANSFERASSIGNEE_GROUPID "groupId"
#define DB_FIELD_TRANSFERASSIGNEE_DEVICEID "deviceId"
#define DB_FIELD_TRANSFERASSIGNEE_CONNECTIONADAPTER "connectionAdapter"
#define DB_FIELD_TRANSFERASSIGNEE_ISCLONE "isClone"

typedef QMap<QString, QVariant> DbObjectMap;

class AccessDatabase;

class DatabaseObject;

class SqlSelection;

class DbChangeSubscriber;

enum ChangeType {
    Any = -1,
    Delete,
    Update,
    Insert
};

namespace DbStructure {
    extern QSqlField generateField(const QString &key, const QVariant::Type &type, bool nullable = true);

    extern QSqlField generateField(const QString &key, const QVariant &type);

    extern QString generateTableCreationSql(const QString &tableName, const QSqlRecord &record, bool mayExist = false);

    extern QString transformType(const QVariant::Type &type);

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

    virtual void onRemovingObject(AccessDatabase *db, DatabaseObject *parent)
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

    template<typename T = DatabaseObject>
    QList<T> castQuery(const SqlSelection &sqlSelection, const T &classInstance)
    {
        QList<T> resultList;
        QSqlQuery query = sqlSelection.toSelectionQuery();

        query.exec();

        if (query.first())
            do {
                T dbObject;
                dbObject.generateValues(query.record());
                resultList.append(dbObject);
            } while (query.next());

        return resultList;
    }

    void initialize();

    template<typename T = DatabaseObject>
    bool removeAsObject(const SqlSelection &selection, const T &type, DatabaseObject *parent = nullptr)
    {
        const QList<T> &objects = castQuery(selection, type);

        for (auto resultingObject : objects)
            resultingObject.onRemovingObject(this, parent);

        return remove(selection);
    }

public slots:

    bool commit();

    bool contains(const DatabaseObject &dbObject);

    bool contains(const SqlSelection &dbObject);

    void doSynchronized(const std::function<void(AccessDatabase *)> &listener)
    {
        listener(this);
    }

    bool insert(DatabaseObject &dbObject);

    bool publish(DatabaseObject &dbObject);

    bool reconstructSilently(DatabaseObject &dbObject);

    void reconstruct(DatabaseObject &dbObject);

    QSqlRecord record(const DatabaseObject &object);

    QSqlRecord record(const DatabaseObject &object, const QSqlTableModel &tableModel);

    QSqlRecord record(const DbObjectMap &objectMap, const QSqlTableModel &tableModel);

    bool remove(const SqlSelection &selection);

    bool remove(DatabaseObject &dbObject);

    bool transaction();

    bool update(DatabaseObject &dbObject);

    bool update(const SqlSelection &selection, const DbObjectMap &record);

signals:

    void databaseChanged(const SqlSelection &, ChangeType);

    bool signalPublish(DatabaseObject &);
};

class AccessDatabaseSignaller : public QObject {
Q_OBJECT

public:
    explicit AccessDatabaseSignaller(AccessDatabase *db, QObject *parent = nullptr)
            : QObject(parent)
    {
        connect(this, &AccessDatabaseSignaller::commit, db, &AccessDatabase::commit, Qt::BlockingQueuedConnection);
        connect(this, SIGNAL(contains(
                                     const DatabaseObject & )),
                db, SLOT(contains(
                                 const DatabaseObject & )), Qt::BlockingQueuedConnection);
        connect(this, SIGNAL(contains(
                                     const SqlSelection & )),
                db, SLOT(contains(
                                 const SqlSelection & )), Qt::BlockingQueuedConnection);
        connect(this, &AccessDatabaseSignaller::doNonDirect, db, &AccessDatabase::doSynchronized);
        connect(this, &AccessDatabaseSignaller::doSynchronized, db, &AccessDatabase::doSynchronized,
                Qt::BlockingQueuedConnection);
        connect(this, &AccessDatabaseSignaller::insert, db, &AccessDatabase::insert, Qt::BlockingQueuedConnection);
        connect(this, &AccessDatabaseSignaller::publish, db, &AccessDatabase::publish, Qt::BlockingQueuedConnection);
        connect(this, &AccessDatabaseSignaller::reconstruct,
                db, &AccessDatabase::reconstructSilently, Qt::BlockingQueuedConnection);

        connect(this, SIGNAL(remove(
                                     const SqlSelection & )),
                db, SLOT(remove(
                                 const SqlSelection & )), Qt::BlockingQueuedConnection);

        connect(this, SIGNAL(remove(DatabaseObject & )),
                db, SLOT(remove(DatabaseObject & )), Qt::BlockingQueuedConnection);

        connect(this, &AccessDatabaseSignaller::transaction, db, &AccessDatabase::transaction, Qt::BlockingQueuedConnection);

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

    bool commit();

    bool contains(const DatabaseObject &dbObject);

    bool contains(const SqlSelection &selection);

    void doNonDirect(const std::function<void(AccessDatabase *)> &listener);

    void doSynchronized(const std::function<void(AccessDatabase *)> &listener);

    bool insert(DatabaseObject &dbObject);

    bool publish(DatabaseObject &dbObject);

    bool reconstruct(DatabaseObject &dbObject);

    bool remove(const SqlSelection &selection);

    bool remove(DatabaseObject &dbObject);

    bool transaction();

    bool update(DatabaseObject &dbObject);

    bool update(const SqlSelection &selection, const DbObjectMap &values);
};

#endif // ACCESSDATABASE_H
