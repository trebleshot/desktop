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
#include <src/model/TransferGroupListModel.h>
#include <src/util/NetworkDeviceLoader.h>
#include <QtWidgets/QAbstractButton>
#include <QPushButton>
#include <QClipboard>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT
    Ui::MainWindow *m_ui;
    CommunicationServer *m_commServer;

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

public slots:

    void about();

    void aboutQt();

    void failureDialogFinished(int state);

    void showReceivedText(const QString &text, const QString &deviceId);

    void transferItemActivated(QModelIndex modelIndex);

};

#endif // MAINWINDOW_H
