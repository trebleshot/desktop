//
// Created by veli on 9/29/18.
//

#ifndef TREBLESHOT_COMMUNICATIONBRIDGE_H
#define TREBLESHOT_COMMUNICATIONBRIDGE_H

#include <src/coolsocket/coolsocket.h>
#include <src/config/config.h>
#include <src/config/keyword.h>
#include <src/database/accessdatabase.h>
#include <src/database/object/networkdevice.h>
#include <src/util/networkdeviceloader.h>

namespace CommunicationBridge {
    class Client;

    class ConnectionHandler;

    Client connect(AccessDatabase *database, ConnectionHandler handler);

    template<typename T>
    Client connect(AccessDatabase *database, T clazz, DeviceConnection targetConnection);

    Client connect(AccessDatabase *database, bool currentThread, ConnectionHandler handler);

    class Client : public CoolSocket::Client {
        AccessDatabase *database;
        NetworkDevice *device;

    public:
        Client(AccessDatabase *database, QObject *parent = nullptr);

        CoolSocket::ActiveConnection *communicate(NetworkDevice *targetDevice, DeviceConnection *targetConnection);

        CoolSocket::ActiveConnection *communicate(CoolSocket::ActiveConnection *connection, NetworkDevice *device);

        CoolSocket::ActiveConnection *connect(QString ipAddress);

        CoolSocket::ActiveConnection *connect(DeviceConnection *connection);

        CoolSocket::ActiveConnection *connectWithHandshake(QString ipAddress, bool handshakeOnly);

        AccessDatabase *getDatabase();

        NetworkDevice &getDevice() const;

        CoolSocket::ActiveConnection *handshake(CoolSocket::ActiveConnection *connection, bool handshakeOnly);

        NetworkDevice *loadDevice(QString ipAddress);

        NetworkDevice *loadDevice(CoolSocket::ActiveConnection *connection);

        void setDevice(const NetworkDevice &device);

        NetworkDevice* updateDeviceIfOkay(CoolSocket::ActiveConnection *connection, NetworkDevice *device);
    };
};


#endif //TREBLESHOT_COMMUNICATIONBRIDGE_H
