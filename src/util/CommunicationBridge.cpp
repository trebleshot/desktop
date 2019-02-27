//
// Created by veli on 9/29/18.
//

#include "CommunicationBridge.h"

CSActiveConnection *CommunicationBridge::communicate(NetworkDevice &targetDevice,
                                                     const DeviceConnection &targetConnection)
{
    CSActiveConnection *connection = connectWithHandshake(targetConnection.hostAddress, false);

    communicate(connection, targetDevice);

    return connection;
}

CSActiveConnection *CommunicationBridge::communicate(CSActiveConnection *connection,
                                                     NetworkDevice &device)
{
    updateDeviceIfOkay(connection, device);
    return connection;
}

CSActiveConnection *CommunicationBridge::connect(const QHostAddress &hostAddress)
{
    return CSClient::openConnection(hostAddress, PORT_COMMUNICATION_DEFAULT, TIMEOUT_SOCKET_DEFAULT, this);
}

CSActiveConnection *CommunicationBridge::connect(DeviceConnection *connection)
{
    return connect(connection->hostAddress);
}

CSActiveConnection *CommunicationBridge::connectWithHandshake(const QHostAddress &hostAddress,
                                                              bool handshakeOnly)
{
    return handshake(connect(hostAddress), handshakeOnly);
}

NetworkDevice CommunicationBridge::getDevice()
{
    return this->m_device;
}

CSActiveConnection *CommunicationBridge::handshake(CSActiveConnection *connection,
                                                   bool handshakeOnly)
{
    try {
        QJsonObject replyJSON{
                {KEYWORD_HANDSHAKE_REQUIRED, true},
                {KEYWORD_HANDSHAKE_ONLY,     handshakeOnly},
                {KEYWORD_DEVICE_INFO_SERIAL, getDeviceId()},
                {KEYWORD_DEVICE_SECURE_KEY,  m_device.id == nullptr ? m_secureKey : m_device.tmpSecureKey}
        };

        connection->reply(replyJSON);
    } catch (exception &e) {
        throw exception();
    }

    return connection;
}

NetworkDevice CommunicationBridge::loadDevice(const QHostAddress &hostAddress)
{
    return loadDevice(connectWithHandshake(hostAddress, true));
}

NetworkDevice CommunicationBridge::loadDevice(CSActiveConnection *connection)
{
    try {
        return NetworkDeviceLoader::loadFrom(connection->receive().asJson());
    } catch (exception &e) {
        throw exception();
    }
}

void CommunicationBridge::setDevice(const NetworkDevice &device)
{
    this->m_device = device;
}

void CommunicationBridge::setSecureKey(int key)
{
    this->m_secureKey = key;
}

NetworkDevice CommunicationBridge::updateDeviceIfOkay(CSActiveConnection *activeConnection,
                                                      NetworkDevice &device)
{
    NetworkDevice loadedDevice = loadDevice(activeConnection);

    NetworkDeviceLoader::processConnection(loadedDevice, activeConnection->socket()->peerAddress());

    if (device.id != loadedDevice.id) {
        qDebug() << "Compared" << device.nickname << "with" << loadedDevice.nickname;
        throw exception();
    } else {
        device = loadedDevice;

        time(&loadedDevice.lastUsageTime);
        gDbSignal->publish(loadedDevice);
        setDevice(loadedDevice);
    }

    return loadedDevice;
}
