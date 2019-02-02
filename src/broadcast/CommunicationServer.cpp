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

        bool result = false;
        bool shouldContinue = false;

        // Insert id of this device
        QJsonObject deviceInfo;
        QJsonObject appInfo;

        deviceInfo.insert(KEYWORD_DEVICE_INFO_SERIAL, getDeviceId());
        deviceInfo.insert(KEYWORD_DEVICE_INFO_BRAND, getDeviceTypeName());
        deviceInfo.insert(KEYWORD_DEVICE_INFO_MODEL, getDeviceNameForOS());
        deviceInfo.insert(KEYWORD_DEVICE_INFO_USER, getUserNickname());

        appInfo.insert(KEYWORD_APP_INFO_VERSION_CODE, getApplicationVersionCode());
        appInfo.insert(KEYWORD_APP_INFO_VERSION_NAME, getApplicationVersion());

        replyJSON.insert(KEYWORD_APP_INFO, appInfo);
        replyJSON.insert(KEYWORD_DEVICE_INFO, deviceInfo);

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
                responseJSON = QJsonDocument::fromJson(QByteArray::fromStdString(response->response->toStdString())).object();
            } else {
                return;
            }
        }

        if (deviceSerial != nullptr) {

        }

        pushReply(connection, replyJSON, true);
    } catch (...) {
        cout << "What could go so wrong??"
             << endl;
    }
}

void CommunicationServer::pushReply(CoolSocket::ActiveConnection *activeConnection, QJsonObject &json, bool result)
{
    json.insert(KEYWORD_RESULT, result);
    activeConnection->reply(QJsonDocument(json).toJson().toStdString().c_str());
}
