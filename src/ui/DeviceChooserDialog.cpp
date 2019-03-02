//
// Created by veli on 2/24/19.
//

#include <ui_DeviceChooserDialog.h>
#include "DeviceChooserDialog.h"

DeviceChooserDialog::DeviceChooserDialog(QWidget *parent, groupid groupId)
        : QDialog(parent), m_ui(new Ui::DeviceChooserDialog), m_deviceModel(new NetworkDeviceModel)
{
    m_groupId = groupId;
    m_ui->setupUi(this);
    m_ui->treeView->setModel(m_deviceModel);
    m_ui->buttonBox->button(QDialogButtonBox::StandardButton::Ok)->setEnabled(false);

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
    auto *selectionModel = m_ui->treeView->selectionModel();

    if (selectionModel == nullptr) {
        qDebug() << "Selection model is empty. This might be an error";
        return;
    }

    QList<NetworkDevice> devices;

    for (const auto &modelIndex : selectionModel->selectedIndexes()) {
        if (!modelIndex.isValid() || modelIndex.column() != 0)
            continue;

        devices.append(m_deviceModel->list()->at(modelIndex.row()));
    }

    emit devicesSelected(m_groupId, devices);
}
