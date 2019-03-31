/*
* Copyright (C) 2019 Veli Tasalı, created on 3/2/19
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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