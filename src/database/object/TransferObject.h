#ifndef TRANSFEROBJECT_H
#define TRANSFEROBJECT_H

#include "src/database/AccessDatabase.h"

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
    QString deviceId;
    size_t fileSize;
    size_t skippedBytes;
    int requestId;
    int groupId;
    int accessPort;
    Type type;
    Flag flag;

    explicit TransferObject(int requestId = -1, const QString &deviceId = nullptr,
                            const Type &type = Type::Incoming, QObject *parent = nullptr);

    bool isDivisionObject();

    SqlSelection *getWhere() override;

    QSqlRecord getValues(AccessDatabase *db) override;

    void onGeneratingValues(QSqlRecord record) override;
};

#endif // TRANSFEROBJECT_H
