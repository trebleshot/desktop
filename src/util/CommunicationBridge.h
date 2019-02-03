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
    NetworkDevice *m_device;
    int m_secureKey = -1;

public:
    explicit CommunicationBridge(QObject *parent = nullptr, NetworkDevice* target = nullptr)
            : CoolSocket::Client(parent), m_device(target)
    {

    }

    CoolSocket::ActiveConnection *communicate(NetworkDevice *targetDevice,
                                              DeviceConnection *targetConnection);

    CoolSocket::ActiveConnection *communicate(CoolSocket::ActiveConnection *connection,
                                              NetworkDevice *device);

    CoolSocket::ActiveConnection *connect(const QString &ipAddress);

    CoolSocket::ActiveConnection *connect(DeviceConnection *connection);

    CoolSocket::ActiveConnection *connectWithHandshake(const QString &ipAddress, bool handshakeOnly);

    NetworkDevice *getDevice();

    CoolSocket::ActiveConnection *handshake(CoolSocket::ActiveConnection *connection,
                                            bool handshakeOnly);

    NetworkDevice *loadDevice(const QString &ipAddress);

    NetworkDevice *loadDevice(CoolSocket::ActiveConnection *connection);

    void setDevice(NetworkDevice *device);

    void setSecureKey(int key);

    NetworkDevice *updateDeviceIfOkay(CoolSocket::ActiveConnection *connection,
                                      NetworkDevice *device);
};


#endif //TREBLESHOT_COMMUNICATIONBRIDGE_H
