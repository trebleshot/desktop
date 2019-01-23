//
// Created by veli on 9/29/18.
//

#include "CommunicationBridge.h"

CommunicationBridge::CommunicationBridge(AccessDatabase *database, QObject *parent)
        : CoolSocket::Client(parent)
{
    this->database = database;
}

CoolSocket::ActiveConnection *
CommunicationBridge::communicate(NetworkDevice *targetDevice, DeviceConnection *targetConnection)
{
    CoolSocket::ActiveConnection *connection = connectWithHandshake(targetConnection->ipAddress, false);

    communicate(connection, targetDevice);

    return connection;
}

CoolSocket::ActiveConnection *
CommunicationBridge::communicate(CoolSocket::ActiveConnection *connection, NetworkDevice *device)
{
    updateDeviceIfOkay(connection, device);
    return connection;
}

CoolSocket::ActiveConnection *CommunicationBridge::connect(QString ipAddress)
{
    return this->openConnection(std::move(ipAddress), PORT_COMMUNICATION_DEFAULT, PORT_COMMUNICATION_DEFAULT);
}

CoolSocket::ActiveConnection *CommunicationBridge::connect(DeviceConnection *connection)
{
    return connect(connection->ipAddress);
}

CoolSocket::ActiveConnection *CommunicationBridge::connectWithHandshake(QString ipAddress, bool handshakeOnly)
{
    return handshake(connect(std::move(ipAddress)), handshakeOnly);
}

AccessDatabase *CommunicationBridge::getDatabase()
{
    return this->database;
}

NetworkDevice *CommunicationBridge::getDevice()
{
    return this->device;
}

CoolSocket::ActiveConnection *
CommunicationBridge::handshake(CoolSocket::ActiveConnection *connection, bool handshakeOnly)
{
    try {
        QJsonObject replyJSON;

        replyJSON.insert(KEYWORD_HANDSHAKE_REQUIRED, true);
        replyJSON.insert(KEYWORD_HANDSHAKE_ONLY, handshakeOnly);
        replyJSON.insert(KEYWORD_DEVICE_INFO_SERIAL, "THSANTWRKG");
        replyJSON.insert(KEYWORD_DEVICE_SECURE_KEY, -1);

        //fixme: Should add AppUtils that generates device info and serial

        connection->reply(QJsonDocument(replyJSON).toJson().toStdString().c_str());
    } catch (exception &e) {
        throw exception();
    }

    return connection;
}

NetworkDevice *CommunicationBridge::loadDevice(QString ipAddress)
{
    return loadDevice(connectWithHandshake(std::move(ipAddress), true));
}

NetworkDevice *CommunicationBridge::loadDevice(CoolSocket::ActiveConnection *connection)
{
    try {
        CoolSocket::Response *response = connection->receive();
        QJsonObject receivedJSON = QJsonDocument::fromJson(QByteArray::fromStdString(response->response->toStdString())).object();

        return NetworkDeviceLoader::loadFrom(getDatabase(), receivedJSON);
    } catch (exception &e) {
        throw exception();
    }
}

void CommunicationBridge::setDevice(NetworkDevice *device)
{
    this->device = device;
}

NetworkDevice *
CommunicationBridge::updateDeviceIfOkay(CoolSocket::ActiveConnection *activeConnection, NetworkDevice *device)
{
    NetworkDevice *loadedDevice = loadDevice(activeConnection);
    DeviceConnection *connection = NetworkDeviceLoader::processConnection(getDatabase(), loadedDevice, activeConnection
            ->getSocket()
            ->localAddress().toString());

    if (device->deviceId != loadedDevice->deviceId)
        throw exception();
    else {
        time_t this_time;
        loadedDevice->lastUsageTime = static_cast<int>(this_time);

        getDatabase()->publish(loadedDevice);
        setDevice(loadedDevice);
    }

    return loadedDevice;
}
