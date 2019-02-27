//
// Created by veli on 2/12/19.
//

#ifndef TREBLESHOT_TRANSFERUTILS_H
#define TREBLESHOT_TRANSFERUTILS_H


#include <src/database/AccessDatabase.h>
#include <src/database/object/TransferObject.h>
#include <QtCore/QFile>
#include <src/database/object/TransferGroup.h>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QMimeDatabase>
#include "src/database/object/NetworkDevice.h"
#include "GThread.h"

struct AssigneeInfo {
    NetworkDevice device;
    TransferAssignee assignee;
    bool valid = false;

public:
    AssigneeInfo()
    {
        this->valid = false;
    }

    AssigneeInfo(const NetworkDevice &device, const TransferAssignee &assignee)
    {
        this->device = device;
        this->assignee = assignee;
        this->valid = true;
    }
};

struct TransferGroupInfo {
    TransferGroup group;
    QList<AssigneeInfo> assignees;
    int total = 0;
    int completed = 0;
    bool hasError = false;
    bool hasIncoming = false;
    bool hasOutgoing = false;
    size_t totalBytes = 0;
    size_t completedBytes = 0;

    TransferGroupInfo(const TransferGroup &group, const QList<AssigneeInfo> &assignees, int total = 0,
                      int completed = 0, bool hasError = false, bool hasIncoming = false, bool hasOutgoing = false,
                      size_t totalBytes = 0, size_t completedBytes = 0)
    {
        this->group = group;
        this->assignees = assignees;
        this->total = total;
        this->completed = completed;
        this->hasError = hasError;
        this->hasIncoming = hasIncoming;
        this->hasOutgoing = hasOutgoing;
        this->totalBytes = totalBytes;
        this->completedBytes = completedBytes;
    }
};

class TransferUtils {
public:
    static SqlSelection createSqlSelection(groupid groupId, const QString &deviceId,
                                           TransferObject::Flag flag = TransferObject::Flag::Any,
                                           bool equals = true);

    static void createTransferMap(GThread *thread, QList<TransferObject>* objectList,
                                  const TransferGroup &group, const QMimeDatabase &mimeDatabase,
                                  requestid &requestId, const QString &filePath, const QString &directory = nullptr);

    static TransferObject firstAvailableTransfer(groupid groupId, const QString &deviceId);

    static bool firstAvailableTransfer(TransferObject &object, groupid groupId, const QString &deviceId);

    static QString getDefaultSavePath();

    static QString getIncomingFilePath(const TransferGroup &transferGroup, const TransferObject &object);

    static QString getSavePath(const TransferGroup &group);

    static QString getUniqueFileName(const QString &filePath, bool tryActualFile);

    static QList<AssigneeInfo> getAllAssigneeInfo(const TransferGroup &group);

    static TransferGroupInfo getInfo(const TransferGroup &group);

    static AssigneeInfo getInfo(const TransferAssignee &assignee);

    static QString saveIncomingFile(const TransferGroup &group, TransferObject &object);

    static QString sizeExpression(size_t size, bool notUseByte);
};


#endif //TREBLESHOT_TRANSFERUTILS_H
