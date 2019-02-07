//
// Created by veli on 2/7/19.
//

#ifndef TREBLESHOT_GTHREAD_H
#define TREBLESHOT_GTHREAD_H


#include <QtCore/QThread>

class GThread : public QThread {
Q_OBJECT
    const std::function<void(QThread *)> m_callback;

public:
    explicit GThread(const std::function<void(QThread *)> &function, QObject *parent = nullptr);

protected:
    void run() override;
};


#endif //TREBLESHOT_GTHREAD_H
