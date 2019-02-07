#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <src/util/NetworkDeviceLoader.h>
#include "CommunicationServer.h"

CommunicationServer::CommunicationServer(QObject *parent)
        : CoolSocket::Server(QHostAddress::Any, PORT_COMMUNICATION_DEFAULT, parent)
{
}

void CommunicationServer::connected(CoolSocket::ActiveConnection *connection)
{
    connection->setTimeout(3000);

    try {
        CoolSocket::Response *response = connection->receive();
        QJsonObject responseJSON = QJsonDocument::fromJson(QByteArray::fromStdString(response->response->toStdString())).object();
        QJsonObject replyJSON = QJsonObject();

        AppUtils::applyDeviceToJSON(replyJSON);

        bool result = false;
        bool shouldContinue = false;
        QString deviceSerial = nullptr;

        if (responseJSON.contains(KEYWORD_HANDSHAKE_REQUIRED) &&
            responseJSON.value(KEYWORD_HANDSHAKE_REQUIRED).toBool(false)) {
            pushReply(connection, replyJSON, true);

            if (!responseJSON.contains(KEYWORD_HANDSHAKE_ONLY) ||
                !responseJSON.value(KEYWORD_HANDSHAKE_ONLY).toBool(false)) {
                if (responseJSON.contains(KEYWORD_DEVICE_INFO_SERIAL)) {
                    deviceSerial = responseJSON.value(KEYWORD_DEVICE_INFO_SERIAL).toString();
                }

                response = connection->receive();
                responseJSON = QJsonDocument::fromJson(QByteArray::fromStdString(response->response->toStdString()))
                        .object();
            } else {
                return;
            }
        }

        qDebug() << "Handshake sent, continuing to handling the request";

        if (deviceSerial != nullptr) {
            NetworkDevice *device = nullptr;
            auto* existingDevice = new NetworkDevice(deviceSerial);
            auto existingState = false;

            qDebug() << "The device id has been provided";
            gDbSignal->reconstruct(existingDevice, &existingState);

            if (existingState) {
                if (!existingDevice->isRestricted)
                    shouldContinue = true;

                device = existingDevice;
            } else {
                device = NetworkDeviceLoader::load(
                        this,
                        connection->getSocket()->peerAddress().toString());

                device->isTrusted = false;
                device->isRestricted = true;

                shouldContinue = true;

                qDebug() << "Reached device name: " << device->nickname;
                gDbSignal->publish(device);
            }

            if (!shouldContinue) {
                replyJSON.insert(KEYWORD_ERROR, KEYWORD_ERROR_NOT_ALLOWED);
            } else {
                qDebug() << "UP and RUNNING";
            }
        }

        pushReply(connection, replyJSON, result);
    } catch (const exception &e) {
        cout << "An error occurred: "
             << e.what()
             << endl;
    } catch (...) {
        cout << "An unknown error occurred"
             << endl;
    }
}

void CommunicationServer::pushReply(CoolSocket::ActiveConnection *activeConnection,
                                    QJsonObject &json,
                                    bool result)
{
    json.insert(KEYWORD_RESULT, result);
    activeConnection->reply(QJsonDocument(json).toJson().toStdString().c_str());
}