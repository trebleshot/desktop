#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "coolsocket.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

public slots:
    void communicate(CoolSocket::ActiveConnection* connection);
};

#endif // MAINWINDOW_H
