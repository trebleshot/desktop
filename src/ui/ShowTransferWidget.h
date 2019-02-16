//
// Created by veli on 2/16/19.
//

#ifndef TREBLESHOT_TRANSFERWINDOW_H
#define TREBLESHOT_TRANSFERWINDOW_H

#include <QWidget>
#include "ui_ShowTransferWidget.h"

namespace Ui {
    class ShowTransferWidget;
}

class ShowTransferWidget : public QWidget {
Q_OBJECT

    Ui::ShowTransferWidget *m_ui;

public:
    explicit ShowTransferWidget(QWidget *parentWindow);
};


#endif //TREBLESHOT_TRANSFERWINDOW_H
