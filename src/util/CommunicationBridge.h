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


#endif //TREBLESHOT_COMMUNICATIONBRIDGE_H
