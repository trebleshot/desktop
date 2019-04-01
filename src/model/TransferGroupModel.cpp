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

#include "TransferGroupModel.h"

TransferGroupModel::TransferGroupModel(QObject *parent) : QAbstractTableModel(parent)
{
	DatabaseLoader::databaseChanged();
}

int TransferGroupModel::columnCount(const QModelIndex &parent) const
{
	return ColumnName::__itemCount;
}

int TransferGroupModel::rowCount(const QModelIndex &parent) const
{
	return list()->size();
}

QVariant TransferGroupModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			switch (section) {
				case ColumnName::Status:
					return tr("Status");
				case ColumnName::Devices:
					return tr("Devices");
				case ColumnName::Date:
					return tr("Date");
				case ColumnName::Size:
					return tr("Size");
				default:
					return QString("?");
			}
		} else
			return QString("%1").arg(section);
	}

	return QVariant();
}

QVariant TransferGroupModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
		const auto &currentGroup = list()->at(index.row());

		switch (index.column()) {
			case ColumnName::Devices: {
				QString devicesString;

				if (currentGroup.assignees.empty())
					devicesString.append("-");
				else {
					for (const auto &assigneeInfo : currentGroup.assignees) {
						if (devicesString.length() > 0)
							devicesString.append(", ");

						devicesString.append(assigneeInfo.device.nickname);
					}
				}

				return devicesString;
			}
			case ColumnName::Status:
				return QString("%1 of %2")
						.arg(currentGroup.completed)
						.arg(currentGroup.total);
			case ColumnName::Size:
				return TransferUtils::sizeExpression(currentGroup.totalBytes, false);
			case ColumnName::Date:
				return QDateTime::fromTime_t(static_cast<uint>(currentGroup.group.dateCreated))
						.toString("ddd, d MMM");
			default:
				return QString("Data id %1x%2")
						.arg(index.row())
						.arg(index.column());
		}
	} else if (role == Qt::DecorationRole) {
		switch (index.column()) {
			case ColumnName::Devices: {
				const auto &currentGroup = list()->at(index.row());
				return QIcon(currentGroup.hasIncoming
				             ? ":/icon/arrow_down"
				             : ":/icon/arrow_up");
			}
			default: {
				// do nothing
			}
		}
	}

	return QVariant();
}

void TransferGroupModel::databaseChanged(const SqlSelection &change, ChangeType changeType)
{
	if ((!change.valid() || change.tableName == DB_TABLE_TRANSFERGROUP || change.tableName == DB_TABLE_TRANSFER)
	    && gAccessList(this)) {
		emit layoutAboutToBeChanged();
		clearList();

		QList<TransferGroup> groupList;
		SqlSelection selection;
		selection.setTableName(DB_TABLE_TRANSFERGROUP);
		selection.setOrderBy(DB_FIELD_TRANSFERGROUP_DATECREATED, false);

		gDatabase->castQuery(selection, groupList);

		for (const auto &transferGroup : groupList)
			list()->append(TransferUtils::getInfo(transferGroup));

		emit layoutChanged();
	}
}