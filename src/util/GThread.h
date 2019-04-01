/*
* Copyright (C) 2019 Veli Tasalı, created on 2/7/19
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

#include <QtCore/QThread>
#include <QDebug>
#include "Interrupter.h"

class GThread : public QThread, public Interrupter {
Q_OBJECT
	const std::function<void(GThread *)> m_callback;

public:
	explicit GThread(std::function<void(GThread *)> function,
	                 bool deleteOnFinish = false,
	                 QObject *parent = nullptr);

	static GThread *startIndependent(const std::function<void(GThread *)> &function);

signals:

	void statusUpdate(int max, int progress, const QString &text);

public slots:

	void notifyInterrupt();

protected:
	void run() override;
};