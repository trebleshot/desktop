//
// Created by veli on 2/24/19.
//

#ifndef TREBLESHOT_DEVICECHOOSERDIALOG_H
#define TREBLESHOT_DEVICECHOOSERDIALOG_H

#include <QtWidgets/QDialog>
#include <QPushButton>
#include <src/database/object/TransferGroup.h>
#include <src/model/NetworkDeviceModel.h>

namespace Ui {
    class DeviceChooserDialog;
}

class DeviceChooserDialog : public QDialog {
Q_OBJECT
    groupid m_groupId;

public:
    explicit DeviceChooserDialog(QWidget *parent, groupid groupId);

    ~DeviceChooserDialog() override;

public slots:

    void modelActivated(const QModelIndex &modelIndex);

    void modelPressed(const QModelIndex &modelIndex);

    void selectionAccepted();

signals:
    void devicesSelected(groupid groupId, QList<NetworkDevice> devices);

protected:
    Ui::DeviceChooserDialog *m_ui;
    NetworkDeviceModel *m_deviceModel;
};


#endif //TREBLESHOT_DEVICECHOOSERDIALOG_H
