#include <utility>

#include <utility>

#include <utility>

//
// Created by veli on 2/12/19.
//

#ifndef TREBLESHOT_TRANSFERUTILS_H
#define TREBLESHOT_TRANSFERUTILS_H


#include <src/database/AccessDatabase.h>
#include <src/database/object/TransferObject.h>
#include <QtCore/QFile>
#include <src/database/object/TransferGroup.h>
#include "src/database/object/NetworkDevice.h"

struct AssigneeInfo {
    NetworkDevice device;
    TransferAssignee assignee;
    bool valid = false;

public:
    AssigneeInfo()
    {
        this->valid = false;
    }

    AssigneeInfo(const NetworkDevice &device, const TransferAssignee &assignee) : device(device), assignee(assignee)
    {
        this->valid = true;
    }
};

struct TransferGroupInfo {
    TransferGroup group;
    QList<AssigneeInfo *> assignees;
    int total = 0;
    int completed = 0;
    bool hasError = false;
    bool hasIncoming = false;
    bool hasOutgoing = false;
    size_t totalBytes = 0;
    size_t completedBytes = 0;

    TransferGroupInfo() = default;

    TransferGroupInfo(const TransferGroupInfo &other)
    {

    }

    TransferGroupInfo(const TransferGroup &group, QList<AssigneeInfo *> assignees, int total = 0,
                      int completed = 0, bool hasError = false, bool hasIncoming = false, bool hasOutgoing = false,
                      size_t totalBytes = 0, size_t completedBytes = 0) : group(group), assignees(std::move(assignees))
    {
        this->total = total;
        this->completed = completed;
        this->hasError = hasError;
        this->hasIncoming = hasIncoming;
        this->hasOutgoing = hasOutgoing;
        this->totalBytes = totalBytes;
        this->completedBytes = completedBytes;
    };
};

class TransferUtils {
public:
    static SqlSelection *createSqlSelection(quint32 groupId, const QString &deviceId,
                                            TransferObject::Flag flag = TransferObject::Flag::Any,
                                            bool equals = true);

    static TransferObject *firstAvailableTransfer(quint32 groupId, const QString &deviceId);

    static bool firstAvailableTransfer(TransferObject *object, quint32 groupId, const QString &deviceId);

    static QString getDefaultSavePath();

    static QString getIncomingFilePath(TransferGroup *transferGroup, TransferObject *object);

    static QString getSavePath(TransferGroup *group);

    static QString getUniqueFileName(const QString &filePath, bool tryActualFile);

    static QList<AssigneeInfo *> getAllAssigneeInfo(const TransferGroup &group);

    static TransferGroupInfo getInfo(const TransferGroup &group);

    static AssigneeInfo getInfo(const TransferAssignee &assignee);

    static QString saveIncomingFile(TransferGroup *group, TransferObject *object);

    static QString sizeExpression(size_t size, bool notUseByte);
};


#endif //TREBLESHOT_TRANSFERUTILS_H
