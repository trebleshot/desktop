#pragma once

#include <src/database/AccessDatabase.h>
#include <src/broadcast/CommunicationServer.h>
#include <QMainWindow>
#include <iostream>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSqlDriver>
#include <QtCore/QJsonArray>
#include <src/dialog/WelcomeDialog.h>
#include <src/model/TransferGroupModel.h>
#include <src/util/NetworkDeviceLoader.h>
#include <QtWidgets/QAbstractButton>
#include <QPushButton>
#include <QClipboard>
#include <src/broadcast/SeamlessServer.h>
#include <src/model/NetworkDeviceModel.h>
#include <src/broadcast/DNSSDService.h>
#include <src/model/TextStreamModel.h>
#include "ui_MainWindow.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow() override;

public slots:

	void about();

	static void aboutQt();

	void copyTextStream();

	void copySavedSelectedText();

	void deleteSavedSelectedText();

	void deviceBlocked(const QString &deviceId, const QHostAddress &address);

	void deviceContextMenu(const QPoint &point);

	void deviceSelected(const QModelIndex &modelIndex);

	void removeTransfer();

	void refreshStorageLocation();

	void savePathChanged();

	void selectFilesToSend();

	void sendTextTo();

	void sendTextToDevices(const QList<NetworkDevice> &devices);

	void setStorageLocation();

	static void showReceivedFiles();

	void showReceivedText(const QString &text, const QString &deviceId);

	void saveTextStream();

	void showTransferRequest(const QString &deviceId, groupid groupId, int filesTotal);

	void showTransfer();

	void showTransferWithAddDevicesDialog(groupid groupId);

	void showTransfer(groupid groupId, bool showAddDeviceDialog = false);

	void taskStart();

	void taskPause();

	void taskToggle();

	void textItemActivated(const QModelIndex &modelIndex);

	void transferItemActivated(const QModelIndex &modelIndex);

	void transferSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

	void transferContextMenu(const QPoint &point);

	void updateButtons();

	void usernameChanged();

protected:
	Ui::MainWindow *m_ui;
	SeamlessServer *m_seamlessServer;
	CommunicationServer *m_commServer;
	TransferGroupModel *m_groupModel;
	NetworkDeviceModel *m_deviceModel;
	TextStreamModel *m_textStreamModel;
	DNSSDService *m_discoveryService;

	void dragEnterEvent(QDragEnterEvent *event) override;

	void dropEvent(QDropEvent *event) override;
};