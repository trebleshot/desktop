//
// Created by veli on 2/12/19.
//

#include <QtGui/QDesktopServices>
#include <QtCore/QDir>
#include <QSqlError>
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
    sqlSelection->setWhere(QString("`%1` = ? AND `%2` = ? AND `%3` = ? AND `%4` = ?")
                                   .arg(DbStructure::FIELD_TRANSFER_GROUPID)
                                   .arg(DbStructure::FIELD_TRANSFER_DEVICEID)
                                   .arg(DbStructure::FIELD_TRANSFER_FLAG)
                                   .arg(DbStructure::FIELD_TRANSFER_TYPE))
            ->setLimit(1)
            ->setOrderBy(QString("`%1` ASC, `%2` ASC")
                                 .arg(DbStructure::FIELD_TRANSFER_DIRECTORY)
                                 .arg(DbStructure::FIELD_TRANSFER_NAME));

    sqlSelection->whereArgs << groupId
                            << deviceId
                            << TransferObject::Flag::Pending
                            << TransferObject::Type::Incoming;

    auto *query = sqlSelection->toSelectionQuery();

    query->exec();

    auto taskResult = query->first();

    if (taskResult)
        object->onGeneratingValues(query->record());
    else
        qDebug() << query->lastError() << endl << query->executedQuery();

    delete query;
    delete sqlSelection;

    return taskResult;
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

QString TransferUtils::getUniqueFileName(const QString &filePath, bool tryActualFile)
{
    if (tryActualFile && !QFile::exists(filePath))
        return filePath;

    QFile file(filePath);
    QString fileName = file.fileName();
    int pathStartPosition = file.fileName().lastIndexOf(".");

    QString mergedName = pathStartPosition != -1 ? fileName.left(pathStartPosition) : fileName;
    QString fileExtension = pathStartPosition != -1 ? fileName.right(pathStartPosition) : "";

    if (mergedName.length() == 0 && fileExtension.length() > 0) {
        mergedName = fileExtension;
        fileExtension = "";
    }

    QFileInfo fileInfo(file);

    for (int exceed = 1; exceed < 999; exceed++) {
        QString newName = mergedName + " (" + exceed + ")" + fileExtension;

        if (!QFile::exists(fileInfo.dir().filePath(newName)))
            return newName;
    }

    return fileName;
}

QString TransferUtils::saveIncomingFile(TransferGroup *group, TransferObject *object)
{
    QFile file(getIncomingFilePath(group, object));
    QFileInfo fileInfo(file);
    QString uniqueName = getUniqueFileName(fileInfo.dir().filePath(object->friendlyName), true);
    QFile uniqueFile(uniqueName);

    qDebug() << file.fileName();

    if (file.exists()) {
        if (file.rename(uniqueFile.fileName()))
            object->file = uniqueFile.fileName();
    }

    object->flag = TransferObject::Flag::Done;

    gDatabase->publish(object);

    return QString();
}
