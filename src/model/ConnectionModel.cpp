/*
* Copyright (C) 2019 Veli Tasalı, created on 4/1/19
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

#include "ConnectionModel.h"

ConnectionModel::ConnectionModel(const QString &deviceId, QObject *parent)
		: QAbstractTableModel(parent), DatabaseLoader(parent)
{
	m_deviceId = deviceId;
	DatabaseLoader::databaseChanged();
}

int ConnectionModel::columnCount(const QModelIndex &parent) const
{
	return ColumnName::__itemCount;
}

int ConnectionModel::rowCount(const QModelIndex &parent) const
{
	return list()->size();
}

void ConnectionModel::databaseChanged(const SqlSelection &change, ChangeType changeType)
{
	if (change.valid() && change.tableName != DB_TABLE_DEVICECONNECTION)
		return;

	if (gAccessList(this)) {
		emit layoutAboutToBeChanged();
		clearList();

		SqlSelection selection;
		selection.setTableName(DB_TABLE_DEVICECONNECTION);
		selection.setOrderBy(DB_FIELD_DEVICECONNECTION_LASTCHECKEDDATE, false);
		selection.setWhere(QString("%1 = ?").arg(DB_FIELD_DEVICECONNECTION_DEVICEID));
		selection.whereArgs << m_deviceId;

		gDatabase->castQuery(selection, *list());

		emit layoutChanged();
	}
}

QVariant ConnectionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			switch (section) {
				case ColumnName::IpAddress:
					return tr("Address");
				case ColumnName::AdapterName:
					return tr("Name");
				case ColumnName::LastCheckedData:
					return tr("Last checked");
				default:
					return QString("?");
			}
		} else
			return QString("%1").arg(section);
	}

	return QVariant();
}

QVariant ConnectionModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
		const auto &connection = list()->at(index.row());

		switch (index.column()) {
			case ColumnName::AdapterName:
				return connection.adapterName;
			case ColumnName::IpAddress:
				return connection.hostAddress.toString();
			case ColumnName::LastCheckedData:
				return QDateTime::fromTime_t(static_cast<uint>(connection.lastCheckedDate))
						.toString("H:m on ddd, d MMM");
			default:
				return QString("Data id %1x%2")
						.arg(index.row())
						.arg(index.column());
		}
	}

	return QVariant();
}