//
// Created by veli on 3/24/19.
//

#include <QtGui/QDesktopServices>
#include <src/config/Config.h>
#include <QPushButton>
#include <QUrl>
#include "AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent)
        : QDialog(parent), m_ui(new Ui::AboutDialog)
{
    m_ui->setupUi(this);
    auto *moreButton = m_ui->buttonBox->addButton("More", QDialogButtonBox::ButtonRole::NoRole);

    connect(moreButton, &QPushButton::pressed, []() {
        QDesktopServices::openUrl(QUrl(URI_APP_HOME));
    });
}
