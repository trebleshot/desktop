//
// Created by veli on 12/7/18.
//

#ifndef TREBLESHOT_APPUTILS_H
#define TREBLESHOT_APPUTILS_H

#include "src/config/Config.h"
#include "src/config/Keyword.h"
#include "src/database/AccessDatabase.h"
#include "Interrupter.h"
#include <QMutex>
#include <QApplication>
#include <QtCore/QSettings>
#include <src/database/object/NetworkDevice.h>
#include <src/database/object/TransferObject.h>

#define gTaskMgr AppUtils::getTransferTaskManager()
#define gDatabase AppUtils::getDatabase()
#define gDbSignal emit AppUtils::getDatabaseSignaller()

class TransferTaskManager;

class TransferTask : public Interrupter {

public:
    friend TransferTaskManager;

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

    void taskAdded(groupid groupId, const QString &device, int type);

    void taskRemoved(groupid groupId, const QString &device, int type);
};

class AppUtils {
public:
    static bool applyAdapterName(DeviceConnection &connection);

    static void applyDeviceToJSON(QJsonObject &object);

    static AccessDatabase *getDatabase();

    static AccessDatabaseSignaller *getDatabaseSignaller();

    static QSettings &getDefaultSettings();

    static QString getDeviceId();

    static NetworkDevice getLocalDevice();

    static TransferTaskManager *getTransferTaskManager();

    static AccessDatabase *newDatabaseInstance(QObject *parent = nullptr);
};


#endif //TREBLESHOT_APPUTILS_H
