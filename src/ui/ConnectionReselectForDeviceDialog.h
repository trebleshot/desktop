/*
* Copyright (C) 2019 Veli Tasalı, created on 4/1/19
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

#include <QtWidgets/QDialog>
#include <QPushButton>
#include <QMessageBox>
#include <src/model/ConnectionModel.h>
#include <src/util/ViewUtils.h>
#include "ui_ConnectionReselectForDeviceDialog.h"

namespace Ui {
	class ConnectionReselectForDeviceDialog;
}

class ConnectionReselectForDeviceDialog
		: public QDialog {
Q_OBJECT

public:
	explicit ConnectionReselectForDeviceDialog(QWidget *parent, const QString &deviceId);

	~ConnectionReselectForDeviceDialog() override;

public slots:

	void defaultButtonsClicked(QAbstractButton *button);

signals:

	void connectionSelected(const QString &adapterName);

protected:
	Ui::ConnectionReselectForDeviceDialog *m_ui;
	ConnectionModel *m_connectionModel;
};

