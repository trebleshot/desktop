/*
* Copyright (C) 2019 Veli Tasalı, created on 2/12/19
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <QtGui/QDesktopServices>
#include <QtCore/QDir>
#include <QSqlError>
#include <cmath>
#include <src/broadcast/SeamlessClient.h>

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
		sqlStatement.append(QString(" AND %1 %2 ?").arg(DB_FIELD_TRANSFER_FLAG).arg(equals ? "==" : "!="));
		sqlSelection.whereArgs << flag;
	}

	sqlSelection.setWhere(sqlStatement);

	return sqlSelection;
}


void TransferUtils::createTransferMap(GThread *thread, QList<TransferObject *> *objectList, const TransferGroup &group,
                                      const QMimeDatabase &mimeDatabase, requestid &requestId, const QString &filePath,
                                      const QString &directory)
{
	if (thread->interrupted())
		return;

	emit thread->statusUpdate(-1, -1, directory);
	QFileInfo fileInfo(filePath);

	if (fileInfo.isFile()) {
		QFile file(filePath);
		auto object = new TransferObject(requestId++, nullptr, TransferObject::Type::Outgoing);

		object->groupId = group.id;
		object->friendlyName = fileInfo.fileName();
		object->file = filePath;
		object->fileSize = static_cast<size_t>(file.size());
		object->fileMimeType = mimeDatabase.mimeTypeForFile(fileInfo).name();
		object->flag = TransferObject::Flag::Pending;

		if (!directory.isEmpty())
			object->directory = directory;

		file.close();

		*objectList << object;
	} else if (fileInfo.isDir()) {
		QDir dir = filePath;
		QString currentDirectory;

		if (!directory.isEmpty())
			currentDirectory.append(QString("%1%2").arg(directory).arg(QDir::separator()));

		currentDirectory.append(dir.dirName());

		const auto &entries = dir.entryList(QDir::AllDirs | QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst);

		for (const auto &thisEntry : entries) {
			createTransferMap(thread, objectList, group, mimeDatabase, requestId, dir.filePath(thisEntry),
			                  currentDirectory);
		}
	}
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

Reason TransferUtils::getErrorReason(const QString &error)
{
	if (error == KEYWORD_ERROR_NOT_FOUND)
		return Reason::NotFound;
	else if (error == KEYWORD_ERROR_NOT_ALLOWED)
		return Reason::Blocked;
	else if (error == KEYWORD_ERROR_NOT_ACCESSIBLE)
		return Reason::NotAccessible;

	return Reason::Unknown;
}

QString TransferUtils::getFlagString(TransferObject::Flag flag)
{
	switch (flag) {
		case TransferObject::Flag::Any:
			return QObject::tr("Any");
		case TransferObject::Flag::Interrupted:
			return QObject::tr("Interrupted");
		case TransferObject::Flag::Done:
			return QObject::tr("Done");
		case TransferObject::Flag::Pending:
			return QObject::tr("Pending");
		case TransferObject::Flag::Removed:
			return QObject::tr("Removed");
		case TransferObject::Flag::Running:
			return QObject::tr("Running");
	}

	return QString();
}

QString TransferUtils::getSavePath(const TransferGroup &group)
{
	return (!group.savePath.isEmpty() && QDir(group.savePath).mkpath(".")) ? group.savePath : getDefaultSavePath();
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

	return QString();
}

TransferGroupInfo TransferUtils::getInfo(const TransferGroup &group)
{
	TransferGroupInfo groupInfo(group);
	getInfo(group, groupInfo);
	return groupInfo;
}

void TransferUtils::getInfo(const TransferGroup &group, TransferGroupInfo &groupInfo)
{
	groupInfo.assignees.clear();
	getAllAssigneeInfo(group, groupInfo.assignees);

	SqlSelection selection;
	selection.setTableName(groupInfo.assignees.empty() ? DB_DIVIS_TRANSFER : DB_TABLE_TRANSFER);
	selection.setWhere(QString("`%1` = ?").arg(DB_FIELD_TRANSFER_GROUPID));
	selection.whereArgs << group.id;

	QList<TransferObject> list;
	gDatabase->castQuery(selection, list);

	getInfo(groupInfo, list, false);
}

void TransferUtils::getInfo(TransferGroupInfo &groupInfo, const QList<TransferObject> &list, bool resetFirst)
{
	if (resetFirst)
		groupInfo.resetCalculations();

	groupInfo.total = list.size();

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
}

AssigneeInfo TransferUtils::getInfo(const TransferAssignee &assignee)
{
	try {
		NetworkDevice device(assignee.deviceId);

		gDatabase->reconstruct(device);

		return AssigneeInfo(device, assignee);
	} catch (...) {
		// do nothing
	}

	return AssigneeInfo();
}

void TransferUtils::getAllAssigneeInfo(const TransferGroup &group, QList<AssigneeInfo> &list)
{
	SqlSelection selection;

	selection.setTableName(DB_TABLE_TRANSFERASSIGNEE);
	selection.setWhere(QString("`%1` = ?").arg(DB_FIELD_TRANSFERASSIGNEE_GROUPID));
	selection.whereArgs << group.id;

	QList<AssigneeInfo> returnedList;
	QList<TransferAssignee> assigneeList;

	gDatabase->castQuery(selection, assigneeList);

	for (const auto &assignee : assigneeList)
		list << getInfo(assignee);
}

QString TransferUtils::sizeExpression(size_t bytes, bool notUseByte)
{
	size_t unit = notUseByte ? 1000 : 1024;

	if (bytes < unit)
		return QString("%1 B").arg(bytes);

	int expression = (int) (log(bytes) / log(unit));

	return QString("%1 %2B%3")
			.arg(QString::asprintf("%.1f", bytes / pow(unit, expression)))
			.arg(QString(notUseByte ? "kMGTPE" : "KMGTPE").at(expression - 1))
			.arg(notUseByte ? "i" : "");
}

void TransferUtils::getPaths(const QList<QUrl> &urls, QList<QString> &list)
{
	for (const auto &url : urls)
		list << url.toLocalFile();
}

void TransferUtils::startTransfer(groupid groupId, const QString &deviceId)
{
	auto *client = new SeamlessClient(groupId, deviceId, true);
	client->start();
}
