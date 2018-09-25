//
// Created by veli on 9/25/18.
//

#ifndef TREBLESHOT_TRANSFERGROUP_H
#define TREBLESHOT_TRANSFERGROUP_H

#include <ctime>
#include <QtCore/QString>
#include <src/database/accessdatabase.h>

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


#endif //TREBLESHOT_TRANSFERGROUP_H
