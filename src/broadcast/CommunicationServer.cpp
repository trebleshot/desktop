#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <src/util/NetworkDeviceLoader.h>
#include <QtCore/QJsonArray>
#include <src/database/object/TransferGroup.h>
#include <src/database/object/TransferObject.h>
#include <src/database/object/TextStreamObject.h>
#include "CommunicationServer.h"

CommunicationServer::CommunicationServer(QObject *parent)
        : CoolSocket::Server(QHostAddress::Any, PORT_COMMUNICATION_DEFAULT, parent)
{
}

void CommunicationServer::connected(CoolSocket::ActiveConnection *connection)
{
    connection->setTimeout(3000);
    NetworkDevice device;

    try {
        auto response = connection->receive();
        auto responseJSON = response.asJson();
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
                responseJSON = response.asJson();
            } else {
                return;
            }
        }

        if (deviceSerial != nullptr) {
            device.deviceId = deviceSerial;

            if (gDbSignal->reconstruct(device)) {
                if (!device.isRestricted)
                    shouldContinue = true;
            } else {
                device = NetworkDeviceLoader::load(
                        this,
                        connection->getSocket()->peerAddress());

                device.isTrusted = false;
                device.isRestricted = true;

                shouldContinue = true;

                gDbSignal->publish(device);
            }

            const DeviceConnection &deviceConnection =
                    NetworkDeviceLoader::processConnection(device, connection->getSocket()->peerAddress());

            if (!shouldContinue) {
                replyJSON.insert(KEYWORD_ERROR, KEYWORD_ERROR_NOT_ALLOWED);
            } else if (responseJSON.contains(KEYWORD_REQUEST)) {
                const QJsonValue &request = responseJSON.value(KEYWORD_REQUEST);

                if (request == KEYWORD_REQUEST_TRANSFER) {
                    if (responseJSON.contains(KEYWORD_FILES_INDEX)
                        && responseJSON.contains(KEYWORD_TRANSFER_GROUP_ID)) {
                        const groupid &groupId = responseJSON.value(KEYWORD_TRANSFER_GROUP_ID)
                                .toVariant()
                                .toUInt();

                        const QJsonArray &filesIndex
                                = QJsonDocument::fromJson(
                                        QByteArray::fromStdString(responseJSON.value(KEYWORD_FILES_INDEX)
                                                                          .toString()
                                                                          .toStdString())).array();

                        result = true;

                        GThread::startIndependent([this, filesIndex, groupId, device, deviceConnection](
                                GThread *thisThread) {
                            TransferGroup transferGroup(groupId);
                            TransferAssignee transferAssignee(transferGroup.id,
                                                              device.deviceId,
                                                              deviceConnection.adapterName);

                            bool usePublishing = gDbSignal->reconstruct(transferGroup);

                            time(&transferGroup.dateCreated);

                            if ()
                            gDbSignal->publish(transferGroup);
                            gDbSignal->publish(transferAssignee);

                            int filesTotal = 0;

                            for (const auto &iterator : filesIndex) {
                                const QJsonObject &transferIndex = iterator.toObject();
                                auto *transferObject
                                        = new TransferObject(transferIndex.value(KEYWORD_TRANSFER_REQUEST_ID)
                                                                     .toVariant()
                                                                     .toUInt(),
                                                             device.deviceId,
                                                             TransferObject::Incoming);

                                transferObject->flag = TransferObject::Flag::Pending;
                                transferObject->groupId = groupId;
                                transferObject->friendlyName = transferIndex.value(KEYWORD_INDEX_FILE_NAME).toString();
                                transferObject->file = QString("%1-%2.tshare")
                                        .arg(transferObject->id)
                                        .arg(transferObject->deviceId);
                                transferObject->fileMimeType = transferIndex.value(KEYWORD_INDEX_FILE_MIME).toString();
                                transferObject->fileSize = transferIndex.value(KEYWORD_INDEX_FILE_SIZE)
                                        .toVariant()
                                        .toUInt();

                                if (transferIndex.contains(KEYWORD_INDEX_DIRECTORY))
                                    transferObject->directory = transferIndex.value(KEYWORD_INDEX_DIRECTORY).toString();

                                if (usePublishing ? gDbSignal->publish(*transferObject)
                                                  : gDbSignal->insert(*transferObject))
                                    filesTotal++;

                                delete transferObject;
                            }

                            if (filesTotal > 0)
                                    emit transferRequest(device.deviceId, transferGroup.id, filesTotal);
                        }, this);
                    }
                } else if (request == KEYWORD_REQUEST_RESPONSE) {
                    if (responseJSON.contains(KEYWORD_TRANSFER_GROUP_ID)) {
                        const groupid &groupId = responseJSON.value(KEYWORD_TRANSFER_GROUP_ID)
                                .toVariant()
                                .toUInt();
                        const bool isAccepted = responseJSON.value(KEYWORD_TRANSFER_IS_ACCEPTED).toBool(false);

                        TransferGroup transferGroup(groupId);
                        TransferAssignee transferAssignee(groupId, device.deviceId);

                        if (gDbSignal->reconstruct(transferGroup)
                            && gDbSignal->reconstruct(transferAssignee)) {

                            if (!isAccepted)
                                gDbSignal->remove(transferGroup);

                            result = true;
                        }
                    }
                } else if (request == KEYWORD_REQUEST_CLIPBOARD) {
                    if (responseJSON.contains(KEYWORD_TRANSFER_CLIPBOARD_TEXT)) {
                        auto text = responseJSON
                                .value(KEYWORD_TRANSFER_CLIPBOARD_TEXT)
                                .toString();
                        emit textReceived(text, device.deviceId);

                        auto *textObject = new TextStreamObject;
                        textObject->text = text;
                        textObject->id = qrand();
                        time(&textObject->dateCreated);

                        gDbSignal->publish(*textObject);
                        delete textObject;

                        result = true;
                    }
                } else if (request == KEYWORD_REQUEST_ACQUAINTANCE) {
                    result = true;
                } else if (request == KEYWORD_REQUEST_HANDSHAKE) {
                    result = true;
                }
            }
        }

        pushReply(connection, replyJSON, result);
    } catch (const exception &e) {
        qDebug() << "An error occurred:"
                 << e.what();
    } catch (...) {
        qDebug() << "An unknown error occurred";
    }
}

void CommunicationServer::pushReply(CoolSocket::ActiveConnection *activeConnection,
                                    QJsonObject &json,
                                    bool result)
{
    json.insert(KEYWORD_RESULT, result);
    activeConnection->reply(json);
}