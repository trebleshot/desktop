//
// Created by veli on 9/28/18.
//

#ifndef TREBLESHOT_NETWORKDEVICELOADER_H
#define TREBLESHOT_NETWORKDEVICELOADER_H

#include "src/config/Config.h"
#include "src/database/object/NetworkDevice.h"
#include "AppUtils.h"
#include "CommunicationBridge.h"
#include "GThread.h"
#include <QtCore/QJsonObject>
#include <QHostAddress>

class NetworkDeviceLoader {
public:
    static QString convertToInet4Address(int ipv4Address);

    static void loadAsynchronously(QObject *sender,
                                   const QHostAddress &hostAddress,
                                   const std::function<void(NetworkDevice *)> &listener);

    static NetworkDevice *load(QObject *sender, const QHostAddress &hostAddress);

    static NetworkDevice *loadFrom(QJsonObject jsonIndex);

    static DeviceConnection *processConnection(NetworkDevice *device, const QHostAddress &hostAddress);

    static void processConnection(NetworkDevice *device, DeviceConnection *connection);
};


#endif //TREBLESHOT_NETWORKDEVICELOADER_H
