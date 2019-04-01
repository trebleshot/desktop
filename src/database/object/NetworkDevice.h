/*
* Copyright (C) 2019 Veli Tasalı, created on 9/25/18
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

#include <QtNetwork/QHostAddress>
#include <QtCore/QString>
#include <src/database/AccessDatabase.h>

class DeviceConnection;

class NetworkDevice : public DatabaseObject {
public:
	QString id;
	QString brand;
	QString model;
	QString nickname;
	QString versionName;
	int versionNumber = 0;
	int tmpSecureKey = 0;
	time_t lastUsageTime = 0;
	bool isTrusted = false;
	bool isRestricted = false;
	bool isLocalAddress = false;

	explicit NetworkDevice(const QString &id = nullptr);

	SqlSelection getWhere() const override;

	DbObjectMap getValues() const override;

	void onGeneratingValues(const DbObjectMap &record) override;

	void onRemovingObject(AccessDatabase *db, DatabaseObject *parent) override;
};

class DeviceConnection : public DatabaseObject {
public:
	QString adapterName;
	QHostAddress hostAddress;
	QString deviceId;
	time_t lastCheckedDate = 0;

	DeviceConnection() = default;

	explicit DeviceConnection(const QString &deviceId, const QString &adapterName);

	explicit DeviceConnection(const QHostAddress &hostAddress);

	SqlSelection getWhere() const override;

	DbObjectMap getValues() const override;

	void onGeneratingValues(const DbObjectMap &record) override;
};