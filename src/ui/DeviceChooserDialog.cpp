//
// Created by veli on 2/24/19.
//

#include <ui_DeviceChooserDialog.h>
#include <src/util/ViewUtils.h>
#include "DeviceChooserDialog.h"

DeviceChooserDialog::DeviceChooserDialog(QWidget *parent, groupid groupId)
        : QDialog(parent), m_ui(new Ui::DeviceChooserDialog), m_deviceModel(new NetworkDeviceModel)
{
    m_groupId = groupId;
    m_ui->setupUi(this);
    m_ui->treeView->setModel(m_deviceModel);
    m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);
    m_ui->treeView->setColumnWidth(0, 120);

    connect(m_ui->treeView, &QTreeView::activated, this, &DeviceChooserDialog::modelActivated);
    connect(m_ui->treeView, &QTreeView::pressed, this, &DeviceChooserDialog::modelPressed);
    connect(m_ui->buttonBox, &QDialogButtonBox::accepted, this, &DeviceChooserDialog::selectionAccepted);
}

DeviceChooserDialog::~DeviceChooserDialog()
{
    delete m_ui;
    delete m_deviceModel;
}

void DeviceChooserDialog::modelActivated(const QModelIndex &modelIndex)
{
    selectionAccepted();
    close();
}

void DeviceChooserDialog::modelPressed(const QModelIndex &modelIndex)
{
    m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(modelIndex.isValid());
}

void DeviceChooserDialog::selectionAccepted()
{
    QList<NetworkDevice> devices;

    for (int row : ViewUtils::getSelectionRows(m_ui->treeView->selectionModel())) {
        devices.append(m_deviceModel->list()->at(row));
    }

    emit devicesSelected(m_groupId, devices);
}
