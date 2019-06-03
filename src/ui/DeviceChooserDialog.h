/*
* Copyright (C) 2019 Veli Tasalı, created on 2/24/19
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
#include <QLineEdit>
#include <src/database/object/TransferGroup.h>
#include <src/model/NetworkDeviceModel.h>
#include <src/util/NetworkDeviceLoader.h>

namespace Ui {
	class DeviceChooserDialog;
}

class DeviceChooserDialog : public QDialog {
Q_OBJECT

public:
	explicit DeviceChooserDialog(QWidget *parent = nullptr);

	~DeviceChooserDialog() override;

public slots:

	void deviceLoaded(const NetworkDevice &device);

	void enableAddDeviceViews();

	void ipAddressChanged(const QString &ipAddress);

	void ipAddressReturnPressed();

	void modelActivated(const QModelIndex &modelIndex);

	void modelPressed(const QModelIndex &modelIndex);

	void selectionAccepted();

	void setConfirmButtonState(bool state);

signals:

	void devicesSelected(QList<NetworkDevice> devices);

protected:
	Ui::DeviceChooserDialog *m_ui;
	NetworkDeviceModel *m_deviceModel;
};