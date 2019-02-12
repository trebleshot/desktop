//
// Created by veli on 2/12/19.
//

#include "TransferUtils.h"

SqlSelection *TransferUtils::createSqlSelection(quint32 groupId, const QString &deviceId,
                                                TransferObject::Flag flag, bool equals)
{
    auto *sqlSelection = new SqlSelection;
    sqlSelection->setTableName(DbStructure::TABLE_TRANSFER);

    QString sqlStatement = QString("%1 AND %2")
            .arg(DbStructure::FIELD_TRANSFER_GROUPID)
            .arg(DbStructure::FIELD_TRANSFER_DEVICEID);

    sqlSelection->whereArgs << groupId
                            << deviceId;

    if (flag != TransferObject::Flag::Any) {
        sqlStatement.append(QString("%1 %2 ?")
                                    .arg(DbStructure::FIELD_TRANSFER_FLAG)
                                    .arg(equals ? "==" : "!="));

        sqlSelection->whereArgs << flag;
    }

    sqlSelection->setWhere(sqlStatement);

    return sqlSelection;
}
