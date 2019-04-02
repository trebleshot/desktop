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

#include "ConnectionReselectForDeviceDialog.h"

ConnectionReselectForDeviceDialog::ConnectionReselectForDeviceDialog(QWidget *parent, const QString &deviceId)
		: QDialog(parent), m_ui(new Ui::ConnectionReselectForDeviceDialog),
		  m_connectionModel(new ConnectionModel(deviceId))
{
	m_ui->setupUi(this);
	m_ui->connectionTreeView->setModel(m_connectionModel);

	connect(m_ui->defaultButtonBox, &QDialogButtonBox::clicked, this,
	        &ConnectionReselectForDeviceDialog::defaultButtonsClicked);
}

ConnectionReselectForDeviceDialog::~ConnectionReselectForDeviceDialog()
{
	delete m_ui;
	delete m_connectionModel;
}

void ConnectionReselectForDeviceDialog::defaultButtonsClicked(QAbstractButton *button)
{
	if (button == m_ui->defaultButtonBox->button(QDialogButtonBox::StandardButton::Apply)) {
		MutexEnablingScope mutexScope(m_connectionModel);

		auto *model = m_ui->connectionTreeView->selectionModel();
		const auto &selectedItems = ViewUtils::getSelectionRows(model);

		if (selectedItems.empty()) {
			QMessageBox::warning(this, "Nothing selected", "Select one to continue");
		} else {
			int selectedRow = selectedItems[0];

			if (selectedRow < m_connectionModel->list()->size()) {
				emit connectionSelected(m_connectionModel->list()->at(selectedRow).adapterName);
				accept();
			}
		}
	}
}

