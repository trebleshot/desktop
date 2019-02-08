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
    qulonglong groupId;
    qulonglong dateCreated;
    QString savePath;

    explicit TransferGroup(ulong groupId = 0, QObject *parent = nullptr);

    SqlSelection *getWhere() override;

    QSqlRecord getValues(AccessDatabase *db) override;

    void onGeneratingValues(QSqlRecord record) override;
};

class TransferAssignee : public DatabaseObject {
public:
    qulonglong groupId;
    QString deviceId;
    QString connectionAdapter;
    bool isClone = false;

    explicit TransferAssignee(ulong groupId = 0, QString deviceId = nullptr, QString connectionAdapter = nullptr,
                              QObject *parent = nullptr);


    SqlSelection *getWhere() override;

    QSqlRecord getValues(AccessDatabase *db) override;

    void onGeneratingValues(QSqlRecord record) override;
};

#endif //TREBLESHOT_TRANSFERGROUP_H
