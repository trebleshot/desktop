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
#include <ui_ShowTransferDialog.h>
#include <src/database/object/TransferGroup.h>
#include <src/model/TransferObjectModel.h>
#include <src/model/FlawedTransferModel.h>

namespace Ui {
	class ShowTransferDialog;
}

class ShowTransferDialog : public QDialog {
Q_OBJECT

public:
	explicit ShowTransferDialog(QWidget *parentWindow, groupid groupId, bool showDeviceSelector = false);

	~ShowTransferDialog() override;

public slots:

	void addDevOrChangeConnection();

	void assigneeChanged(int index);

	void changeSavePath();

	void connectionChanged(const QString &adapterName);

	void globalTaskAdded(groupid groupId, const QString &deviceId, int type);

	void globalTaskRemoved(groupid groupId, const QString &deviceId, int type);

	void globalTaskStatus(groupid groupId, const QString &deviceId, int type, qint64 completed,
	                      const TransferObject &object);

	void globalTaskError(groupid groupId, const QString &deviceId, int type, int errorType);

	void removeTransfer();

	void checkGroupIntegrity(const SqlSelection &change, ChangeType type);

	void retryReceiving();

	void saveDirectory();

	void sendToDevices(groupid groupId, const QList<NetworkDevice> &devices);

	void showFiles();

	void startTransfer();

	void taskToggle();

	void transferItemActivated(const QModelIndex &modelIndex);

	void updateAssignees();

	void updateButtons();

	void updateStats();

protected:
	Ui::ShowTransferDialog *m_ui;
	TransferObjectModel *m_objectModel;
	FlawedTransferModel *m_errorsModel;
	TransferGroup m_group;
	TransferGroupInfo m_groupInfo;
	bool m_showAddDeviceDialog;

	struct {
		TransferObject object;
		qint64 completedBytes = 0;

		void reset() {
			object = TransferObject();
			completedBytes = 0;
		}
	} m_ongoingTaskInfo;

	void paintEvent(QPaintEvent *event) override;
};
