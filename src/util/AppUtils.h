/*
* Copyright (C) 2019 Veli Tasalı, created on 12/7/18
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

#include "src/config/Config.h"
#include "src/config/Keyword.h"
#include "src/database/AccessDatabase.h"
#include "Interrupter.h"
#include <QMutex>
#include <QApplication>
#include <QtCore/QSettings>
#include <src/database/object/NetworkDevice.h>
#include <src/database/object/TransferObject.h>
#include <src/broadcast/SeamlessServer.h>

#define gTaskMgr AppUtils::getTransferTaskManager()
#define gDatabase AppUtils::getDatabase()
#define gDbSignal emit AppUtils::getDatabaseSignaller()

class TransferTaskManager;

class TransferTask : public Interrupter {

public:
	friend TransferTaskManager;
	friend SeamlessServer;

	explicit TransferTask(groupid groupId, const QString &deviceId, TransferObject::Type type)
	{
		m_groupId = groupId;
		m_deviceId = deviceId;
		m_type = type;
	}

protected:
	TransferObject::Type m_type = TransferObject::Type::Incoming;
	groupid m_groupId;
	QString m_deviceId;
};

class TransferTaskManager : public QObject {
Q_OBJECT
	QMutex m_mutex;

protected:
	QList<TransferTask *> m_activeTasks;

public:
	friend class TransferTask;

	void attachTask(TransferTask *task)
	{
		m_mutex.lock();
		m_activeTasks << task;
		emit taskAdded(task->m_groupId, task->m_deviceId, task->m_type);
		m_mutex.unlock();
	}

	bool hasActiveTasksFor(groupid groupId, const QString &deviceId = QString())
	{
		m_mutex.lock();

		for (const auto *task : m_activeTasks)
			if (task->m_groupId == groupId && (deviceId == nullptr || task->m_deviceId == deviceId)) {
				m_mutex.unlock();
				return true;
			}

		m_mutex.unlock();
		return false;
	}

	QList<TransferTask> getActiveTasksFor(groupid groupId)
	{
		QList<TransferTask> tasks;

		m_mutex.lock();
		for (const auto *task : m_activeTasks) {
			if (task->m_groupId == groupId)
				tasks << TransferTask(*task);
		}
		m_mutex.unlock();

		return tasks;
	}

	int pauseTasks(groupid groupId, const QString &deviceId = QString())
	{
		int foundTotal = 0;

		m_mutex.lock();
		for (auto *task : m_activeTasks)
			if (task->m_groupId == groupId && (deviceId == nullptr || deviceId == task->m_deviceId)) {
				task->interrupt();
				foundTotal++;
			}
		m_mutex.unlock();

		return foundTotal;
	}

	void detachTask(TransferTask *task)
	{
		m_mutex.lock();
		m_activeTasks.removeOne(task);
		emit taskRemoved(task->m_groupId, task->m_deviceId, task->m_type);
		m_mutex.unlock();
	}

signals:

	void taskAdded(groupid groupId, const QString &deviceId, int type);

	void taskRemoved(groupid groupId, const QString &deviceId, int type);

	void taskStatus(groupid groupId, const QString &deviceId, int type, qint64 completed);
};

class AppUtils {
public:
	static bool applyAdapterName(DeviceConnection &connection);

	static void applyDeviceToJSON(QJsonObject &object);

	static AccessDatabase *getDatabase();

	static QThread *getDatabaseWorker();

	static AccessDatabaseSignaller *getDatabaseSignaller();

	static QSettings &getDefaultSettings();

	static QString getDeviceId();

	static NetworkDevice getLocalDevice();

	static TransferTaskManager *getTransferTaskManager();
};