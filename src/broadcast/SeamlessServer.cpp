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
#include <src/database/object/TransferGroup.h>
#include <src/database/object/TransferObject.h>
#include <QtCore/QFile>
#include <src/util/AppUtils.h>

SeamlessServer::SeamlessServer(QObject *parent)
		: CSServer(QHostAddress::Any, PORT_SEAMLESS, TIMEOUT_SOCKET_DEFAULT, parent)
{

}

void SeamlessServer::connected(CSActiveConnection *connection)
{
	TransferTask *thisTask = nullptr;

	try {
		const auto &mainRequest = connection->receive();
		const auto &mainRequestJSON = mainRequest.asJson();
		const QString &deviceId = mainRequestJSON.contains(KEYWORD_TRANSFER_DEVICE_ID)
		                          ? mainRequestJSON.value(KEYWORD_TRANSFER_DEVICE_ID).toString()
		                          : nullptr;
		groupid groupId = mainRequestJSON.value(KEYWORD_TRANSFER_GROUP_ID).toVariant().toUInt();

		thisTask = new TransferTask(groupId, deviceId, TransferObject::Type::Outgoing);
		gTaskMgr->attachTask(thisTask);

		{
			QJsonObject reply;

			reply.insert(KEYWORD_RESULT, false);

			// device might be null because this was introduced in build 91
			if (deviceId != nullptr && !gDbSignal->contains(NetworkDevice(deviceId))) {
				reply.insert(KEYWORD_ERROR, KEYWORD_ERROR_NOT_ALLOWED);
			} else if (!gDbSignal->contains(TransferGroup(groupId))) {
				reply.insert(KEYWORD_ERROR, KEYWORD_ERROR_NOT_FOUND);
			} else {
				reply.insert(KEYWORD_RESULT, true);
			}

			connection->reply(reply);

			if (!reply.value(KEYWORD_RESULT).toBool(false))
				return;
		}

		emit taskStarted(groupId, deviceId);

		while (connection->socket()->isOpen()) {
			const auto &response = connection->receive();

			if (response.response == nullptr || response.length <= 0)
				return;

			const QJsonObject &request = response.asJson();
			QJsonObject reply;

			qDebug() << this << request;

			{
				if (request.contains(KEYWORD_RESULT) && !request.value(KEYWORD_RESULT).toBool(false)) {
					if (request.contains(KEYWORD_TRANSFER_JOB_DONE)
					    && request.value(KEYWORD_TRANSFER_JOB_DONE).toBool(false)) {
						qDebug() << this << "Receiver notified that the transfer is done";
						emit taskDone(groupId, deviceId);
						break;
					} else
						thisTask->interrupt();
				} else if (!thisTask->interrupted()) {
					qDebug() << this << "Entering sending phase";

					TransferObject transferObject(request.value(KEYWORD_TRANSFER_REQUEST_ID).toVariant().toUInt(),
					                              deviceId, TransferObject::Type::Outgoing);

					if (gDbSignal->reconstruct(transferObject)) {
						try {
							quint16 serverPort = static_cast<quint16>(request.value(KEYWORD_TRANSFER_SOCKET_PORT)
									.toVariant()
									.toUInt());
							size_t skippedBytes = request.contains(KEYWORD_SKIPPED_BYTES)
							                      ? request.value(KEYWORD_SKIPPED_BYTES).toVariant().toUInt()
							                      : 0;

							qDebug() << this << "File sending about to begin" << serverPort << skippedBytes
							         << transferObject.file;

							QFile file(transferObject.file);

							if (file.exists() && file.open(QFile::OpenModeFlag::ReadOnly)) {
								transferObject.accessPort = serverPort;
								transferObject.skippedBytes = skippedBytes;
								auto fileSize = static_cast<size_t>(file.size());

								reply.insert(KEYWORD_RESULT, true);

								if (transferObject.fileSize != fileSize) {
									reply.insert(KEYWORD_SIZE_CHANGED, file.size());
									transferObject.fileSize = fileSize;
								}

								connection->reply(reply);

								if (skippedBytes > 0)
									if (!file.seek(static_cast<qint64>(skippedBytes)))
										qDebug() << this << "File seek failed for byte" << skippedBytes;

								qDebug() << this << "About to connect";

								if (transferObject.flag != TransferObject::Flag::Running) {
									transferObject.flag = TransferObject::Flag::Running;
									gDbSignal->update(transferObject);
								}

								QTcpSocket socket;
								socket.open(QTcpSocket::OpenModeFlag::WriteOnly);

								{
									// Establish the connection to the socket that will send the file
									socket.connectToHost(connection->socket()->peerAddress(), serverPort);

									while (QAbstractSocket::SocketState::ConnectingState == socket.state())
										socket.waitForConnected(TIMEOUT_SOCKET_DEFAULT_LARGE);

									if (QAbstractSocket::SocketState::ConnectedState != socket.state()) {
										qDebug() << this << "Did not connect";
										throw exception();
									}

									socket.setSocketOption(QTcpSocket::SocketOption::LowDelayOption, 1);
								}

								char *buffer = new char[BUFFER_LENGTH_DEFAULT];

								try {
									auto lastUpdated = time(nullptr);

									while (!file.atEnd()) {
										time_t currentTime = time(nullptr);

										if (socket.bytesToWrite() == 0) {
											//socket.write(file.read(BUFFER_LENGTH_DEFAULT));
											qint64 readSize = file.read(buffer, BUFFER_LENGTH_DEFAULT);
											socket.write(buffer, readSize);
											socket.flush();
										} else if (thisTask->interrupted()
										           || (socket.bytesToWrite() > 0
										               && !socket.waitForBytesWritten(TIMEOUT_SOCKET_DEFAULT))) {
											qDebug() << this << "Timed out or interrupted:" << thisTask->interrupted();
											throw exception();
										}

										if (currentTime - lastUpdated > 2) {
											// only emit current file size
											// also always update when a file status changes
											qDebug() << this << "Notified updated";
											emit gTaskMgr->taskStatus(thisTask->m_groupId, thisTask->m_deviceId,
											                          thisTask->m_type,
											                          file.pos(),
											                          transferObject);
											lastUpdated = currentTime;
										}
									}

									qDebug() << this << "I/O Completed";
									transferObject.flag = TransferObject::Flag::Done;
								} catch (...) {
									qDebug() << this << "I/O Error occurred";
									transferObject.flag = TransferObject::Flag::Interrupted;
								}

								delete[] buffer;
								socket.close();
								file.close();
							} else {
								connection->reply({
										                  {KEYWORD_RESULT,            false},
										                  {KEYWORD_ERROR, KEYWORD_ERROR_NOT_ACCESSIBLE},
										                  {KEYWORD_FLAG_GROUP_EXISTS, true}
								                  });

								transferObject.flag = TransferObject::Flag::Interrupted;

								qDebug() << this << "File does not exist or did not open";
							}
						} catch (...) {
							qDebug() << this << "Some error occurred";
							emit gTaskMgr->taskError(thisTask->m_groupId, thisTask->m_deviceId, thisTask->m_type,
							                         TransferTaskManager::TaskError::DuringTask);
						}

						gDbSignal->update(transferObject);
					} else {
						qDebug() << this << "Object does not exist, but the group does";
						connection->reply({
								                  {KEYWORD_RESULT,            false},
								                  {KEYWORD_ERROR, KEYWORD_ERROR_NOT_FOUND},
								                  {KEYWORD_FLAG_GROUP_EXISTS, true}
						                  });
					}
				} else if (thisTask->interrupted()) {
					qDebug() << this << "Interrupted";

					connection->reply({
							                  {KEYWORD_RESULT,            false},
							                  {KEYWORD_TRANSFER_JOB_DONE, false}
					                  });
				}
			}
		}
	} catch (...) {
		qDebug() << this << "Error occurred";
	}

	if (thisTask != nullptr) {
		gTaskMgr->detachTask(thisTask);
		delete thisTask;
	}
}