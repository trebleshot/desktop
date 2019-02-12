//
// Created by veli on 2/12/19.
//

#ifndef TREBLESHOT_TRANSFERUTILS_H
#define TREBLESHOT_TRANSFERUTILS_H


#include <src/database/AccessDatabase.h>
#include <src/database/object/TransferObject.h>

class TransferUtils {
public:
    static SqlSelection *createSqlSelection(quint32 groupId, const QString &deviceId,
                                            TransferObject::Flag flag = TransferObject::Flag::Any,
                                            bool equals = true);
};


#endif //TREBLESHOT_TRANSFERUTILS_H
