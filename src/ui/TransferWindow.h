//
// Created by veli on 2/16/19.
//

#ifndef TREBLESHOT_TRANSFERWINDOW_H
#define TREBLESHOT_TRANSFERWINDOW_H

#include <QtGui/QWindow>
#include "ui_TransferWindow.h"

namespace Ui {
    class TransferWindow;
}

class TransferWindow : public QWindow {
Q_OBJECT;
    Ui::TransferWindow *m_ui;

public:
    explicit TransferWindow(QWindow *parentWindow);
};


#endif //TREBLESHOT_TRANSFERWINDOW_H
