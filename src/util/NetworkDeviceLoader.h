/*
* Copyright (C) 2019 Veli Tasalı, created on 9/28/18
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

#include "src/config/Config.h"
#include "src/database/object/NetworkDevice.h"
#include "AppUtils.h"
#include "CommunicationBridge.h"
#include "GThread.h"
#include <QtCore/QJsonObject>
#include <QHostAddress>

class NetworkDeviceLoader {
public:
    static QString convertToInet4Address(const QHostAddress &hostAddress, bool parentOnly = false);

    static QString convertToInet4Address(int ipv4Address, bool parentOnly = false);

    static void loadAsynchronously(const QHostAddress &hostAddress,
                                   const std::function<void(const NetworkDevice &)> &listener);

    static NetworkDevice load(QObject *sender, const QHostAddress &hostAddress);

    static NetworkDevice loadFrom(const QJsonObject &jsonIndex);

    static DeviceConnection processConnection(NetworkDevice &device, const QHostAddress &hostAddress);

    static void processConnection(NetworkDevice &device, DeviceConnection &connection);
};