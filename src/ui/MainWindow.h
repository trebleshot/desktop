#pragma once

#include <src/database/AccessDatabase.h>
#include <src/broadcast/CommunicationServer.h>
#include <src/coolsocket/CoolSocket.h>
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
#include "ui_MainWindow.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

protected:
    Ui::MainWindow *m_ui;
    SeamlessServer *m_seamlessServer;
    CommunicationServer *m_commServer;
    TransferGroupModel *m_groupModel;
    NetworkDeviceModel *m_deviceModel;
	DNSSDService *m_discoveryService;

    void dragEnterEvent(QDragEnterEvent *event) override;

    void dropEvent(QDropEvent *event) override;

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

public slots:

    void about();

    void aboutQt();

    void deviceBlocked(const QString &deviceId, const QHostAddress &address);

    void deviceContextMenu(const QPoint &point);

    void deviceSelected(const QModelIndex &modelIndex);

    void showTransfer();

    void showTransfer(groupid groupId);

    void removeTransfer();

    void refreshStorageLocation();

    void savePathChanged();

    void selectFilesToSend();

    void setStorageLocation();

    static void showReceivedFiles();

    void showReceivedText(const QString &text, const QString &deviceId);

    void showTransferRequest(const QString &deviceId, groupid groupId, int filesTotal);

    void taskStart();

    void taskPause();

    void taskToggle();

    void transferItemActivated(const QModelIndex &modelIndex);

    void transferSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

    void transferContextMenu(const QPoint &point);

    static void usernameChanged(const QString& username);
};