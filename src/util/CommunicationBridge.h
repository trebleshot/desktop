/*
* Copyright (C) 2019 Veli Tasalı, created on 9/29/18
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

#include <src/coolsocket/CoolSocket.h>
#include <src/config/Config.h>
#include <src/config/Keyword.h>
#include <src/database/AccessDatabase.h>
#include <src/database/object/NetworkDevice.h>
#include <src/util/NetworkDeviceLoader.h>

class CommunicationBridge : public CSClient {
    NetworkDevice m_device;
    int m_secureKey = -1;

public:
    explicit CommunicationBridge(QObject *parent = nullptr, const NetworkDevice &target = NetworkDevice())
            : CSClient(parent)
    {
        m_device = target;
    }

    CSActiveConnection *communicate(NetworkDevice &targetDevice,
                                              const DeviceConnection &targetConnection);

    CSActiveConnection *communicate(CSActiveConnection *connection,
                                              NetworkDevice &device);

    CSActiveConnection *connect(const QHostAddress &hostAddress);

    CSActiveConnection *connect(DeviceConnection *connection);

    CSActiveConnection *connectWithHandshake(const QHostAddress &hostAddress, bool handshakeOnly);

    NetworkDevice getDevice();

    CSActiveConnection *handshake(CSActiveConnection *connection,
                                            bool handshakeOnly);

    NetworkDevice loadDevice(const QHostAddress &hostAddress);

    NetworkDevice loadDevice(CSActiveConnection *connection);

    void setDevice(const NetworkDevice &device);

    void setSecureKey(int key);

    NetworkDevice updateDeviceIfOkay(CSActiveConnection *connection,
                                     NetworkDevice &device);
};