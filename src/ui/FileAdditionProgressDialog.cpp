//
// Created by veli on 2/24/19.
//

#include <QUrl>
#include <QPushButton>
#include <src/util/GThread.h>
#include <QtCore/QRandomGenerator>
#include <src/database/object/TransferGroup.h>
#include <QtCore/QMimeDatabase>
#include <src/database/object/TransferObject.h>
#include <src/util/TransferUtils.h>
#include <src/util/AppUtils.h>
#include <QtWidgets/QMessageBox>
#include "FileAdditionProgressDialog.h"
#include "DeviceChooserDialog.h"

FileAdditionProgressDialog::FileAdditionProgressDialog(QWidget *parent, const QList<QUrl> &urls)
        : QDialog(parent), m_ui(new Ui::FileAdditionProgressDialog)
{
    m_thread = new GThread([this, urls](GThread *thread) { task(thread, urls); }, true);
    m_ui->setupUi(this);

    connect(m_thread, &GThread::statusUpdate, this, &FileAdditionProgressDialog::taskProgress);
    connect(m_thread, &GThread::finished, this, &FileAdditionProgressDialog::close);
    connect(this, &QDialog::finished, m_thread, &GThread::notifyInterrupt);

    m_thread->start();
}

FileAdditionProgressDialog::~FileAdditionProgressDialog()
{
    delete m_ui;
}

void FileAdditionProgressDialog::task(GThread *thread, const QList<QUrl> &urls)
{
    try {
        auto groupId = QRandomGenerator::global()->bounded(static_cast<groupid>(time(nullptr)), sizeof(int));
        requestid requestId = groupId + 1;
        TransferGroup group(groupId);
        QMimeDatabase mimeDb;
        QList<TransferObject> transferMap;

        {
            int position = 0;
            for (const auto &url : urls) {
                if (thread->interrupted())
                    throw exception();

                transferMap << TransferUtils::createTransferMap(group, mimeDb, requestId, url.toLocalFile());
                emit thread->statusUpdate(urls.size(), position++, url.toLocalFile());
            }
        }

        try {
            if (gDbSignal->transaction()) {
                int position = 0;
                for (auto &transferObject : transferMap) {
                    if (thread->interrupted())
                        throw exception();

                    gDbSignal->insert(transferObject);
                    emit thread->statusUpdate(transferMap.size(), position++, transferObject.friendlyName);
                }

                gDbSignal->commit();
            } else {
                QMessageBox error(this);
                error.setText(tr("Could not add the files right now. Try again."));
                error.show();
            }

            if (!transferMap.empty()) {
                gDbSignal->insert(group);
                emit filesAdded(group.id);
            }
        } catch (...) {
            // do nothing
        }
    } catch (...) {
        // do nothing
    }

}

void FileAdditionProgressDialog::taskProgress(int max, int progress, const QString &text)
{
    m_ui->progressBar->setMaximum(max);
    m_ui->progressBar->setValue(progress);
    m_ui->label->setText(text);
}
