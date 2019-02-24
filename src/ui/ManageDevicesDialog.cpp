//
// Created by veli on 2/16/19.
//

#include <src/model/NetworkDeviceModel.h>
#include "ManageDevicesDialog.h"

ManageDevicesDialog::ManageDevicesDialog(QWidget *parent)
        : QDialog(parent), m_ui(new Ui::ManageDevicesDialog), m_deviceModel(new NetworkDeviceModel())
{
    m_ui->setupUi(this);
    m_ui->treeView->setModel(m_deviceModel);
}

ManageDevicesDialog::~ManageDevicesDialog()
{
    delete m_ui;
    delete m_deviceModel;
}
