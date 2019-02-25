//
// Created by veli on 2/16/19.
//

#ifndef TREBLESHOT_TRANSFERWINDOW_H
#define TREBLESHOT_TRANSFERWINDOW_H

#include <QDialog>
#include <ui_ShowTransferDialog.h>
#include <src/database/object/TransferGroup.h>

namespace Ui {
    class ShowTransferDialog;
}

class ShowTransferDialog : public QDialog {
Q_OBJECT

public:
    explicit ShowTransferDialog(QWidget *parentWindow, groupid groupId);

    ~ShowTransferDialog() override;

protected:
    Ui::ShowTransferDialog *m_ui;
};


#endif //TREBLESHOT_TRANSFERWINDOW_H
