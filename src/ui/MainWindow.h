#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
#include "ui_MainWindow.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;

    void dropEvent(QDropEvent *event) override;

protected:
    Ui::MainWindow *m_ui;
    SeamlessServer *m_seamlessServer;
    CommunicationServer *m_commServer;
    TransferGroupModel *m_groupModel;

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

public slots:

    void about();

    void aboutQt();

    void deviceBlocked(const QString &deviceId, const QHostAddress &address);

    void deviceForAddedFiles(groupid groupId, QList<NetworkDevice> devices);

    void filesAdded(groupid groupId);

    void manageDevices();

    void remove();

    void refreshStorageLocation();

    void send();

    void setStorageLocation();

    void showReceivedText(const QString &text, const QString &deviceId);

    void showTransferRequest(const QString &deviceId, groupid groupId, int filesTotal);

    void transferItemActivated(QModelIndex modelIndex);

    void updateAvailability();
};

#endif // MAINWINDOW_H
