//
// Created by veli on 2/7/19.
//

#pragma once

#include <QtCore/QThread>
#include <QDebug>
#include "Interrupter.h"

class GThread : public QThread, public Interrupter {
Q_OBJECT
    const std::function<void(GThread *)> m_callback;

public:
    explicit GThread(const std::function<void(GThread *)> &function,
                     bool deleteOnFinish = false,
                     QObject *parent = nullptr);

    static GThread *startIndependent(const std::function<void(GThread *)> &function, QObject *parent = nullptr);

signals:

    void statusUpdate(int max, int progress, const QString &text);

public slots:

    void notifyInterrupt();

protected:
    void run() override;
};