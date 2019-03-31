/*
* Copyright (C) 2019 Veli Tasalı, created on 3/24/19
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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