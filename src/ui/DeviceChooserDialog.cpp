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

#include <ui_DeviceChooserDialog.h>
#include <src/util/ViewUtils.h>
#include "DeviceChooserDialog.h"

DeviceChooserDialog::DeviceChooserDialog(QWidget *parent, groupid groupId)
		: QDialog(parent), m_ui(new Ui::DeviceChooserDialog), m_deviceModel(new NetworkDeviceModel)
{
	m_groupId = groupId;

	m_ui->setupUi(this);
	m_ui->treeView->setModel(m_deviceModel);
	m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);
	m_ui->treeView->setColumnWidth(0, 120);

	connect(m_ui->treeView, &QTreeView::activated, this, &DeviceChooserDialog::ipAddressReturnPressed);
	connect(m_ui->treeView, &QTreeView::pressed, this, &DeviceChooserDialog::modelPressed);
	connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &DeviceChooserDialog::confirmed);
	connect(m_ui->ipAddressLineEdit, &QLineEdit::returnPressed, this, &DeviceChooserDialog::selectionAccepted);
	connect(m_ui->ipAddressLineEdit, &QLineEdit::textChanged, this, &DeviceChooserDialog::ipAddressChanged);
}

DeviceChooserDialog::~DeviceChooserDialog()
{
	delete m_ui;
	delete m_deviceModel;
}

void DeviceChooserDialog::confirmed() {
	if (m_ui->ipAddressLineEdit->text().size() > 0)
		ipAddressReturnPressed();
	else
		selectionAccepted();
}

void DeviceChooserDialog::deviceLoaded(const NetworkDevice &device) {
	QList<NetworkDevice> devices;
	devices << device;
	emit devicesSelected(m_groupId, devices);
}

void DeviceChooserDialog::modelActivated(const QModelIndex &modelIndex)
{
	selectionAccepted();
	accept();
}

void DeviceChooserDialog::ipAddressChanged(const QString &ipAddress) {
	setConfirmButtonState(ipAddress.size() > 0);
}

void DeviceChooserDialog::ipAddressReturnPressed()
{
	QHostAddress hostAddress(m_ui->ipAddressLineEdit->text());
	auto *loaderResult = NetworkDeviceLoader::loadAsynchronously(hostAddress, nullptr);
	connect(loaderResult, &LoaderResult::deviceLoaded, this, &DeviceChooserDialog::deviceLoaded);
}

void DeviceChooserDialog::modelPressed(const QModelIndex &modelIndex)
{
	setConfirmButtonState(modelIndex.isValid());
}

void DeviceChooserDialog::selectionAccepted()
{
	QList<NetworkDevice> devices;

	if (ViewUtils::gatherSelections(m_ui->treeView->selectionModel(), m_deviceModel, devices))
		emit devicesSelected(m_groupId, devices);
}

void DeviceChooserDialog::setConfirmButtonState(bool state) {
	m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(state);
}
