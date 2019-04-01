/*
* Copyright (C) 2019 Veli Tasalı, created on 2/9/19
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

#include <QtCore/QThread>
#include <QFile>
#include <src/util/Interrupter.h>
#include <src/database/object/TransferGroup.h>
#include <src/util/TransferUtils.h>
#include <src/util/AppUtils.h>

class SeamlessClient : public QThread, public TransferTask {
Q_OBJECT
	int m_attemptsLeft = 2;

public:
	explicit SeamlessClient(groupid groupId, const QString &deviceId, bool autoDelete = false,
	                        QObject *parent = nullptr);

signals:

	void taskDone(groupid groupId, QString deviceId);

	void taskFailed(groupid groupId, QString deviceId, Reason reason);

protected:
	void run() override;
};