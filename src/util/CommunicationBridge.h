//
// Created by veli on 9/29/18.
//

#ifndef TREBLESHOT_COMMUNICATIONBRIDGE_H
#define TREBLESHOT_COMMUNICATIONBRIDGE_H

#include <src/coolsocket/CoolSocket.h>
#include <src/config/Config.h>
#include <src/config/Keyword.h>
#include <src/database/AccessDatabase.h>
#include <src/database/object/NetworkDevice.h>
#include <src/util/NetworkDeviceLoader.h>

class CommunicationBridge : public CoolSocket::Client {
    AccessDatabase *database;
    NetworkDevice *device;

public:
    explicit CommunicationBridge(AccessDatabase *database, QObject *parent = nullptr);

    CoolSocket::ActiveConnection *communicate(NetworkDevice *targetDevice, DeviceConnection *targetConnection);

    CoolSocket::ActiveConnection *communicate(CoolSocket::ActiveConnection *connection, NetworkDevice *device);

    CoolSocket::ActiveConnection *connect(QString ipAddress);

    CoolSocket::ActiveConnection *connect(DeviceConnection *connection);

    CoolSocket::ActiveConnection *connectWithHandshake(QString ipAddress, bool handshakeOnly);

    AccessDatabase *getDatabase();

    NetworkDevice *getDevice();

    CoolSocket::ActiveConnection *handshake(CoolSocket::ActiveConnection *connection, bool handshakeOnly);

    NetworkDevice *loadDevice(QString ipAddress);

    NetworkDevice *loadDevice(CoolSocket::ActiveConnection *connection);

    void setDevice(NetworkDevice *device);

    NetworkDevice *updateDeviceIfOkay(CoolSocket::ActiveConnection *connection, NetworkDevice *device);
};


#endif //TREBLESHOT_COMMUNICATIONBRIDGE_H
