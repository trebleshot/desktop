//
// Created by veli on 2/16/19.
//

#ifndef TREBLESHOT_MANAGEDEVICESWIDGET_H
#define TREBLESHOT_MANAGEDEVICESWIDGET_H

#include <QDialog>
#include <QAction>
#include <src/model/NetworkDeviceModel.h>
#include "ui_ManageDevicesDialog.h"

namespace Ui {
    class ManageDevicesDialog;
}

class ManageDevicesDialog : public QDialog {
Q_OBJECT

public:
    explicit ManageDevicesDialog(QWidget *parent);

    ~ManageDevicesDialog() override;

    void itemContextMenu(const QPoint& point);

    void itemSelected(const QModelIndex& modelIndex);

protected:
    Ui::ManageDevicesDialog *m_ui;
    NetworkDeviceModel *m_deviceModel;
};

#endif //TREBLESHOT_MANAGEDEVICESWIDGET_H
