//
// Created by veli on 2/16/19.
//

#ifndef TREBLESHOT_MANAGEDEVICESWIDGET_H
#define TREBLESHOT_MANAGEDEVICESWIDGET_H

#include <QtWidgets/QWidget>
#include "ui_ManageDevicesWidget.h"

namespace Ui {
    class ManageDevicesWidget;
}

class ManageDevicesWidget : public QWidget {
    Q_OBJECT

    Ui::ManageDevicesWidget *m_ui;

public:
    explicit ManageDevicesWidget(QWidget* parent);
};


#endif //TREBLESHOT_MANAGEDEVICESWIDGET_H
