//
// Created by veli on 9/29/18.
//

#include "CommunicationBridge.h"

CoolSocket::ActiveConnection *CommunicationBridge::communicate(NetworkDevice &targetDevice,
                                                               const DeviceConnection &targetConnection)
{
    CoolSocket::ActiveConnection *connection = connectWithHandshake(targetConnection.hostAddress, false);

    communicate(connection, targetDevice);

    return connection;
}

CoolSocket::ActiveConnection *CommunicationBridge::communicate(
        CoolSocket::ActiveConnection *connection, NetworkDevice &device)
{
    updateDeviceIfOkay(connection, device);
    return connection;
}

CoolSocket::ActiveConnection *CommunicationBridge::connect(const QHostAddress &hostAddress)
{
    return Client::openConnection(this, hostAddress, PORT_COMMUNICATION_DEFAULT,
                                  PORT_COMMUNICATION_DEFAULT);
}

CoolSocket::ActiveConnection *CommunicationBridge::connect(DeviceConnection *connection)
{
    return connect(connection->hostAddress);
}

CoolSocket::ActiveConnection *CommunicationBridge::connectWithHandshake(const QHostAddress &hostAddress,
                                                                        bool handshakeOnly)
{
    return handshake(connect(hostAddress), handshakeOnly);
}

NetworkDevice CommunicationBridge::getDevice()
{
    return this->m_device;
}

CoolSocket::ActiveConnection *CommunicationBridge::handshake(
        CoolSocket::ActiveConnection *connection, bool handshakeOnly)
{
    try {
        QJsonObject replyJSON;

        replyJSON.insert(KEYWORD_HANDSHAKE_REQUIRED, true);
        replyJSON.insert(KEYWORD_HANDSHAKE_ONLY, handshakeOnly);
        replyJSON.insert(KEYWORD_DEVICE_INFO_SERIAL, getDeviceId());
        replyJSON.insert(KEYWORD_DEVICE_SECURE_KEY, m_device.deviceId == nullptr
                                                    ? m_secureKey
                                                    : m_device.tmpSecureKey);

        connection->reply(QJsonDocument(replyJSON).toJson().toStdString().c_str());
    } catch (exception &e) {
        throw exception();
    }

    return connection;
}

NetworkDevice CommunicationBridge::loadDevice(const QHostAddress &hostAddress)
{
    return loadDevice(connectWithHandshake(hostAddress, true));
}

NetworkDevice CommunicationBridge::loadDevice(CoolSocket::ActiveConnection *connection)
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

NetworkDevice CommunicationBridge::updateDeviceIfOkay(CoolSocket::ActiveConnection *activeConnection,
                                                      NetworkDevice &device)
{
    NetworkDevice loadedDevice = loadDevice(activeConnection);

    NetworkDeviceLoader::processConnection(loadedDevice, activeConnection->getSocket()->peerAddress());

    if (device.deviceId != loadedDevice.deviceId) {
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
