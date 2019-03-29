//
// Created by veli on 3/2/19.
//

#pragma once

#include "ui_TransferRequestProgressDialog.h"
#include <QtWidgets/QDialog>
#include <QJsonArray>
#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <src/util/GThread.h>
#include <QPushButton>

namespace Ui {
    class TransferRequestProgressDialog;
}

class TransferRequestProgressDialog : public QDialog {
Q_OBJECT

public:
    explicit TransferRequestProgressDialog(QWidget *parent, const groupid &groupId,
                                           const QList<NetworkDevice> &devices, bool signalOnSuccess = true);

public slots:

    void showError(const groupid &groupId,
                   const QList<NetworkDevice> &devices);

    void statusUpdate(int total, int progress, QString statusText);


signals:

    void errorOccurred(groupid groupId, const QList<NetworkDevice> &devices);

    void transferReady(groupid groupId);

protected:
    Ui::TransferRequestProgressDialog *m_ui;
    GThread *m_thread;
    bool m_signalOnSuccess;

    void task(GThread *thread, const groupid &groupId, const QList<NetworkDevice> &devices);
};