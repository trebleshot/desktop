//
// Created by veli on 2/12/19.
//

#include <QtGui/QDesktopServices>
#include <QtCore/QDir>
#include "TransferUtils.h"
#include "AppUtils.h"

SqlSelection *TransferUtils::createSqlSelection(quint32 groupId, const QString &deviceId,
                                                TransferObject::Flag flag, bool equals)
{
    auto *sqlSelection = new SqlSelection;
    sqlSelection->setTableName(DbStructure::TABLE_TRANSFER);

    QString sqlStatement = QString("%1 = ? AND %2 = ?")
            .arg(DbStructure::FIELD_TRANSFER_GROUPID)
            .arg(DbStructure::FIELD_TRANSFER_DEVICEID);

    sqlSelection->whereArgs << groupId
                            << deviceId;

    if (flag != TransferObject::Flag::Any) {
        sqlStatement.append(QString("AND %1 %2 ?")
                                    .arg(DbStructure::FIELD_TRANSFER_FLAG)
                                    .arg(equals ? "==" : "!="));

        sqlSelection->whereArgs << flag;
    }

    sqlSelection->setWhere(sqlStatement);

    return sqlSelection;
}

TransferObject *TransferUtils::firstAvailableTransfer(quint32 groupId, const QString &deviceId)
{
    TransferObject *object = new TransferObject();

    gDbSignal->doSynchronized([object, groupId, deviceId](AccessDatabase *db) {
        auto *sqlSelection = new SqlSelection;

        sqlSelection->tableName = DbStructure::TABLE_TRANSFER;
        sqlSelection->setWhere(QString("`%1` = ? AND `%2` = ? AND %3 = ?")
                                       .arg(DbStructure::FIELD_TRANSFER_GROUPID)
                                       .arg(DbStructure::FIELD_TRANSFER_DEVICEID)
                                       .arg(DbStructure::FIELD_TRANSFER_FLAG)
                                       .arg(DbStructure::FIELD_TRANSFER_TYPE));

        sqlSelection->whereArgs << groupId
                                << deviceId
                                << TransferObject::Flag::Pending
                                << TransferObject::Type::Incoming;

        sqlSelection->setOrderBy(QString("`%1` ASC, `%2` ASC")
                                         .arg(DbStructure::FIELD_TRANSFER_DIRECTORY)
                                         .arg(DbStructure::FIELD_TRANSFER_NAME));

        auto *query = sqlSelection->toSelectionQuery();

        query->exec();

        if (query->first())
            object->onGeneratingValues(query->record());

        delete query;
        delete sqlSelection;
    });

    return object;
}

QString TransferUtils::getDefaultSavePath()
{
    QString downloadsFolder = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DownloadLocation);
    QString defaultFolder = AppUtils::getDefaultSettings().value("savePath", downloadsFolder).toString();

    QDir defaultFolderFile;

    if (defaultFolderFile.mkdir(defaultFolder))
        return defaultFolder;

    defaultFolderFile.mkdir(downloadsFolder);

    return downloadsFolder;
}

QString TransferUtils::getSavePath(TransferGroup *group)
{
    return (group->savePath != nullptr && group->savePath.length() > 0 && QDir(group->savePath).exists())
           ? group->savePath
           : getDefaultSavePath();
}

QString TransferUtils::getIncomingFilePath(TransferGroup *transferGroup, TransferObject *object)
{
    QDir savePath(getSavePath(transferGroup));

    return savePath.filePath(object->file);
}

