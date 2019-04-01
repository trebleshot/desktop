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

#include <src/util/Interrupter.h>
#include "src/coolsocket/CoolSocket.h"
#include "src/config/Config.h"
#include "src/config/Keyword.h"

class SeamlessServer : public CSServer {
Q_OBJECT

public:
	explicit SeamlessServer(QObject *parent = nullptr);

	void connected(CSActiveConnection *connection) override;

signals:

	void taskStarted(groupid groupId, QString deviceId);

	void taskDone(groupid groupId, QString deviceId);
};