//
// Created by veli on 2/16/19.
//

#ifndef TREBLESHOT_MANAGEDEVICESWIDGET_H
#define TREBLESHOT_MANAGEDEVICESWIDGET_H

#include <QDialog>
#include "ui_ManageDevicesDialog.h"

namespace Ui {
    class ManageDevicesDialog;
}

class ManageDevicesDialog : public QDialog {
    Q_OBJECT

public:
    explicit ManageDevicesDialog(QWidget* parent);

protected:
    Ui::ManageDevicesDialog *m_ui;
};


#endif //TREBLESHOT_MANAGEDEVICESWIDGET_H
