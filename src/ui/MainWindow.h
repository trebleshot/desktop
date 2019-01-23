#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "src/database/AccessDatabase.h"
#include "src/broadcast/CommunicationServer.h"
#include "src/coolsocket/CoolSocket.h"
#include <QMainWindow>
#include <iostream>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

public Q_SLOTS:

    void about();

    void aboutQt();

    void failureDialogFinished(int state);

    void transferItemActivated(QModelIndex modelIndex);

    void customContextMenuRequested(QPoint point);

    void tableViewViewPortEntered();

private:
    Ui::MainWindow *ui;
    CommunicationServer *commServer;
};

#endif // MAINWINDOW_H
