//
// Created by veli on 2/9/19.
//

#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <src/database/object/TransferGroup.h>
#include <src/database/object/TransferObject.h>
#include "SeamlessServer.h"

SeamlessServer::SeamlessServer(QObject *parent)
        : CoolSocket::Server(QHostAddress::Any, PORT_SEAMLESS, parent)
{

}

void SeamlessServer::connected(CoolSocket::ActiveConnection *connection)
{
    try {
        CoolSocket::Response *mainRequest = connection->receive();
        QJsonObject mainRequestJSON = QJsonDocument::fromJson(
                QByteArray::fromStdString(mainRequest->response->toStdString()))
                .object();
        QString deviceId = mainRequestJSON.contains(KEYWORD_TRANSFER_DEVICE_ID)
                           ? mainRequestJSON.value(KEYWORD_TRANSFER_DEVICE_ID).toString()
                           : nullptr;
        quint32 groupId = mainRequestJSON.value(KEYWORD_TRANSFER_GROUP_ID)
                .toVariant()
                .toUInt();

        {
            QJsonObject reply;

            reply.insert(KEYWORD_RESULT, false);

            auto *device = deviceId != nullptr ? new NetworkDevice(deviceId) : nullptr;
            auto *group = new TransferGroup(groupId);

            // device might be null because this was introduced in build 91
            if (device != nullptr && !gDbSignal->reconstruct(device)) {
                reply.insert(KEYWORD_ERROR, KEYWORD_ERROR_NOT_ALLOWED);
            } else if (!gDbSignal->reconstruct(group)) {
                reply.insert(KEYWORD_ERROR, KEYWORD_ERROR_NOT_FOUND);
            } else {
                reply.insert(KEYWORD_RESULT, true);
            }

            connection->reply(reply);

            if (!reply.value(KEYWORD_RESULT).toBool(false))
                return;
        }

        while (connection->getSocket()->isOpen()) {
            CoolSocket::Response *response = connection->receive();

            if (response->response == nullptr
                || response->length <= 0)
                return;

            QJsonObject request = QJsonDocument::fromJson(
                    QByteArray::fromStdString(response->response->toStdString()))
                    .object();

            QJsonObject reply;

            {
                if (request.contains(KEYWORD_RESULT) && !request.value(KEYWORD_RESULT).toBool(false)) {
                    if (request.contains(KEYWORD_TRANSFER_JOB_DONE)
                        && request.value(KEYWORD_TRANSFER_JOB_DONE).toBool(false)) {

                    } else {
                        //todo: request interrupt
                        // processHolder.builder.getTransferProgress().interrupt();
                    }
                } else {
                    //todo: Previous else statement should be an else if statement similar to the condition below
                    // !processHolder.builder.getTransferProgress().isInterrupted()
                    auto* transferObject = new TransferObject(request.value(KEYWORD_TRANSFER_REQUEST_ID)
                            .toVariant()
                            .toUInt(), deviceId, TransferObject::Type::Outgoing);

                }
            }
        }
    } catch (...) {

    }
}
