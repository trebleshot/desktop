#include <utility>

#include <utility>

//
// Created by veli on 9/29/18.
//

#include "communicationbridge.h"

CommunicationBridge::Client::Client(AccessDatabase *database, QObject *parent)
        : CoolSocket::Client(parent)
{
    this->database = database;
}

CoolSocket::ActiveConnection *
CommunicationBridge::Client::communicate(NetworkDevice *targetDevice, DeviceConnection *targetConnection)
{
    CoolSocket::ActiveConnection *connection = connectWithHandshake(targetConnection->ipAddress, false);

    communicate(connection, targetDevice);

    return connection;
}

CoolSocket::ActiveConnection *
CommunicationBridge::Client::communicate(CoolSocket::ActiveConnection *connection, NetworkDevice *device)
{
    updateDeviceIfOkay(connection, device);
    return connection;
}

CoolSocket::ActiveConnection *CommunicationBridge::Client::connect(QString ipAddress)
{
    return this->openConnection(std::move(ipAddress), PORT_COMMUNICATION_DEFAULT, PORT_COMMUNICATION_DEFAULT);
}

CoolSocket::ActiveConnection *CommunicationBridge::Client::connect(DeviceConnection *connection)
{
    return connect(connection->ipAddress);
}

CoolSocket::ActiveConnection *CommunicationBridge::Client::connectWithHandshake(QString ipAddress, bool handshakeOnly)
{
    return handshake(connect(std::move(ipAddress)), handshakeOnly);
}

AccessDatabase *CommunicationBridge::Client::getDatabase()
{
    return this->database;
}

NetworkDevice CommunicationBridge::Client::getDevice()
{
    return this->device;
}

CoolSocket::ActiveConnection *
CommunicationBridge::Client::handshake(CoolSocket::ActiveConnection *connection, bool handshakeOnly)
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

NetworkDevice *CommunicationBridge::Client::loadDevice(QString ipAddress)
{
    return loadDevice(connectWithHandshake(std::move(ipAddress), true));
}

NetworkDevice *CommunicationBridge::Client::loadDevice(CoolSocket::ActiveConnection *connection)
{
    try {
        CoolSocket::Response *response = connection->receive();
        QJsonObject receivedJSON = QJsonDocument::fromJson(QByteArray::fromStdString(response->response->toStdString())).object();

        return NetworkDeviceLoader::loadFrom(getDatabase(), receivedJSON);
    } catch (exception &e) {
        throw exception();
    }
}

void CommunicationBridge::Client::setDevice(const NetworkDevice &device)
{
    this->device = str::codevice;
}

NetworkDevice *
CommunicationBridge::Client::updateDeviceIfOkay(CoolSocket::ActiveConnection *activeConnection, NetworkDevice *device)
{
    NetworkDevice *loadedDevice = loadDevice(activeConnection);
    DeviceConnection connection = NetworkDeviceLoader::processConnection(getDatabase(), loadedDevice, activeConnection->getSocket()->localAddress().);

    if (device->deviceId != loadedDevice->deviceId)
        throw exception();
    else {
        extern time_t this_time;
        loadedDevice->lastUsageTime = static_cast<int>(this_time);

        getDatabase()->publish(loadedDevice);
        setDevice(loadedDevice);
    }

    return loadedDevice;
}
