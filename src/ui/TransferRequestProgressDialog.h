//
// Created by veli on 3/2/19.
//

#ifndef TREBLESHOT_TRANSFERREQUESTPROGRESSDIALOG_H
#define TREBLESHOT_TRANSFERREQUESTPROGRESSDIALOG_H

#include "ui_TransferRequestProgressDialog.h"
#include <QtWidgets/QDialog>
#include <QJsonArray>
#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <src/util/GThread.h>

namespace Ui {
    class TransferRequestProgressDialog;
}

class TransferRequestProgressDialog : public QDialog {
Q_OBJECT

public:
    explicit TransferRequestProgressDialog(QWidget *parent, const groupid &groupId,
                                           const QList<NetworkDevice> &devices);

public slots:

    void statusUpdate(int total, int progress, QString statusText);

signals:

    void transferReady(groupid groupId);

protected:
    Ui::TransferRequestProgressDialog *m_ui;
    GThread *m_thread;

    void task(GThread *thread, const groupid &groupId, const QList<NetworkDevice> &devices);
};


#endif //TREBLESHOT_TRANSFERREQUESTPROGRESSDIALOG_H
