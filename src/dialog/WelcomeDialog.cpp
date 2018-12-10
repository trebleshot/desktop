//
// Created by veli on 12/10/18.
//

#include "WelcomeDialog.h"
#include "ui_WelcomeDialog.h"

WelcomeDialog::WelcomeDialog(QWidget *parent)
        : QDialog(parent), ui(new Ui::WelcomeDialog)
{
    ui->setupUi(this);
}
