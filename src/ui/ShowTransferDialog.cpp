//
// Created by veli on 2/16/19.
//

#include "ShowTransferDialog.h"

ShowTransferDialog::ShowTransferDialog(QWidget *parent, groupid groupId)
        : QDialog(parent), m_ui(new Ui::ShowTransferDialog), m_objectModel(new TransferObjectModel(groupId))
{
    m_groupId = groupId;
    m_ui->setupUi(this);
    m_ui->transfersTreeView->setModel(m_objectModel);
    m_ui->transfersTreeView->setColumnWidth(0, 250);
}

ShowTransferDialog::~ShowTransferDialog()
{
    delete m_ui;
    delete m_objectModel;
}
