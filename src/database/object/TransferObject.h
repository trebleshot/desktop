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
        Any = -1,
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
    quint32 requestId;
    quint32 id;
    int accessPort;
    Type type;
    Flag flag;

    explicit TransferObject(quint32 id = 0, const QString &deviceId = nullptr, const Type &type = Type::Incoming);

    bool isDivisionObject() const;

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;
};

#endif // TRANSFEROBJECT_H
