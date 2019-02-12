//
// Created by veli on 2/12/19.
//

#ifndef TREBLESHOT_TRANSFERUTILS_H
#define TREBLESHOT_TRANSFERUTILS_H


#include <src/database/AccessDatabase.h>
#include <src/database/object/TransferObject.h>
#include <QtCore/QFile>
#include <src/database/object/TransferGroup.h>

class TransferUtils {
public:
    static SqlSelection *createSqlSelection(quint32 groupId, const QString &deviceId,
                                            TransferObject::Flag flag = TransferObject::Flag::Any,
                                            bool equals = true);

    static TransferObject *firstAvailableTransfer(TransferObject *object, quint32 groupId, const QString &deviceId);

    static QString getDefaultSavePath();

    static QString getIncomingFilePath(TransferGroup *transferGroup, TransferObject *object);

    static QString getSavePath(TransferGroup *group);
};


#endif //TREBLESHOT_TRANSFERUTILS_H
