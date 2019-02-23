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
Q_OBJECT
public:
    quint32 groupId;
    time_t dateCreated;
    QString savePath;

    TransferGroup(const TransferGroup &group)
    {

    }

    explicit TransferGroup(quint32 groupId = 0, QObject *parent = nullptr);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;
};

class TransferAssignee : public DatabaseObject {
public:
    quint32 groupId;
    QString deviceId;
    QString connectionAdapter;
    bool isClone = false;

    TransferAssignee(const TransferAssignee &assignee)
    {

    }

    explicit TransferAssignee(quint32 groupId = 0, const QString &deviceId = nullptr,
                              const QString &connectionAdapter = nullptr,
                              QObject *parent = nullptr);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;
};

#endif //TREBLESHOT_TRANSFERGROUP_H
