//
// Created by veli on 2/16/19.
//

#include "ShowTransferDialog.h"

ShowTransferWidget::ShowTransferWidget(QWidget *parent, groupid groupId)
        : QDialog(parent), m_ui(new Ui::ShowTransferDialog)
{
    m_ui->setupUi(this);
}

ShowTransferWidget::~ShowTransferWidget()
{
    delete m_ui;
}
