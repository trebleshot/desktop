//
// Created by veli on 2/24/19.
//

#include <ui_DeviceChooserDialog.h>
#include "DeviceChooserDialog.h"

DeviceChooserDialog::DeviceChooserDialog(QWidget *parent)
        : QDialog(parent), m_ui(new Ui::DeviceChooserDialog)
{
    m_ui->setupUi(this);
}

DeviceChooserDialog::~DeviceChooserDialog()
{
    delete m_ui;
}
