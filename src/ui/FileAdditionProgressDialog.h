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

#include <ui_FileAdditionProgressDialog.h>
#include <QtWidgets/QDialog>

namespace Ui {
	class FileAdditionProgressDialog;
}

class FileAdditionProgressDialog : public QDialog {
Q_OBJECT

public:
	explicit FileAdditionProgressDialog(QWidget *parent, const QList<QString> &files);

	~FileAdditionProgressDialog() override;

	void task(GThread *thread, const QList<QString> &files);

public slots:

	void taskProgress(int max, int progress, const QString &text);

signals:

	void filesAdded(groupid groupId);

protected:
	Ui::FileAdditionProgressDialog *m_ui;
	GThread *m_thread;
};