//
// Created by veli on 2/16/19.
//

#include <QtCore/QFileInfo>
#include <QtWidgets/QMessageBox>
#include <src/util/AppUtils.h>
#include <QtWidgets/QFileDialog>
#include <src/util/TransferUtils.h>
#include "ShowTransferDialog.h"
#include "DeviceChooserDialog.h"
#include "TransferRequestProgressDialog.h"

ShowTransferDialog::ShowTransferDialog(QWidget *parent, groupid groupId)
        : QDialog(parent), m_ui(new Ui::ShowTransferDialog), m_objectModel(new TransferObjectModel(groupId)),
          m_group(groupId), m_groupInfo()
{
    m_ui->setupUi(this);
    m_ui->transfersTreeView->setModel(m_objectModel);
    m_ui->transfersTreeView->setColumnWidth(0, 250);

    connect(gTaskMgr, &TransferTaskManager::taskAdded, this, &ShowTransferDialog::globalTaskStarted);
    connect(gTaskMgr, &TransferTaskManager::taskRemoved, this, &ShowTransferDialog::globalTaskFinished);
    connect(gDatabase, &AccessDatabase::databaseChanged, this, &ShowTransferDialog::checkGroupIntegrity);
    connect(m_ui->assigneesComboBox, SIGNAL(activated(int)), this, SLOT(assigneeChanged(int)));
    connect(m_ui->startButton, &QPushButton::pressed, this, &ShowTransferDialog::startTransfer);
    connect(m_ui->saveDirectoryButton, &QPushButton::pressed, this, &ShowTransferDialog::saveDirectory);
    connect(m_ui->removeButton, &QPushButton::pressed, this, &ShowTransferDialog::removeTransfer);
    connect(m_ui->chooseDirectoryButton, &QPushButton::pressed, this, &ShowTransferDialog::changeSavePath);
    connect(m_ui->addDevicesButton, &QPushButton::pressed, this, &ShowTransferDialog::addDevices);
    checkGroupIntegrity(SqlSelection(), ChangeType::Any);
}

ShowTransferDialog::~ShowTransferDialog()
{
    delete m_ui;
    delete m_objectModel;
}

void ShowTransferDialog::changeSavePath()
{
    QString string = QFileDialog::getExistingDirectory(this, "Choose where this transfer will be saved to",
                                                       TransferUtils::getSavePath(m_group));

    if (string != nullptr) {
        m_group.savePath = string;
        gDatabase->update(m_group);
    }
}

void ShowTransferDialog::saveDirectory()
{
    const auto &text = m_ui->storageLineEdit->text();

    if (text.isEmpty() || !QFileInfo::exists(text)) {
        QMessageBox box;
        box.setWindowTitle("Error");
        box.setText("The entered path cannot be used.");
        box.exec();
    }

    m_group.savePath = text;
    gDatabase->update(m_group);
}

void ShowTransferDialog::checkGroupIntegrity(const SqlSelection &change, ChangeType type)
{
    if (!change.valid() || change.tableName == DB_TABLE_TRANSFERASSIGNEE) {
        m_assigneeList = TransferUtils::getAllAssigneeInfo(m_group);
        updateAssignees();
    }

    if (!change.valid() || change.tableName == DB_TABLE_TRANSFERGROUP) {
        if (type == ChangeType::Update || type == ChangeType::Any)
            m_ui->storageLineEdit->setText(TransferUtils::getSavePath(m_group));

        if (type == ChangeType::Delete || type == ChangeType::Insert || type == ChangeType::Any) {
            if (!gDatabase->reconstructSilently(m_group)) {
                close();
                return;
            }

            m_groupInfo = TransferUtils::getInfo(m_group);
            updateButtons();
        }
    }
}

void ShowTransferDialog::updateAssignees()
{
    const QList<AssigneeInfo> &copyList = m_assigneeList;
    m_ui->assigneesComboBox->clear();
    m_ui->assigneesComboBox->addItem(tr("Any"), QString());

    for (const auto &info : copyList)
        m_ui->assigneesComboBox->addItem(info.device.nickname, info.device.id);
}

void ShowTransferDialog::updateButtons()
{
    bool hasRunning = gTaskMgr->hasActiveTasksFor(m_group.id);

    m_ui->startButton->setEnabled(m_groupInfo.hasIncoming || hasRunning);
    m_ui->startButton->setEnabled(hasRunning ? "Pause" : "Start");
    m_ui->addDevicesButton->setEnabled(m_groupInfo.hasOutgoing);
}

void ShowTransferDialog::addDevices()
{
    DeviceChooserDialog dialog(this, m_group.id);
    connect(&dialog, &DeviceChooserDialog::devicesSelected, this, &ShowTransferDialog::sendToDevices);
    dialog.exec();
}

void ShowTransferDialog::sendToDevices(groupid groupId, QList<NetworkDevice> devices)
{
    TransferRequestProgressDialog(this, groupId, devices).exec();
}

void ShowTransferDialog::removeTransfer()
{
    gDatabase->remove(m_group);
}

void ShowTransferDialog::startTransfer()
{
    taskToggle();
}

void ShowTransferDialog::assigneeChanged(int index)
{
    m_objectModel->setDeviceId(m_ui->assigneesComboBox->itemData(index).toString());
    m_objectModel->databaseChanged(SqlSelection(), ChangeType::Any);
}

void ShowTransferDialog::globalTaskStarted(groupid groupId, const QString &deviceId, TransferObject::Type type)
{
    updateButtons();
}

void ShowTransferDialog::globalTaskFinished(groupid groupId, const QString &deviceId, TransferObject::Type type)
{
    updateButtons();
}

void ShowTransferDialog::taskToggle()
{
    if (gTaskMgr->hasActiveTasksFor(m_group.id))
        gTaskMgr->pauseTasks(m_group.id);
    else if (m_groupInfo.hasIncoming && !m_assigneeList.empty())
        TransferUtils::startTransfer(m_group.id, m_assigneeList[0].device.id);
}
