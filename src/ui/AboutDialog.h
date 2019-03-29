//
// Created by veli on 3/24/19.
//

#pragma once

#include <QtWidgets/QDialog>
#include "ui_AboutDialog.h"

namespace Ui {
    class AboutDialog;
}

class AboutDialog : public QDialog {
Q_OBJECT

protected:
    Ui::AboutDialog *m_ui;

public:
    explicit AboutDialog(QWidget* parent);
};