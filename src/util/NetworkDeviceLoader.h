//
// Created by veli on 9/28/18.
//

#ifndef TREBLESHOT_NETWORKDEVICELOADER_H
#define TREBLESHOT_NETWORKDEVICELOADER_H

#include "src/config/Config.h"
#include "src/database/object/NetworkDevice.h"

class NetworkDeviceLoader {
public:
    static DeviceConnection *processConnection(NetworkDevice *device, QString ipAddress);

    static void processConnection(NetworkDevice *device, DeviceConnection *connection);

    static void loadAsynchronously(QObject *sender,
                                   const QString &ipAddress,
                                   const std::function<void(NetworkDevice *)> &listener);

    static NetworkDevice *load(QObject *sender, const QString &ipAddress);

    static NetworkDevice *loadFrom(QJsonObject jsonIndex);
};


#endif //TREBLESHOT_NETWORKDEVICELOADER_H
