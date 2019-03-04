//
// Created by veli on 2/16/19.
//

#ifndef TREBLESHOT_TRANSFERWINDOW_H
#define TREBLESHOT_TRANSFERWINDOW_H

#include <QDialog>
#include <ui_ShowTransferDialog.h>
#include <src/database/object/TransferGroup.h>
#include <src/model/TransferObjectModel.h>

namespace Ui {
    class ShowTransferDialog;
}

class ShowTransferDialog : public QDialog {
Q_OBJECT

public:
    explicit ShowTransferDialog(QWidget *parentWindow, groupid groupId);

    ~ShowTransferDialog() override;

public slots:

    void addDevices();

    void assigneeChanged(int index);

    void changeSavePath();

    void globalTaskStarted(groupid groupId, const QString &deviceId, int type);

    void globalTaskFinished(groupid groupId, const QString &deviceId, int type);

    void removeTransfer();

    void checkGroupIntegrity(const SqlSelection &change, ChangeType type);

    void saveDirectory();

    void sendToDevices(groupid groupId, QList<NetworkDevice> devices);

    void showFiles();

    void startTransfer();

    void taskToggle();

    void updateAssignees();

    void updateButtons();

protected:
    Ui::ShowTransferDialog *m_ui;
    TransferObjectModel *m_objectModel;
    TransferGroup m_group;
    TransferGroupInfo m_groupInfo;
    QList<AssigneeInfo> m_assigneeList;
};


#endif //TREBLESHOT_TRANSFERWINDOW_H
