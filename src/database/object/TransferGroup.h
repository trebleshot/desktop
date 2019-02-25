//
// Created by veli on 9/25/18.
//

#ifndef TREBLESHOT_TRANSFERGROUP_H
#define TREBLESHOT_TRANSFERGROUP_H

#include <ctime>
#include <QtCore/QString>
#include <src/config/Config.h>
#include <src/database/AccessDatabase.h>

class TransferAssignee;

class TransferGroup : public DatabaseObject {
public:
    groupid id = 0;
    time_t dateCreated = 0;
    QString savePath;

    explicit TransferGroup(groupid groupId = 0);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;

    void onInsertingObject(AccessDatabase *db) override;

    void onRemovingObject(AccessDatabase *db, DatabaseObject* parent) override;
};

class TransferAssignee : public DatabaseObject {
public:
    groupid groupId = 0;
    QString deviceId;
    QString connectionAdapter;
    bool isClone = false;

    explicit TransferAssignee(groupid groupId = 0, const QString &deviceId = nullptr,
                              const QString &connectionAdapter = nullptr);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;

    void onRemovingObject(AccessDatabase *db, DatabaseObject *parent) override;
};

#endif //TREBLESHOT_TRANSFERGROUP_H
