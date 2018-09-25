#include "communicationserver.h"

CommunicationServer::CommunicationServer(QObject* parent)
    : CoolSocket::Server(QHostAddress::Any, PORT_COMMUNICATION_DEFAULT, parent)
{
}

void CommunicationServer::connected(CoolSocket::ActiveConnection* connection)
{
    connection->setTimeout(3000);

    try {
        CoolSocket::Response* response = connection->receive();
        QJsonObject responseJSON = QJsonDocument::fromJson(QByteArray::fromStdString(response->response->toStdString())).object();
        QJsonObject replyJSON = QJsonObject();

        bool result = false;
        bool shouldContinue = false;

        // Insert id of this device

        QJsonObject deviceInfo;
        QJsonObject appInfo;

        deviceInfo.insert(QString(KEYWORD_DEVICE_INFO_SERIAL), QJsonValue("athingofbeutyisajoyforever"));
        deviceInfo.insert(QString(KEYWORD_DEVICE_INFO_BRAND), QJsonValue("Nicola"));
        deviceInfo.insert(QString(KEYWORD_DEVICE_INFO_MODEL), QJsonValue("Tesla"));
        deviceInfo.insert(QString(KEYWORD_DEVICE_INFO_USER), QJsonValue("Hitchhiker's Guide"));

        appInfo.insert(QString(KEYWORD_APP_INFO_VERSION_CODE), QJsonValue(57));
        appInfo.insert(QString(KEYWORD_APP_INFO_VERSION_NAME), QJsonValue("0.1"));

        replyJSON.insert(QString(KEYWORD_APP_INFO), QJsonValue(appInfo));
        replyJSON.insert(QString(KEYWORD_DEVICE_INFO), QJsonValue(deviceInfo));

        replyJSON.insert(QString(KEYWORD_RESULT), QJsonValue(true));

        connection->reply(QJsonDocument(replyJSON).toJson().toStdString().c_str());

    } catch (...) {
        cout << "What could go so wrong??\n"
             << endl;
    }
}
