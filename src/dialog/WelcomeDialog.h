//
// Created by veli on 12/10/18.
//

#pragma once

#include <QtWidgets/QDialog>

namespace Ui {
    class WelcomeDialog;
}

class WelcomeDialog : public QDialog {
Q_OBJECT

public:
    explicit WelcomeDialog(QWidget *parent = nullptr);

protected:
    Ui::WelcomeDialog *ui;
};