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
    NetworkDevice* device;
    TransferAssignee* assignee;
    bool valid = false;
};

struct TransferGroupInfo {
    TransferGroup *group;
    QList<AssigneeInfo> assignees;
    int total;
    int completed;
    bool hasError;
    bool hasIncoming;
    bool hasOutgoing;
    size_t totalBytes;
    size_t completedBytes;
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

    static QList<AssigneeInfo> getAllAssigneeInfo(TransferGroup *group);

    static TransferGroupInfo getInfo(TransferGroup *group);

    static AssigneeInfo getInfo(TransferAssignee *assignee);

    static QString saveIncomingFile(TransferGroup *group, TransferObject *object);

    static QString sizeExpression(size_t size, bool notUseByte);
};


#endif //TREBLESHOT_TRANSFERUTILS_H
