//
// Created by veli on 2/9/19.
//

#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <src/util/CommunicationBridge.h>
#include <src/database/object/TransferGroup.h>
#include <src/util/TransferUtils.h>
#include "SeamlessClient.h"

SeamlessClient::SeamlessClient(const QString &deviceId, quint32 groupId, QObject *parent)
        : QThread(parent), m_groupId(groupId), m_deviceId(deviceId)
{
}

void SeamlessClient::run()
{
    qDebug() << "== SeamlessClient ==";

    auto *localDevice = AppUtils::getLocalDevice();
    auto *device = new NetworkDevice(m_deviceId);
    auto *group = new TransferGroup(m_groupId);
    auto *assignee = new TransferAssignee(m_groupId, m_deviceId, nullptr, this);
    auto *connection = new DeviceConnection(); // Adapter name passed when assignee reconstruction is successful
    auto *client = new CommunicationBridge;
    bool retry = false;

    auto connectionLambda = [connection, assignee]() -> DeviceConnection * {
        connection->deviceId = assignee->deviceId;
        connection->adapterName = assignee->connectionAdapter;

        return connection;
    };

    if (gDbSignal->reconstruct(device)
        && gDbSignal->reconstruct(group)
        && gDbSignal->reconstruct(assignee)
        && gDbSignal->reconstruct(connectionLambda())) {

        client->setDevice(device);

        try {
            {
                qDebug() << "Receive process for"
                         << device->nickname
                         << "for group"
                         << group->groupId
                         << "with connection"
                         << connection->hostAddress.toString()
                         << "of adapter"
                         << connection->adapterName;

                auto *activeConnection = client->communicate(device, connection);

                qDebug() << "Communication port is open";

                QJsonObject initialConnection;
                initialConnection.insert(KEYWORD_REQUEST, KEYWORD_REQUEST_HANDSHAKE);

                activeConnection->reply(initialConnection);

                auto *response = activeConnection->receive();
                auto resultObject = response->asJson();

                delete response;

                qDebug() << "Will evaluate the result" << resultObject;

                if (!resultObject.value(KEYWORD_RESULT).toBool(false))
                    throw exception();

                delete activeConnection;
            }

            {
                auto *activeConnection = CoolSocket::Client::openConnection(this, connection->hostAddress,
                                                                            PORT_SEAMLESS, TIMEOUT_SOCKET_DEFAULT);

                qDebug() << "Seamless port is open";

                QJsonObject groupInfoJson;

                groupInfoJson.insert(KEYWORD_TRANSFER_GROUP_ID, QVariant(m_groupId).toString());
                groupInfoJson.insert(KEYWORD_TRANSFER_DEVICE_ID, AppUtils::getDeviceId());

                activeConnection->reply(groupInfoJson);
                auto *response = activeConnection->receive();
                auto request = response->asJson();

                delete response;

                if (!request.value(KEYWORD_RESULT).toBool(false)) {
                    QString errorCode = request.value(KEYWORD_ERROR).toString(nullptr);

                    qDebug() << "Error" << request;

                    if (KEYWORD_ERROR_NOT_FOUND == errorCode) {
                        gDbSignal->doSynchronized([this](AccessDatabase *database) {
                            auto *sqlSelection = TransferUtils::createSqlSelection(m_groupId, m_deviceId,
                                                                                   TransferObject::Flag::Done, false);
                            QSqlRecord record = DbStructure::gatherTableModel(
                                    database, DbStructure::TABLE_TRANSFER)->record();

                            record.setValue(DbStructure::FIELD_TRANSFER_FLAG, QVariant(TransferObject::Flag::Removed));

                            gDbSignal->update(sqlSelection, record);
                        });
                    }
                } else {
                    while (activeConnection->getSocket()->isOpen()) {
                        if (interrupted())
                            break;

                        TransferObject *transferObject = new TransferObject;
                        bool constStatus = false;

                        gDbSignal->doSynchronized([this, transferObject, &constStatus](AccessDatabase *db) {
                            constStatus = TransferUtils::firstAvailableTransfer(transferObject, m_groupId, m_deviceId);
                        });

                        if (constStatus) {
                            QFile currentFile(TransferUtils::getIncomingFilePath(group, transferObject));
                            currentFile.open(QFile::OpenModeFlag::WriteOnly);

                            auto *tcpServer = new QTcpServer;

                            tcpServer->listen();

                            {
                                QJsonObject reply;

                                reply.insert(KEYWORD_TRANSFER_REQUEST_ID,
                                             QVariant(transferObject->requestId).toString());
                                reply.insert(KEYWORD_TRANSFER_GROUP_ID, QVariant(transferObject->groupId).toString());
                                reply.insert(KEYWORD_TRANSFER_SOCKET_PORT, tcpServer->serverPort());
                                reply.insert(KEYWORD_RESULT, true);

                                if (currentFile.size() > 0) {
                                    transferObject->skippedBytes = static_cast<size_t>(currentFile.size());
                                    reply.insert(KEYWORD_SKIPPED_BYTES, currentFile.size());
                                }

                                activeConnection->reply(reply);
                            }

                            {
                                auto *fileResponse = activeConnection->receive();
                                auto fileResponseJSON = fileResponse->asJson();

                                delete fileResponse;

                                if (!fileResponseJSON.value(KEYWORD_RESULT).toBool(false)) {
                                    if (fileResponseJSON.contains(KEYWORD_TRANSFER_JOB_DONE)
                                        && !fileResponseJSON.value(KEYWORD_TRANSFER_JOB_DONE).toBool(false)) {
                                        interrupt();
                                        qDebug() << "The other side requested the task to be cancelled";
                                        break;
                                    } else if (fileResponseJSON.contains(KEYWORD_FLAG)
                                               && fileResponseJSON.value(KEYWORD_FLAG).toString()
                                                  == KEYWORD_FLAG_GROUP_EXISTS) {
                                        if (fileResponseJSON.contains(KEYWORD_ERROR)) {
                                            QString error = fileResponseJSON.value(KEYWORD_ERROR).toString();

                                            qDebug() << "Sender says error" << error << "occurred";

                                            if (error == KEYWORD_ERROR_NOT_FOUND)
                                                transferObject->flag = TransferObject::Flag::Removed;
                                            else if (error == KEYWORD_ERROR_UNKNOWN
                                                     || error == KEYWORD_ERROR_NOT_ACCESSIBLE)
                                                transferObject->flag = TransferObject::Flag::Interrupted;
                                        } else {
                                            transferObject->flag = TransferObject::Flag::Interrupted;
                                        }
                                    }
                                } else {
                                    bool canContinue = true;

                                    if (fileResponseJSON.contains(KEYWORD_SIZE_CHANGED)) {
                                        size_t currentSize = fileResponseJSON.value(KEYWORD_SIZE_CHANGED)
                                                .toVariant()
                                                .toUInt();

                                        if (currentSize != transferObject->fileSize
                                            && currentFile.size() > 0) {
                                            transferObject->fileSize = currentSize;
                                            transferObject->flag = TransferObject::Flag::Removed;

                                            canContinue = false;
                                        }
                                    }

                                    if (canContinue) {
                                        // We do receive the file here ...
                                        tcpServer->waitForNewConnection(TIMEOUT_SOCKET_DEFAULT);

                                        if (tcpServer->hasPendingConnections()) {
                                            auto *socket = tcpServer->nextPendingConnection();
                                            clock_t lastDataAvailable = clock();

                                            while (socket->isReadable()) {
                                                if (socket->waitForReadyRead(2000)) {
                                                    currentFile.write(socket->read(BUFFER_LENGTH_DEFAULT));
                                                    currentFile.flush();

                                                    lastDataAvailable = clock();
                                                }

                                                if (lastDataAvailable < clock() - TIMEOUT_SOCKET_DEFAULT) {
                                                    throw exception();
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        } else {
                            delete transferObject;
                            break;
                        }
                    }
                }
            }
        } catch (...) {
            qDebug() << "Connection failed to the server";
        }
    } else {
        qDebug() << "Could not produce information within given group id"
                 << m_groupId
                 << "and device id"
                 << m_deviceId;
    }

    delete client;
    delete connection;
    delete assignee;
    delete group;
    delete device;
    delete localDevice;

    qDebug() << "-- SeamlessClient --";
}

void SeamlessClient::interrupt()
{
    m_interrupted = true;
}
