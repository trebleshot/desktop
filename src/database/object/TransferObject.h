/*
* Copyright (C) 2019 Veli Tasalı
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
#include "TransferGroup.h"

typedef quint32 requestid;

class TransferObject : public DatabaseObject {
public:
	enum Type {
		Incoming,
		Outgoing
	};

	enum Flag {
		Any = -1, // Should not be passed as a value. Aimed to be used for TransferUtils::firstAvailableTransfer
		Interrupted,
		Pending,
		Removed,
		Done,
		Running
	};

	QString friendlyName;
	QString file;
	QString fileMimeType;
	QString directory;
	QString deviceId;
	size_t fileSize = 0;
	size_t skippedBytes = 0;
	requestid id = 0;
	groupid groupId = 0;
	int accessPort = 0;
	Type type = Type::Incoming;
	Flag flag = Flag::Pending;

	explicit TransferObject(requestid id = 0, const QString &deviceId = nullptr, const Type &type = Type::Incoming);

	bool isDivisionObject() const;

	SqlSelection getWhere() const override;

	DbObjectMap getValues() const override;

	void onGeneratingValues(const DbObjectMap &record) override;

	void onRemovingObject(AccessDatabase *db, DatabaseObject *parent) override;
};