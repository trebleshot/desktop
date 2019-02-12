//
// Created by veli on 2/9/19.
//

#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <src/util/CommunicationBridge.h>
#include <src/database/object/TransferGroup.h>
#include <src/util/TransferUtils.h>
#include "SeamlessClient.h"

typedef const QSqlRecord &sqlRecord;

SeamlessClient::SeamlessClient(const QString &deviceId, quint32 groupId, QObject *parent)
        : QThread(parent), m_groupId(groupId), m_deviceId(deviceId)
{
}

void SeamlessClient::run()
{
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
                auto *activeConnection = client->communicate(device, connection);

                QJsonObject initialConnection;
                initialConnection.insert(KEYWORD_REQUEST, KEYWORD_REQUEST_HANDSHAKE);

                activeConnection->reply(initialConnection);

                auto *response = activeConnection->receive();
                auto resultObject = response->asJson();

                delete response;

                qDebug() << "Will evaluate the result";

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
                groupInfoJson.insert(KEYWORD_TRANSFER_DEVICE_ID, m_deviceId);

                activeConnection->reply(groupInfoJson);
                auto *response = activeConnection->receive();
                const QJsonObject &request = response->asJson();

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
}
