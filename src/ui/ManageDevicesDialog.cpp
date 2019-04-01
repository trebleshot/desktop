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

#include <src/model/NetworkDeviceModel.h>
#include <QtWidgets/QMenu>
#include <src/util/AppUtils.h>
#include "ManageDevicesDialog.h"

ManageDevicesDialog::ManageDevicesDialog(QWidget *parent)
		: QDialog(parent), m_ui(new Ui::ManageDevicesDialog), m_deviceModel(new NetworkDeviceModel())
{
	m_ui->setupUi(this);
	m_ui->treeView->setModel(m_deviceModel);
	m_ui->treeView->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

	connect(m_ui->treeView, &QTreeView::customContextMenuRequested, this, &ManageDevicesDialog::itemContextMenu);
	connect(m_ui->treeView, &QTreeView::activated, this, &ManageDevicesDialog::itemSelected);
}

ManageDevicesDialog::~ManageDevicesDialog()
{
	delete m_ui;
	delete m_deviceModel;
}

void ManageDevicesDialog::itemContextMenu(const QPoint &point)
{
	const QModelIndex &modelIndex = m_ui->treeView->indexAt(point);

	if (modelIndex.isValid()) {
		NetworkDevice device = m_deviceModel->list()->at(modelIndex.row());

		QMenu menu(m_ui->treeView);
		auto *actionAccess = new QAction(device.isRestricted ? tr("Allow to access") : tr("Restrict"), &menu);
		auto *actionRemove = new QAction(tr("Remove"), &menu);

		connect(&menu, &QObject::destroyed, actionAccess, &QObject::deleteLater);
		connect(&menu, &QObject::destroyed, actionRemove, &QObject::deleteLater);
		connect(actionAccess, &QAction::triggered, [&device]() {
			device.isRestricted = !device.isRestricted;
			gDatabase->publish(device);
		});
		connect(actionRemove, &QAction::triggered, [&device]() {
			gDatabase->remove(device);
		});

		menu.addAction(actionAccess);
		menu.addAction(actionRemove);

		menu.exec(m_ui->treeView->mapToGlobal(point));
	}
}

void ManageDevicesDialog::itemSelected(const QModelIndex &modelIndex)
{
	if (modelIndex.isValid() && modelIndex.column() == NetworkDeviceModel::Status) {
		NetworkDevice device = m_deviceModel->list()->at(modelIndex.row());
		device.isRestricted = !device.isRestricted;
		gDatabase->publish(device);
	}
}
