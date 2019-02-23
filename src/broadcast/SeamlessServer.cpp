//
// Created by veli on 2/9/19.
//

#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <src/database/object/TransferGroup.h>
#include <src/database/object/TransferObject.h>
#include <QtCore/QFile>
#include "SeamlessServer.h"

SeamlessServer::SeamlessServer(QObject *parent)
        : CoolSocket::Server(QHostAddress::Any, PORT_SEAMLESS, parent)
{

}

void SeamlessServer::connected(CoolSocket::ActiveConnection *connection)
{
    try {
        CoolSocket::Response *mainRequest = connection->receive();
        QJsonObject mainRequestJSON = mainRequest->asJson();
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
            if (device != nullptr && !gDbSignal->reconstruct(*device)) {
                reply.insert(KEYWORD_ERROR, KEYWORD_ERROR_NOT_ALLOWED);
            } else if (!gDbSignal->reconstruct(*group)) {
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

            QJsonObject request = response->asJson();
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
                    auto *transferObject = new TransferObject(
                            request.value(KEYWORD_TRANSFER_REQUEST_ID)
                                    .toVariant()
                                    .toUInt(), deviceId, TransferObject::Type::Outgoing);

                    if (gDbSignal->reconstruct(*transferObject)) {
                        quint16 serverPort = static_cast<quint16>(request.value(KEYWORD_TRANSFER_SOCKET_PORT)
                                .toVariant()
                                .toUInt());

                        //todo: Check whether size_t is unsigned integer or long int
                        size_t skippedBytes = request.contains(KEYWORD_SKIPPED_BYTES)
                                              ? request.value(KEYWORD_SKIPPED_BYTES).toVariant().toUInt()
                                              : 0;

                        gDbSignal->update(*transferObject);

                        auto *file = new QFile(transferObject->file);

                        if (file->exists()) {
                            transferObject->accessPort = serverPort;
                            transferObject->skippedBytes = skippedBytes;

                            reply.insert(KEYWORD_RESULT, true);

                            if (transferObject->fileSize != file->size()) {
                                reply.insert(KEYWORD_SIZE_CHANGED, file->size());
                                //todo: Somehow qt manages to use unsigned values for this.
                                // Check whether it is a good practise to update size types to qint64
                                transferObject->fileSize = file->size();
                            }

                            if (skippedBytes > 0)
                                file->seek(skippedBytes);

                            auto* socket = new QTcpSocket;

                            {
                                // Establish the connection to the socket that will receive the file
                                socket->connectToHost(connection->getSocket()->peerAddress(), serverPort);

                                while (QAbstractSocket::SocketState::ConnectingState == socket->state())
                                    socket->waitForConnected(TIMEOUT_SOCKET_DEFAULT_LARGE);

                                if (QAbstractSocket::SocketState::ConnectedState != socket->state())
                                    throw exception();
                            }

                            clock_t lastDataAvailable = clock();

                            while (file->isReadable()) {
                                if (file->waitForReadyRead(2000)) {
                                    socket->write(file->read(BUFFER_LENGTH_DEFAULT));
                                    socket->flush();

                                    lastDataAvailable = clock();
                                }

                                if (lastDataAvailable < clock() - TIMEOUT_SOCKET_DEFAULT) {
                                    throw exception();
                                }
                            }
                        }
                    } else {
                        reply.insert(KEYWORD_RESULT, false);
                        reply.insert(KEYWORD_ERROR, KEYWORD_ERROR_NOT_FOUND);
                        reply.insert(KEYWORD_FLAG_GROUP_EXISTS, true);

                        transferObject->flag = TransferObject::Flag::Removed;
                    }
                }
            }
        }
    } catch (...) {

    }
}
