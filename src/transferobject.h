#ifndef TRANSFEROBJECT_H
#define TRANSFEROBJECT_H

#include "accessdatabase.h"

class TransferObject : public DatabaseObject {
public:
    TransferObject();

    // DatabaseObject interface
public:
    QSqlQuery getWhere();
    QList<QSqlField>* getValues();
    void onGeneratingValues(QList<QSqlField>* db);
};

#endif // TRANSFEROBJECT_H
