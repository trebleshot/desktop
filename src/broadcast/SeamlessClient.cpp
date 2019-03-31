/*
* Copyright (C) 2019 Veli Tasalı, created on 2/9/19
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <src/util/CommunicationBridge.h>
#include <src/util/TransferUtils.h>
#include "SeamlessClient.h"

SeamlessClient::SeamlessClient(groupid groupId, const QString &deviceId, bool autoDelete, QObject *parent)
	: QThread(parent), TransferTask(groupId, deviceId, TransferObject::Type::Incoming)
{
	if (autoDelete)
		connect(this, &QThread::finished, this, &QObject::deleteLater);
}

void SeamlessClient::run()
{
	gTaskMgr->attachTask(this);
	qDebug() << this << "== SeamlessClient ==";

	bool retry = false;
	NetworkDevice device(m_deviceId);
	TransferGroup group(m_groupId);
	TransferAssignee assignee(m_groupId, m_deviceId, nullptr);
	DeviceConnection connection; // Adapter name will be passed when assignee reconstruction is successful
	auto *client = new CommunicationBridge;
	client->moveToThread(this);

	auto connectionLambda = [&connection, &assignee]() -> DeviceConnection & {
		connection.deviceId = assignee.deviceId;
		connection.adapterName = assignee.connectionAdapter;

		return connection;
	};

	if (gDbSignal->reconstruct(device)
		&& gDbSignal->reconstruct(group)
		&& gDbSignal->reconstruct(assignee)
		&& gDbSignal->reconstruct(connectionLambda())) {

		client->setDevice(device);

		try {
			{
				qDebug() << this << "Receive process for" << device.nickname << "for group" << group.id
					<< "with connection"
					<< connection.hostAddress.toString() << "of adapter" << connection.adapterName;

				auto *activeConnection = client->communicate(device, connection);

				qDebug() << this << "Communication port is open";

				QJsonObject initialConnection;
				initialConnection.insert(KEYWORD_REQUEST, KEYWORD_REQUEST_HANDSHAKE);

				activeConnection->reply(initialConnection);
				const auto &resultObject = activeConnection->receive().asJson();

				delete activeConnection;

				qDebug() << this << "Will evaluate the result" << resultObject;

				if (!resultObject.value(KEYWORD_RESULT).toBool(false)) {
					emit taskFailed(m_groupId, m_deviceId, Reason::Rejected);
					throw exception();
				}
			}

			{
				auto *activeConnection = CSClient::openConnection(connection.hostAddress, PORT_SEAMLESS,
					TIMEOUT_SOCKET_DEFAULT, client);

				qDebug() << this << "Seamless port is open";

				QJsonObject groupInfoJson{
						{KEYWORD_TRANSFER_GROUP_ID,  QVariant(m_groupId).toLongLong()},
						{KEYWORD_TRANSFER_DEVICE_ID, AppUtils::getDeviceId()}
				};

				activeConnection->reply(groupInfoJson);
				const auto &request = activeConnection->receive().asJson();

				if (!request.value(KEYWORD_RESULT).toBool(false)) {
					const QString &errorCode = request.value(KEYWORD_ERROR).toString(nullptr);

					qDebug() << this << "Error" << request;
					emit taskFailed(m_groupId, m_deviceId, TransferUtils::getErrorReason(errorCode));

					if (KEYWORD_ERROR_NOT_FOUND == errorCode) {
						gDbSignal->doSynchronized([this](AccessDatabase *database) {
							const auto &sqlSelection = TransferUtils::createSqlSelection(m_groupId, m_deviceId,
								TransferObject::Flag::Done,
								false);

							database->update(sqlSelection, DbObjectMap{
									{DB_FIELD_TRANSFER_FLAG, QVariant(TransferObject::Flag::Removed)}
								});
						});
					}
				}
				else {
					while (activeConnection->socket()->isOpen()) {
						if (interrupted())
							break;

						TransferObject transferObject;

						try {
							bool constStatus = false;

							gDbSignal->doSynchronized([this, &transferObject, &constStatus](AccessDatabase *db) {
								constStatus = TransferUtils::firstAvailableTransfer(transferObject, m_groupId, m_deviceId);
							});

							if (!constStatus) {
								emit taskFailed(m_groupId, m_deviceId, Reason::NoPendingTransfer);
								break;
							}
							else {
								QFile currentFile(TransferUtils::getIncomingFilePath(group, transferObject));
								currentFile.open(QFile::OpenModeFlag::Append);

								QTcpServer tcpServer;
								tcpServer.listen();

								{
									QJsonObject reply{
											{KEYWORD_TRANSFER_REQUEST_ID,  QVariant(transferObject.id).toLongLong()},
											{KEYWORD_TRANSFER_GROUP_ID,    QVariant(transferObject.groupId).toLongLong()},
											{KEYWORD_TRANSFER_SOCKET_PORT, tcpServer.serverPort()},
											{KEYWORD_RESULT,               true}
									};

									if (currentFile.size() > 0) {
										transferObject.skippedBytes = static_cast<size_t>(currentFile.size());
										reply.insert(KEYWORD_SKIPPED_BYTES, currentFile.size());
									}

									activeConnection->reply(reply);
								}

								{
									const auto &fileResponseJSON = activeConnection->receive().asJson();

									if (!fileResponseJSON.value(KEYWORD_RESULT).toBool(false)) {
										if (fileResponseJSON.contains(KEYWORD_TRANSFER_JOB_DONE)
											&& !fileResponseJSON.value(KEYWORD_TRANSFER_JOB_DONE).toBool(false)) {
											interrupt();
											qDebug() << this << "The other side requested the task to be cancelled";
											break;
										}
										else if (fileResponseJSON.contains(KEYWORD_FLAG)
											&& fileResponseJSON.value(KEYWORD_FLAG).toString()
											== KEYWORD_FLAG_GROUP_EXISTS) {
											if (fileResponseJSON.contains(KEYWORD_ERROR)) {
												QString error = fileResponseJSON.value(KEYWORD_ERROR).toString();

												qDebug() << this << "Sender says error" << error << "occurred";

												if (error == KEYWORD_ERROR_NOT_FOUND)
													transferObject.flag = TransferObject::Flag::Removed;
												else if (error == KEYWORD_ERROR_UNKNOWN
													|| error == KEYWORD_ERROR_NOT_ACCESSIBLE)
													transferObject.flag = TransferObject::Flag::Interrupted;
											}
											else {
												transferObject.flag = TransferObject::Flag::Interrupted;
											}
										}
									}
									else {
										bool canContinue = true;

										if (fileResponseJSON.contains(KEYWORD_SIZE_CHANGED)) {
											size_t currentSize = fileResponseJSON.value(KEYWORD_SIZE_CHANGED)
												.toVariant()
												.toUInt();

											if (currentSize != transferObject.fileSize && currentFile.size() > 0) {
												transferObject.fileSize = currentSize;
												transferObject.flag = TransferObject::Flag::Removed;

												canContinue = false;
											}
										}

										if (canContinue) {
											// We do receive the file here ...
											if (tcpServer.waitForNewConnection(TIMEOUT_SOCKET_DEFAULT)) {
												if (tcpServer.hasPendingConnections()) {
													if (transferObject.flag != TransferObject::Flag::Running) {
														transferObject.flag = TransferObject::Flag::Running;
														gDbSignal->update(transferObject);
													}

													auto *socket = tcpServer.nextPendingConnection();
													auto lastDataAvailable = clock();
													auto fileSize = static_cast<qint64>(transferObject.fileSize);
													auto lastUpdate = (clock_t)0;
													auto lastKnownSize = (qint64)0;

													while (socket->isReadable() && currentFile.size() < fileSize) {
														if (socket->waitForReadyRead(2000)) {
															currentFile.write(socket->read(BUFFER_LENGTH_DEFAULT));
															currentFile.flush();

															lastDataAvailable = clock();
														}

														// make sure, when about to complete, notify the last bits
														if (clock() - lastUpdate > 2000
															|| currentFile.size() == fileSize) {
															auto size = currentFile.size();
															emit gTaskMgr->taskByteTransferred(m_groupId, m_deviceId,
																TransferObject::Incoming,
																size - lastKnownSize,
																size);
															lastKnownSize = size;
															lastUpdate = clock();
														}

														if (lastDataAvailable < clock() - TIMEOUT_SOCKET_DEFAULT)
															throw exception();

														if (interrupted())
															break;
													}

													socket->disconnectFromHost();

													if (currentFile.size() == fileSize) {
														currentFile.close();

														gDbSignal->doSynchronized(
															[&group, &transferObject](AccessDatabase *db) {
															TransferUtils::saveIncomingFile(group, transferObject);
														});

														emit gTaskMgr->taskItemTransferred(m_groupId, m_deviceId,
															TransferObject::Incoming);
													}
													else {
														qDebug() << this << "The size did not match";
														transferObject.flag = TransferObject::Flag::Interrupted;
													}
												}
											}
											else {
												qDebug() << this << "The other device did not connect in time";
											}
										}
									}
								}
							}
						}
						catch (...) {
							if (transferObject.id != 0)
								transferObject.flag = TransferObject::Flag::Interrupted;
							qDebug() << this << "Error occurred. Will retry";
							retry = true;
						}

						if (transferObject.id != 0)
							gDbSignal->update(transferObject);
					}

					bool hasLeftFiles = gDbSignal->contains(TransferUtils::createSqlSelection(
						m_groupId, m_deviceId, TransferObject::Flag::Done, false));
					bool isJobDone = !retry && !hasLeftFiles;

					activeConnection->reply({
													{KEYWORD_RESULT,            false},
													{KEYWORD_TRANSFER_JOB_DONE, isJobDone}
						});

					if (isJobDone) {
						emit taskDone(m_groupId, m_deviceId);
						qDebug() << this << "Task done";
					}
				}
			}
		}
		catch (...) {
			qDebug() << this << "Connection failed to the server";
		}

		if (retry && m_attemptsLeft > 0 && !interrupted()) {
			run();
			m_attemptsLeft--;
		}
	}
	else {
		qDebug() << this << "Could not produce information within given group id" << m_groupId
			<< "and device id" << m_deviceId;
	}

	delete client;

	qDebug() << this << "-- SeamlessClient --";
	gTaskMgr->detachTask(this);
}
