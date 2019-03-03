//
// Created by veli on 3/2/19.
//

#include <src/database/object/TransferObject.h>
#include <src/util/CommunicationBridge.h>
#include "TransferRequestProgressDialog.h"

TransferRequestProgressDialog::TransferRequestProgressDialog(QWidget *parent, const groupid &groupId,
                                                             const QList<NetworkDevice> &devices)
        : QDialog(parent), m_ui(new Ui::TransferRequestProgressDialog)
{
    m_ui->setupUi(this);
    m_thread = new GThread([this, groupId, devices](GThread *thread) { task(thread, groupId, devices); }, true);
    m_thread->start();

    connect(m_thread, &GThread::statusUpdate, this, &TransferRequestProgressDialog::statusUpdate);
    connect(this, &QDialog::finished, m_thread, &GThread::notifyInterrupt);
}

void TransferRequestProgressDialog::task(GThread *thread, const groupid &groupId, const QList<NetworkDevice> &devices)
{
    QList<TransferObject> objectList;
    bool successful = false;

    {
        SqlSelection selection;
        selection.setTableName(DB_DIVIS_TRANSFER);
        selection.setWhere(QString("%1 = ?").arg(DB_FIELD_TRANSFER_GROUPID));
        selection.whereArgs << groupId;

        gDbSignal->doSynchronized([&selection, &objectList](AccessDatabase *db) {
            objectList << db->castQuery(selection, TransferObject());
        });
    }

    if (objectList.empty() || devices.empty()) {
        qDebug() << "deviceForAddedFiles << Empty object list" << objectList.size() << devices.size();
        return;
    }

    QList<NetworkDevice> passedDevices;

    for (const NetworkDevice &device : devices) {
        if (thread->interrupted())
            break;

        SqlSelection deviceSelection;
        deviceSelection.setTableName(DB_TABLE_TRANSFER);
        deviceSelection.setWhere(QString("%1 = ? AND %2 = ? AND %3 = ?")
                                         .arg(DB_FIELD_TRANSFER_GROUPID)
                                         .arg(DB_FIELD_TRANSFER_DEVICEID)
                                         .arg(DB_FIELD_TRANSFER_TYPE));
        deviceSelection.whereArgs << groupId << device.id << TransferObject::Type::Outgoing;
        deviceSelection.setLimit(1);

        if (gDbSignal->contains(deviceSelection)) {
            qDebug() << "deviceForAddedFiles << Already contains for" << device.nickname;
            passedDevices << device;
        } else {
            if (gDbSignal->transaction()) {
                int iterator = 0;

                for (auto &object : objectList) {
                    object.deviceId = device.id;

                    gDbSignal->insert(object);
                    emit thread->statusUpdate(objectList.size(), ++iterator, object.friendlyName);
                }

                passedDevices << device;
                gDbSignal->commit();

                if (thread->interrupted())
                    break;
            }
        }
    }

    for (auto &thisDevice : passedDevices) {
        if (thread->interrupted())
            break;

        SqlSelection connectionSelection;
        connectionSelection.setTableName(DB_TABLE_DEVICECONNECTION);
        connectionSelection.setWhere(QString("%1 = ?").arg(DB_FIELD_DEVICES_ID));
        connectionSelection.setOrderBy(DB_FIELD_DEVICECONNECTION_LASTCHECKEDDATE, false);
        connectionSelection.whereArgs << thisDevice.id;

        QList<DeviceConnection> connections;

        gDbSignal->doSynchronized([&connectionSelection, &connections](AccessDatabase *db) {
            connections << db->castQuery(connectionSelection, DeviceConnection());
        });

        if (connections.empty()) {
            qDebug() << "deviceForAddedFiles << No connection for" << thisDevice.nickname;
        } else {
            bool shouldTryNext = true;

            for (const auto &thisConnection : connections) {
                if (thread->interrupted())
                    break;

                CSActiveConnection *connection = nullptr;
                CommunicationBridge bridge(thread);
                bridge.setDevice(thisDevice);

                TransferAssignee assignee(groupId, thisDevice.id, thisConnection.adapterName);
                QJsonObject thisObject{
                        {KEYWORD_REQUEST, KEYWORD_REQUEST_TRANSFER},
                        {KEYWORD_TRANSFER_GROUP_ID, QVariant(groupId).toLongLong()}
                };

                QJsonArray filesIndex;

                for (const auto &object : objectList) {
                    if (thread->interrupted())
                        break;

                    QJsonObject jsonObject{
                            {KEYWORD_INDEX_FILE_NAME,     object.friendlyName},
                            {KEYWORD_INDEX_FILE_SIZE,     QVariant((qulonglong) object.fileSize).toLongLong()},
                            {KEYWORD_TRANSFER_REQUEST_ID, QVariant(object.id).toLongLong()},
                            {KEYWORD_INDEX_FILE_MIME,     object.fileMimeType}
                    };

                    if (object.directory != nullptr)
                        jsonObject.insert(KEYWORD_INDEX_DIRECTORY, object.directory);

                    filesIndex.append(jsonObject);
                }

                thisObject.insert(KEYWORD_FILES_INDEX, filesIndex);

                try {
                    connection = bridge.communicate(thisDevice, thisConnection);
                    shouldTryNext = false;

                    connection->reply(thisObject);

                    {
                        const QJsonObject &thisReply = connection->receive().asJson();

                        if (thisReply.value(KEYWORD_RESULT).toBool(false)) {
                            successful = true;

                            qDebug() << "deviceForAddedFiles << Successful for" << thisDevice.nickname;
                            gDbSignal->publish(assignee);
                        } else
                            qDebug() << "deviceForAddedFiles << Failed for <<" << thisDevice.nickname << thisReply;
                    }
                } catch (...) {
                    // do nothing
                    qDebug() << "deviceForAddedFiles << Error" << thisDevice.nickname << thisConnection.adapterName;
                    qDebug() << "deviceForAddedFiles << Continue ??" << shouldTryNext;
                }

                delete connection;

                if (!shouldTryNext)
                    break;
            }
        }
    }

    if (successful) {
        emit transferReady(groupId);
        accept();
    } else
        reject();
}

void TransferRequestProgressDialog::statusUpdate(int total, int progress, QString statusText)
{
    m_ui->progressBar->setMaximum(total);
    m_ui->progressBar->setValue(progress);
    m_ui->label->setText(statusText);
}
