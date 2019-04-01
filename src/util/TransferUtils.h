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

#pragma once

#include <src/database/AccessDatabase.h>
#include <src/database/object/TransferObject.h>
#include <QtCore/QFile>
#include <src/database/object/TransferGroup.h>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QUrl>
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

	TransferGroupInfo() = default;

	TransferGroupInfo(const TransferGroup &group)
	{
		this->group = group;
	}

	void resetCalculations()
	{
		total = 0;
		completed = 0;
		hasError = false;
		hasIncoming = false;
		hasOutgoing = false;
		totalBytes = 0;
		completedBytes = 0;
	}
};

enum Reason {
	Unknown,
	Blocked,
	Rejected,
	NotAccessible,
	NotFound,
	NoPendingTransfer
};

class TransferUtils {
public:
	static SqlSelection createSqlSelection(groupid groupId, const QString &deviceId,
	                                       TransferObject::Flag flag = TransferObject::Flag::Any,
	                                       bool equals = true);

	static void createTransferMap(GThread *thread, QList<TransferObject *> *objectList,
	                              const TransferGroup &group, const QMimeDatabase &mimeDatabase,
	                              requestid &requestId, const QString &filePath, const QString &directory = nullptr);

	static void getPaths(const QList<QUrl> &urls, QList<QString> &list);

	static TransferObject firstAvailableTransfer(groupid groupId, const QString &deviceId);

	static bool firstAvailableTransfer(TransferObject &object, groupid groupId, const QString &deviceId);

	static QString getDefaultSavePath();

	static QString getFlagString(TransferObject::Flag flag);

	static Reason getErrorReason(const QString& errorCode);

	static QString getIncomingFilePath(const TransferGroup &transferGroup, const TransferObject &object);

	static QString getSavePath(const TransferGroup &group);

	static QString getUniqueFileName(const QString &filePath, bool tryActualFile);

	static void getAllAssigneeInfo(const TransferGroup &group, QList<AssigneeInfo> &list);

	static TransferGroupInfo getInfo(const TransferGroup &group);

	static void getInfo(const TransferGroup &group, TransferGroupInfo &info);

	static void getInfo(TransferGroupInfo &info, const QList<TransferObject> &objectList, bool resetFirst = false);

	static AssigneeInfo getInfo(const TransferAssignee &assignee);

	static void startTransfer(groupid groupId, const QString &deviceId);

	static QString saveIncomingFile(const TransferGroup &group, TransferObject &object);

	static QString sizeExpression(size_t size, bool notUseByte);
};
