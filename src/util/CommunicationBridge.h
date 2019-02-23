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
    NetworkDevice m_device;
    int m_secureKey = -1;

public:
    explicit CommunicationBridge(QObject *parent = nullptr, const NetworkDevice &target = NetworkDevice())
            : CoolSocket::Client(parent)
    {
        m_device = target;
    }

    CoolSocket::ActiveConnection *communicate(NetworkDevice &targetDevice,
                                              const DeviceConnection &targetConnection);

    CoolSocket::ActiveConnection *communicate(CoolSocket::ActiveConnection *connection,
                                              NetworkDevice &device);

    CoolSocket::ActiveConnection *connect(const QHostAddress &hostAddress);

    CoolSocket::ActiveConnection *connect(DeviceConnection *connection);

    CoolSocket::ActiveConnection *connectWithHandshake(const QHostAddress &hostAddress, bool handshakeOnly);

    NetworkDevice getDevice();

    CoolSocket::ActiveConnection *handshake(CoolSocket::ActiveConnection *connection,
                                            bool handshakeOnly);

    NetworkDevice loadDevice(const QHostAddress &hostAddress);

    NetworkDevice loadDevice(CoolSocket::ActiveConnection *connection);

    void setDevice(const NetworkDevice &device);

    void setSecureKey(int key);

    NetworkDevice updateDeviceIfOkay(CoolSocket::ActiveConnection *connection,
                                     NetworkDevice &device);
};


#endif //TREBLESHOT_COMMUNICATIONBRIDGE_H
