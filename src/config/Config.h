/*
* Copyright (C) 2019 Veli Tasalı
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

#define TS_SERVICE_NAME "TSComm"
#define TS_SERVICE_TYPE "_tscomm._tcp."
#define PORT_COMMUNICATION_DEFAULT 1128
#define PORT_SEAMLESS 58762
#define TIMEOUT_SOCKET_DEFAULT 5000
#define TIMEOUT_SOCKET_DEFAULT_LARGE 40000
#define BUFFER_LENGTH_DEFAULT 8096
#define NICKNAME_LENGTH_MAX 32
#define URI_APP_HOME "https://github.com/genonbeta/TrebleShot-Desktop"

#include "src/build.h"
#include <QApplication>
#include <QString>

typedef quint32 groupid;
typedef quint32 requestid;

QString getApplicationVersion();

unsigned short getApplicationVersionCode();

QString getDeviceId();

QString getUserNickname();

QString getDeviceTypeName();

QString getDeviceNameForOS();

void initAppEnvironment();