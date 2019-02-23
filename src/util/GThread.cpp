//
// Created by veli on 2/7/19.
//

#include "GThread.h"

GThread::GThread(const std::function<void(GThread *)> &function, QObject *parent)
        : QThread(parent), m_callback(function)
{

}

void GThread::run()
{
    m_callback(this);
}

GThread *GThread::startIndependent(const std::function<void(GThread *)> &function, QObject *parent)
{
    auto *thread = new GThread(function, parent);

    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();

    return thread;
}
