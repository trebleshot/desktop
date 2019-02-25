//
// Created by veli on 2/24/19.
//

#ifndef TREBLESHOT_FILEADDITIONPROGRESSDILAOG_H
#define TREBLESHOT_FILEADDITIONPROGRESSDILAOG_H

#include <ui_FileAdditionProgressDialog.h>
#include <QtWidgets/QDialog>

namespace Ui {
    class FileAdditionProgressDialog;
}

class FileAdditionProgressDialog : public QDialog {
Q_OBJECT

public:
    explicit FileAdditionProgressDialog(QWidget *parent, const QList<QUrl> &urls);

    ~FileAdditionProgressDialog() override;

    void task(GThread *thread, const QList<QUrl> &urls);

public slots:

    void taskProgress(int max, int progress, const QString &text);

signals:

    void filesAdded(groupid groupId);

protected:
    Ui::FileAdditionProgressDialog *m_ui;
    GThread *m_thread;
};


#endif //TREBLESHOT_FILEADDITIONPROGRESSDILAOG_H
