#ifndef TRANSFEROBJECT_H
#define TRANSFEROBJECT_H

#include "src/database/accessdatabase.h"

class TransferObject : public DatabaseObject {
public:
    enum Type {
        Incoming,
        Outgoing
    };

    enum Flag {
        Interrupted,
        Pending,
        Removed,
        Done
    };

    QString friendlyName;
    QString file;
    QString fileMimeType;
    QString directory;
    size_t fileSize;
    size_t skippedBytes;
    int requestId;
    int groupId;
    int accessPort;
    Type type;
    Flag flag;

    TransferObject(int requestId = -1, QObject *parent = 0);

    SqlSelection *getWhere();

    QSqlRecord getValues(AccessDatabase *db);

    void onGeneratingValues(QSqlRecord record);
};

#endif // TRANSFEROBJECT_H
