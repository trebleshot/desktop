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
        Done
    };

    QString friendlyName;
    QString file;
    QString fileMimeType;
    QString directory;
    QString deviceId;
    size_t fileSize;
    size_t skippedBytes;
    requestid id;
    groupid groupId;
    int accessPort;
    Type type;
    Flag flag;

    explicit TransferObject(requestid id = 0, const QString &deviceId = nullptr, const Type &type = Type::Incoming);

    bool isDivisionObject() const;

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;
};

#endif // TRANSFEROBJECT_H
