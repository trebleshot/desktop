/*
* Copyright (C) 2019 Veli Tasalı, created on 2/16/19
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

#include <QDialog>
#include <QAction>
#include <src/model/NetworkDeviceModel.h>
#include "ui_ManageDevicesDialog.h"

namespace Ui {
    class ManageDevicesDialog;
}

class ManageDevicesDialog : public QDialog {
Q_OBJECT

public:
    explicit ManageDevicesDialog(QWidget *parent);

    ~ManageDevicesDialog() override;

    void itemContextMenu(const QPoint& point);

    void itemSelected(const QModelIndex& modelIndex);

protected:
    Ui::ManageDevicesDialog *m_ui;
    NetworkDeviceModel *m_deviceModel;
};