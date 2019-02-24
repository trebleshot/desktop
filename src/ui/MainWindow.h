#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "src/database/AccessDatabase.h"
#include "src/broadcast/CommunicationServer.h"
#include "src/coolsocket/CoolSocket.h"
#include <QMainWindow>
#include <iostream>
#include "ui_MainWindow.h"

#include <QDesktopWidget>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSqlDriver>
#include <src/dialog/WelcomeDialog.h>
#include <src/model/TransferGroupModel.h>
#include <src/util/NetworkDeviceLoader.h>
#include <QtWidgets/QAbstractButton>
#include <QPushButton>
#include <QClipboard>

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
    CommunicationServer *m_commServer;
    TransferGroupModel *m_groupModel;

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

public slots:

    void about();

    void aboutQt();

    void manageDevices();

    void showReceivedText(const QString &text, const QString &deviceId);

    void showTransferRequest(const QString &deviceId, groupid groupId, int filesTotal);

    void transferItemActivated(QModelIndex modelIndex);

};

#endif // MAINWINDOW_H
