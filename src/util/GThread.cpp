//
// Created by veli on 2/7/19.
//

#include "GThread.h"

GThread::GThread(const std::function<void(QThread*)> &function, QObject *parent)
        : QThread(parent), m_callback(function)
{

}

void GThread::run()
{
    m_callback(this);
}
