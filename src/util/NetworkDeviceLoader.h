//
// Created by veli on 9/28/18.
//

#ifndef TREBLESHOT_NETWORKDEVICELOADER_H
#define TREBLESHOT_NETWORKDEVICELOADER_H

#include "src/database/object/NetworkDevice.h"

class NetworkDeviceLoader {
public:
    static DeviceConnection* processConnection(NetworkDevice* device, QString ipAddress);

    static void processConnection(NetworkDevice* device, DeviceConnection *connection);

    static void load(QString &ipAddress); // there was listener here, we should use a signal instead

    static void load(bool currentThread, QString ipAddress); // there was listener here, we should use a signal instead

    static NetworkDevice* loadFrom(QJsonObject jsonIndex);
};


#endif //TREBLESHOT_NETWORKDEVICELOADER_H
