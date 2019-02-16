//
// Created by veli on 2/16/19.
//

#include <src/model/NetworkDeviceModel.h>
#include "ManageDevicesDialog.h"

ManageDevicesDialog::ManageDevicesDialog(QWidget *parent)
        : QDialog(parent), m_ui(new Ui::ManageDevicesDialog)
{
    m_ui->setupUi(this);

    auto *deviceModel = new NetworkDeviceModel();

    qDebug() << sizeof(NetworkDeviceModel::ColumnNames
    );

    m_ui->treeView->setModel(deviceModel);

}
