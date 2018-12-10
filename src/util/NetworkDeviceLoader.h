//
// Created by veli on 9/28/18.
//

#ifndef TREBLESHOT_NETWORKDEVICELOADER_H
#define TREBLESHOT_NETWORKDEVICELOADER_H


#include <src/database/object/NetworkDevice.h>

class NetworkDeviceLoader {
public:
    static DeviceConnection* processConnection(AccessDatabase* database, NetworkDevice* device, QString ipAddress);

    static void processConnection(AccessDatabase* database, NetworkDevice* device, DeviceConnection *connection);

    static void load(AccessDatabase* database,  QString ipAddress); // there was listener here, we should use a signal instead

    static void load(bool currentThread, AccessDatabase* database, QString ipAddress); // there was listener here, we should use a signal instead

    static NetworkDevice* loadFrom(AccessDatabase* database, QJsonObject jsonIndex);
};


#endif //TREBLESHOT_NETWORKDEVICELOADER_H
