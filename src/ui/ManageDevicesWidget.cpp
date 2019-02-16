//
// Created by veli on 2/16/19.
//

#include "ManageDevicesWidget.h"

ManageDevicesWidget::ManageDevicesWidget(QWidget *parent)
        : QWidget(parent), m_ui(new Ui::ManageDevicesWidget)
{
    m_ui->setupUi(this);
}
