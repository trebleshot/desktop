//
// Created by veli on 2/12/19.
//

#include <QtGui/QDesktopServices>
#include <QtCore/QDir>
#include <QSqlError>
#include <cmath>
#include "TransferUtils.h"
#include "AppUtils.h"

SqlSelection TransferUtils::createSqlSelection(groupid groupId, const QString &deviceId,
                                               TransferObject::Flag flag, bool equals)
{
    SqlSelection sqlSelection;
    sqlSelection.setTableName(DB_TABLE_TRANSFER);

    QString sqlStatement = QString("%1 = ? AND %2 = ?")
            .arg(DB_FIELD_TRANSFER_GROUPID)
            .arg(DB_FIELD_TRANSFER_DEVICEID);

    sqlSelection.whereArgs << groupId
                           << deviceId;

    if (flag != TransferObject::Flag::Any) {
        sqlStatement.append(QString("AND %1 %2 ?")
                                    .arg(DB_FIELD_TRANSFER_FLAG)
                                    .arg(equals ? "==" : "!="));

        sqlSelection.whereArgs << flag;
    }

    sqlSelection.setWhere(sqlStatement);

    return sqlSelection;
}

TransferObject TransferUtils::firstAvailableTransfer(groupid groupId, const QString &deviceId)
{
    TransferObject object;
    firstAvailableTransfer(object, groupId, deviceId);

    return object;
}

bool TransferUtils::firstAvailableTransfer(TransferObject &object, groupid groupId, const QString &deviceId)
{
    SqlSelection selection;

    selection.tableName = DB_TABLE_TRANSFER;
    selection.setWhere(QString("`%1` = ? AND `%2` = ? AND `%3` = ? AND `%4` = ?")
                               .arg(DB_FIELD_TRANSFER_GROUPID)
                               .arg(DB_FIELD_TRANSFER_DEVICEID)
                               .arg(DB_FIELD_TRANSFER_FLAG)
                               .arg(DB_FIELD_TRANSFER_TYPE));
    selection.setLimit(1);
    selection.setOrderBy(QString("`%1` ASC, `%2` ASC")
                                 .arg(DB_FIELD_TRANSFER_DIRECTORY)
                                 .arg(DB_FIELD_TRANSFER_NAME));

    selection.whereArgs << groupId
                        << deviceId
                        << TransferObject::Flag::Pending
                        << TransferObject::Type::Incoming;

    auto query = selection.toSelectionQuery();

    query.exec();

    auto taskResult = query.first();

    if (taskResult)
        object.generateValues(query.record());
    else
        qDebug() << query.lastError() << endl << query.executedQuery();

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

QString TransferUtils::getSavePath(const TransferGroup &group)
{
    return (group.savePath != nullptr && group.savePath.length() > 0 && QDir().mkdir(group.savePath))
           ? group.savePath
           : getDefaultSavePath();
}

QString TransferUtils::getIncomingFilePath(const TransferGroup &transferGroup, const TransferObject &object)
{
    QDir savePath(getSavePath(transferGroup));

    if (object.directory != nullptr && object.directory.length() > 0) {
        savePath.mkpath(object.directory);
        savePath.setPath(savePath.filePath(object.directory));
    }

    return savePath.filePath(object.file);
}

QString TransferUtils::getUniqueFileName(const QString &filePath, bool tryActualFile)
{
    if (tryActualFile && !QFile::exists(filePath))
        return filePath;

    QFile file(filePath);
    QFileInfo fileInfo(file);
    QString fileName = fileInfo.fileName();
    int pathStartPosition = fileName.lastIndexOf(".");

    QString mergedName = pathStartPosition != -1 ? fileName.left(pathStartPosition) : fileName;
    QString fileExtension = pathStartPosition != -1 ? fileName.mid(pathStartPosition) : "";

    if (mergedName.length() == 0 && fileExtension.length() > 0) {
        mergedName = fileExtension;
        fileExtension = "";
    }

    for (int exceed = 1; exceed < 999; exceed++) {
        QString newName = fileInfo.dir().filePath(
                QString("%1 (%2)%3")
                        .arg(mergedName)
                        .arg(exceed)
                        .arg(fileExtension));

        if (!QFile::exists(newName))
            return newName;
    }

    return fileName;
}

QString TransferUtils::saveIncomingFile(const TransferGroup &group, TransferObject &object)
{
    QFile file(getIncomingFilePath(group, object));
    QFileInfo fileInfo(file);
    QString uniqueName = getUniqueFileName(fileInfo.dir().filePath(object.friendlyName), true);
    QFile uniqueFile(uniqueName);
    QFileInfo uniqueFileInfo(uniqueFile);

    if (file.exists()) {
        if (file.rename(uniqueFile.fileName()))
            object.file = uniqueFileInfo.fileName();
    }

    object.flag = TransferObject::Flag::Done;

    gDatabase->publish(object);

    return QString();
}

TransferGroupInfo TransferUtils::getInfo(const TransferGroup &group)
{
    const auto &assignees = getAllAssigneeInfo(group);

    SqlSelection selection;

    selection.setTableName(assignees.empty() ? DB_DIVIS_TRANSFER : DB_TABLE_TRANSFER);
    selection.setWhere(QString("`%1` = ?").arg(DB_FIELD_TRANSFER_GROUPID));
    selection.whereArgs << group.id;

    const auto &list = gDatabase->castQuery(selection, TransferObject());

    TransferGroupInfo groupInfo(group, assignees, list.size());

    for (const auto &object: list) {
        if (!groupInfo.hasError
            && (object.flag == TransferObject::Flag::Interrupted || object.flag == TransferObject::Flag::Removed))
            groupInfo.hasError = true;

        groupInfo.totalBytes += object.fileSize;

        if (object.flag == TransferObject::Flag::Done) {
            groupInfo.completed++;
            groupInfo.completedBytes += object.fileSize;
        }

        if (!groupInfo.hasIncoming && object.type == TransferObject::Type::Incoming)
            groupInfo.hasIncoming = true;

        if (!groupInfo.hasOutgoing && object.type == TransferObject::Type::Outgoing)
            groupInfo.hasOutgoing = true;
    }

    return groupInfo;
}

AssigneeInfo TransferUtils::getInfo(const TransferAssignee &assignee)
{
    try {
        auto *device = new NetworkDevice(assignee.deviceId);

        gDatabase->reconstruct(*device);

        return AssigneeInfo(*device, assignee);
    } catch (...) {
        // do nothing
    }

    return AssigneeInfo();
}

QList<AssigneeInfo> TransferUtils::getAllAssigneeInfo(const TransferGroup &group)
{
    SqlSelection selection;

    selection.setTableName(DB_TABLE_TRANSFERASSIGNEE);
    selection.setWhere(QString("`%1` = ?").arg(DB_FIELD_TRANSFERASSIGNEE_GROUPID));
    selection.whereArgs << group.id;

    QList<AssigneeInfo> returnedList;
    const auto &assigneeList = gDatabase->castQuery(selection, TransferAssignee());

    for (const auto &assignee : assigneeList)
        returnedList << getInfo(assignee);

    return returnedList;
}

QString TransferUtils::sizeExpression(size_t bytes, bool notUseByte)
{
    int unit = notUseByte ? 1000 : 1024;

    if (bytes < unit)
        return QString("%1 B").arg(bytes);

    int expression = (int) (log(bytes) / log(unit));

    return QString("%1 %2B%3")
            .arg(QString::asprintf("%.1f", bytes / pow(unit, expression)))
            .arg(QString(notUseByte ? "kMGTPE" : "KMGTPE").at(expression - 1))
            .arg(notUseByte ? "i" : "");
}
