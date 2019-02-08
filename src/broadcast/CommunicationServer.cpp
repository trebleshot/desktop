#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <src/util/NetworkDeviceLoader.h>
#include <QtCore/QJsonArray>
#include <src/util/GThread.h>
#include <src/database/object/TransferGroup.h>
#include <src/database/object/TransferObject.h>
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

        if (responseJSON.contains(KEYWORD_HANDSHAKE_REQUIRED)
            && responseJSON.value(KEYWORD_HANDSHAKE_REQUIRED).toBool(false)) {
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

        if (deviceSerial != nullptr) {
            auto *device = new NetworkDevice(deviceSerial);

            if (gDbSignal->reconstruct(device)) {
                if (!device->isRestricted)
                    shouldContinue = true;
            } else {
                delete device;

                device = NetworkDeviceLoader::load(
                        this,
                        connection->getSocket()->peerAddress().toString());

                device->isTrusted = false;
                device->isRestricted = true;

                shouldContinue = true;

                gDbSignal->publish(device);
            }

            DeviceConnection *deviceConnection =
                    NetworkDeviceLoader::processConnection(device,
                                                           connection->getSocket()->peerAddress().toString());

            if (!shouldContinue) {
                replyJSON.insert(KEYWORD_ERROR, KEYWORD_ERROR_NOT_ALLOWED);
            } else if (responseJSON.contains(KEYWORD_REQUEST)) {
                if (responseJSON.value(KEYWORD_REQUEST) == KEYWORD_REQUEST_TRANSFER) {
                    if (responseJSON.contains(KEYWORD_FILES_INDEX)
                        && responseJSON.contains(KEYWORD_TRANSFER_GROUP_ID)) {
                        const QJsonArray &filesIndex = responseJSON.value(KEYWORD_FILES_INDEX).toArray();
                        const int &groupId = responseJSON.value(KEYWORD_TRANSFER_GROUP_ID).toInt(-1);

                        result = true;

                        GThread::startIndependent([filesIndex, groupId, device, deviceConnection](GThread *thisThread) {
                            auto *transferGroup = new TransferGroup(groupId);
                            auto *transferAssignee =
                                    new TransferAssignee(transferGroup->groupId,
                                                         device->deviceId,
                                                         deviceConnection->adapterName);

                            bool usePublishing = gDbSignal->reconstruct(transferGroup);

                            gDbSignal->publish(transferGroup);
                            gDbSignal->publish(transferAssignee);

                            for (int iterator = 0; iterator < filesIndex.size(); iterator++) {
                                const QJsonObject &transferIndex = filesIndex.at(iterator).toObject();
                                auto *transferObject
                                        = new TransferObject(transferIndex.value(KEYWORD_TRANSFER_REQUEST_ID).toInt(-1),
                                                             device->deviceId,
                                                             TransferObject::Incoming,
                                                             transferGroup);

                                transferObject->groupId = groupId;
                                transferObject->friendlyName = transferIndex.value(KEYWORD_INDEX_FILE_NAME).toString();
                                transferObject->file = QString("%1-%2.tshare")
                                        .arg(transferObject->requestId)
                                        .arg(transferObject->deviceId);
                                transferObject->fileMimeType = transferIndex.value(KEYWORD_INDEX_FILE_MIME).toString();
                                transferObject->fileSize = transferIndex.value(KEYWORD_INDEX_FILE_SIZE).toInt(-1);

                                if (transferIndex.contains(KEYWORD_INDEX_DIRECTORY))
                                    transferObject->directory = transferIndex.value(KEYWORD_INDEX_DIRECTORY).toString();

                                if (usePublishing)
                                    gDbSignal->publish(transferObject);
                                else
                                    gDbSignal->insert(transferObject);

                                delete transferObject;
                            }

                            delete transferGroup;
                            delete transferAssignee;
                        }, this);
                    }
                }
            }
        }

        pushReply(connection, replyJSON, result);
    } catch (const exception &e) {
        qDebug() << "An error occurred: "
                 << e.what()
                 << endl;
    } catch (...) {
        qDebug() << "An unknown error occurred"
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