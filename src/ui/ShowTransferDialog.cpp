//
// Created by veli on 2/16/19.
//

#include "ShowTransferDialog.h"

ShowTransferDialog::ShowTransferDialog(QWidget *parent, groupid groupId)
        : QDialog(parent), m_ui(new Ui::ShowTransferDialog)
{
    m_ui->setupUi(this);
    auto* model = DbStructure::gatherTableModel(DB_DIVIS_TRANSFER);

    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setFilter(QString("%1 = %2").arg(DB_FIELD_TRANSFERGROUP_ID).arg(groupId));
    model->setHeaderData(0, Qt::Horizontal, tr("File name"));
    model->setHeaderData(1, Qt::Horizontal, tr("Bytes"));

    model->select();

    m_ui->treeView->setModel(model);

    m_ui->treeView->hideColumn(0);
    m_ui->treeView->hideColumn(1);
    m_ui->treeView->hideColumn(2);
    m_ui->treeView->hideColumn(3);
}

ShowTransferDialog::~ShowTransferDialog()
{
    delete m_ui;
}
