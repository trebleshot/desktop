//
// Created by veli on 2/9/19.
//

#pragma once

#include <QtCore/QThread>
#include <QFile>
#include <src/util/Interrupter.h>
#include <src/database/object/TransferGroup.h>
#include <src/util/TransferUtils.h>
#include <src/util/AppUtils.h>

class SeamlessClient : public QThread, public TransferTask {
Q_OBJECT
    int m_attemptsLeft = 2;

public:
    explicit SeamlessClient(groupid groupId, const QString &deviceId, bool autoDelete = false,
                            QObject *parent = nullptr);

signals:

    void taskDone(groupid groupId, QString deviceId);

    void taskFailed(groupid groupId, QString deviceId, Reason reason);

protected:
    void run() override;
};