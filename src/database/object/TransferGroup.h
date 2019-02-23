//
// Created by veli on 9/25/18.
//

#ifndef TREBLESHOT_TRANSFERGROUP_H
#define TREBLESHOT_TRANSFERGROUP_H

#include <ctime>
#include <QtCore/QString>
#include <src/database/AccessDatabase.h>

class TransferAssignee;

class TransferGroup : public DatabaseObject {
public:
    quint32 id = 0;
    time_t dateCreated = 0;
    QString savePath;

    explicit TransferGroup(quint32 groupId = 0);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;
};

class TransferAssignee : public DatabaseObject {
public:
    quint32 groupId = 0;
    QString deviceId;
    QString connectionAdapter;
    bool isClone = false;

    explicit TransferAssignee(quint32 groupId = 0, const QString &deviceId = nullptr,
                              const QString &connectionAdapter = nullptr);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;
};

#endif //TREBLESHOT_TRANSFERGROUP_H
