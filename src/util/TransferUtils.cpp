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
    TransferObject *object = new TransferObject;
    firstAvailableTransfer(object, groupId, deviceId);

    return object;
}

bool TransferUtils::firstAvailableTransfer(TransferObject *object, quint32 groupId, const QString &deviceId)
{
    auto *sqlSelection = new SqlSelection;

    sqlSelection->tableName = DbStructure::TABLE_TRANSFER;
    sqlSelection->setWhere(QString("`%1` = ? AND `%2` = ? AND %3 = ? AND %4 = ?")
                                   .arg(DbStructure::FIELD_TRANSFER_GROUPID)
                                   .arg(DbStructure::FIELD_TRANSFER_DEVICEID)
                                   .arg(DbStructure::FIELD_TRANSFER_FLAG)
                                   .arg(DbStructure::FIELD_TRANSFER_TYPE))
            ->setLimit(0)
            ->setOrderBy(QString("`%1` ASC, `%2` ASC")
                                 .arg(DbStructure::FIELD_TRANSFER_DIRECTORY)
                                 .arg(DbStructure::FIELD_TRANSFER_NAME));

    sqlSelection->whereArgs << groupId
                            << deviceId
                            << TransferObject::Flag::Pending
                            << TransferObject::Type::Incoming;

    auto *query = sqlSelection->toSelectionQuery();

    query->exec();

    if (query->first())
        object->onGeneratingValues(query->record());

    delete query;
    delete sqlSelection;

    return object;
}

QString TransferUtils::getDefaultSavePath()
{
    QString downloadsFolder = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::DownloadLocation);
    QString defaultFolder = AppUtils::getDefaultSettings().value("savePath", downloadsFolder).toString();

    QDir defaultFolderFile;

    if (defaultFolderFile.mkpath(defaultFolder))
        return defaultFolder;

    defaultFolderFile.mkpath(downloadsFolder);

    return downloadsFolder;
}

QString TransferUtils::getSavePath(TransferGroup *group)
{
    return (group->savePath != nullptr && group->savePath.length() > 0 && QDir().mkdir(group->savePath))
           ? group->savePath
           : getDefaultSavePath();
}

QString TransferUtils::getIncomingFilePath(TransferGroup *transferGroup, TransferObject *object)
{
    QDir savePath(getSavePath(transferGroup));

    if (object->directory != nullptr && object->directory.length() > 0) {
        savePath.mkpath(object->directory);
        savePath.setPath(savePath.filePath(object->directory));
    }

    return savePath.filePath(object->file);
}

