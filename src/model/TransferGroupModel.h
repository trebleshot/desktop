/*
* Copyright (C) 2019 Veli Tasalı, created on 1/22/19
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

#include "src/util/TransferUtils.h"
#include <iostream>
#include <src/database/AccessDatabase.h>
#include <src/database/object/TransferGroup.h>
#include <src/database/object/TransferObject.h>
#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <QtCore/QAbstractListModel>
#include <QtCore/QDateTime>
#include <QIcon>
#include <QtGui/QIconEngine>
#include <QtCore/QMutex>
#include <src/util/SynchronizedList.h>
#include <src/database/DatabaseLoader.h>

class TransferGroupModel : public QAbstractTableModel, public SynchronizedList<TransferGroupInfo>,
                           public DatabaseLoader {
Q_OBJECT

public:
	enum ColumnName {
		Devices,
		Size,
		Status,
		Date,
		__itemCount
	};

	explicit TransferGroupModel(QObject *parent = nullptr);

	int columnCount(const QModelIndex &parent) const override;

	int rowCount(const QModelIndex &parent) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	QVariant data(const QModelIndex &index, int role) const override;

	void databaseChanged(const SqlSelection &change, ChangeType changeType) override;
};