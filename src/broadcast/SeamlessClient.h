//
// Created by veli on 2/9/19.
//

#ifndef TREBLESHOT_SEAMLESSCLIENT_H
#define TREBLESHOT_SEAMLESSCLIENT_H

#include <QtCore/QThread>
#include <QFile>
#include <src/util/Interrupter.h>

class SeamlessClient : public QThread, public Interrupter {
    quint32 m_groupId = 0;
    QString m_deviceId;

public:
    explicit SeamlessClient(const QString &deviceId, quint32 groupId, QObject *parent = nullptr);

protected:
    void run() override;
};


#endif //TREBLESHOT_SEAMLESSCLIENT_H
