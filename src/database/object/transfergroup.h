//
// Created by veli on 9/25/18.
//

#ifndef TREBLESHOT_TRANSFERGROUP_H
#define TREBLESHOT_TRANSFERGROUP_H

#include <ctime>
#include <QtCore/QString>
#include <src/database/accessdatabase.h>

class TransferAssignee;

class TransferGroup : public DatabaseObject {
    Q_OBJECT
public:
    int groupId;
    int dateCreated;
    QString savePath;

    TransferGroup(int groupId = -1, QObject *parent = 0);

    SqlSelection *getWhere();

    QSqlRecord getValues(AccessDatabase *db);

    void onGeneratingValues(QSqlRecord record);
};

class TransferAssignee : public DatabaseObject {
public:
    int groupId;
    QString deviceId;
    QString connectionAdapter;
    bool isClone = false;

    TransferAssignee(int groupId, QString deviceId, QString connectionAdapter, QObject *parent = 0);

    SqlSelection *getWhere();

    QSqlRecord getValues(AccessDatabase *db);

    void onGeneratingValues(QSqlRecord record);
};

#endif //TREBLESHOT_TRANSFERGROUP_H
