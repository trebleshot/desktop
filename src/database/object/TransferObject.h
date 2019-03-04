#ifndef TRANSFEROBJECT_H
#define TRANSFEROBJECT_H

#include "src/database/AccessDatabase.h"
#include "TransferGroup.h"

typedef quint32 requestid;

class TransferObject : public DatabaseObject {
public:
    enum Type {
        Incoming,
        Outgoing
    };

    enum Flag {
        Any = -1, // Should not be passed as a value. Aimed to be used for TransferUtils::firstAvailableTransfer
        Interrupted,
        Pending,
        Removed,
        Done,
        Running
    };

    QString friendlyName;
    QString file;
    QString fileMimeType;
    QString directory;
    QString deviceId;
    size_t fileSize = 0;
    size_t skippedBytes = 0;
    requestid id = 0;
    groupid groupId = 0;
    int accessPort = 0;
    Type type = Type::Incoming;
    Flag flag = Flag::Pending;

    explicit TransferObject(requestid id = 0, const QString &deviceId = nullptr, const Type &type = Type::Incoming);

    bool isDivisionObject() const;

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;

    void onRemovingObject(AccessDatabase *db, DatabaseObject* parent) override;
};

#endif // TRANSFEROBJECT_H
