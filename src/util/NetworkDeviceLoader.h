//
// Created by veli on 9/28/18.
//

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