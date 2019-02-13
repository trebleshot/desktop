//
// Created by veli on 2/9/19.
//

#ifndef TREBLESHOT_SEAMLESSCLIENT_H
#define TREBLESHOT_SEAMLESSCLIENT_H

#include <QtCore/QThread>
#include <QFile>

class SeamlessClient : public QThread {
    quint32 m_groupId = 0;
    QString m_deviceId;
    bool m_interrupted = false;

public:
    explicit SeamlessClient(const QString &deviceId, quint32 groupId, QObject *parent = nullptr);

    bool interrupted() {
        return m_interrupted;
    }

protected:
    void run() override;

public slots:

    void interrupt();
};


#endif //TREBLESHOT_SEAMLESSCLIENT_H
