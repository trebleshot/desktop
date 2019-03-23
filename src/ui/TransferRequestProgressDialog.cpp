//
// Created by veli on 3/2/19.
//

#include <src/database/object/TransferObject.h>
#include <src/util/CommunicationBridge.h>
#include <QtWidgets/QMessageBox>
#include "TransferRequestProgressDialog.h"

TransferRequestProgressDialog::TransferRequestProgressDialog(QWidget *parent, const groupid &groupId,
                                                             const QList<NetworkDevice> &devices,
                                                             bool signalOnSuccess)
        : QDialog(parent), m_ui(new Ui::TransferRequestProgressDialog)
{
    m_ui->setupUi(this);
    m_signalOnSuccess = signalOnSuccess;
    m_thread = new GThread([this, groupId, devices](GThread *thread) { task(thread, groupId, devices); }, true);
    m_thread->start();

    connect(m_thread, &GThread::statusUpdate, this, &TransferRequestProgressDialog::statusUpdate);
    connect(this, &QDialog::finished, m_thread, &GThread::notifyInterrupt);
    connect(this, &TransferRequestProgressDialog::errorOccurred, this, &TransferRequestProgressDialog::showError);
}

void TransferRequestProgressDialog::task(GThread *thread, const groupid &groupId, const QList<NetworkDevice> &devices)
{
    QList<TransferObject> objectList;
    bool addedAny = false;

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

    QList<NetworkDevice> failedDevices;
    QList<NetworkDevice> passedDevices;

    for (auto &thisDevice : QList<NetworkDevice>(devices)) {
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
            bool successful = false;

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
                            addedAny = true;
                            successful = true;
                            passedDevices << thisDevice;

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

            if (!successful)
                failedDevices << thisDevice;
        }
    }

    if (!failedDevices.isEmpty())
            emit errorOccurred(groupId, failedDevices);

    if (addedAny) {
        for (const NetworkDevice &device : passedDevices) {
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

        if (m_signalOnSuccess)
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

void TransferRequestProgressDialog::showError(const groupid &groupId, const QList<NetworkDevice> &devices)
{
    auto *dialog = new QMessageBox;
    QString devicesString;

    for (const auto &device : devices) {
        if (devicesString.size() > 0)
            devicesString.append("\n");

        devicesString.append(device.nickname);
    }

    connect(dialog, &QDialog::finished, dialog, &QObject::deleteLater);
    dialog->setWindowTitle("Connection Error");
    dialog->setText(QString("Failed to connect to the devices below:\n\n%1").arg(devicesString));
    dialog->addButton(QMessageBox::StandardButton::Close);
    QPushButton* retryButton = dialog->addButton(QMessageBox::StandardButton::Retry);
    dialog->show();

    connect(retryButton, &QPushButton::pressed, [groupId, devices, dialog](){
        dialog->close();

        auto* progressDialog = new TransferRequestProgressDialog(nullptr, groupId, devices, false);
        connect(progressDialog, &QDialog::finished, progressDialog, &QObject::deleteLater);
        progressDialog->show();
    });
}
