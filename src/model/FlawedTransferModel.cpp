/*
* Copyright (C) 2019 Veli Tasalı, created on 3/5/19
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

#include <src/util/AppUtils.h>
#include <src/util/TransferUtils.h>
#include "FlawedTransferModel.h"

FlawedTransferModel::FlawedTransferModel(groupid groupId, QObject *parent)
		: QAbstractTableModel(parent)
{
	m_groupId = groupId;
	connect(gDatabase, &AccessDatabase::databaseChanged, this, &FlawedTransferModel::databaseChanged);
	databaseChanged(SqlSelection(), ChangeType::Any);
}

int FlawedTransferModel::columnCount(const QModelIndex &parent) const
{
	return ColumnName::__itemCount;
}

int FlawedTransferModel::rowCount(const QModelIndex &parent) const
{
	return list()->size();
}

QVariant FlawedTransferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			switch (section) {
				case ColumnName::Status:
					return tr("Error");
				case ColumnName::FileName:
					return tr("File name");
				default:
					return QString("?");
			}
		} else
			return QString("%1").arg(section);
	}

	return QVariant();
}

QVariant FlawedTransferModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
		const auto &currentObject = list()->at(index.row());

		switch (index.column()) {
			case ColumnName::FileName:
				return currentObject.friendlyName;
			case ColumnName::Status:
				return TransferUtils::getFlagString(currentObject.flag);
			default:
				return QString("Data id %1x%2")
						.arg(index.row())
						.arg(index.column());
		}
	} else if (role == Qt::DecorationRole) {
		switch (index.column()) {
			case ColumnName::FileName: {
				const auto &currentGroup = list()->at(index.row());
				return QIcon(currentGroup.type == TransferObject::Type::Incoming
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

void FlawedTransferModel::databaseChanged(const SqlSelection &change, ChangeType type)
{
	if ((!change.valid() || change.tableName == DB_TABLE_TRANSFER || change.tableName == DB_DIVIS_TRANSFER)
	    && gAccessList(this)) {
		emit layoutAboutToBeChanged();
		clearList();

		SqlSelection selection;
		selection.setTableName(DB_TABLE_TRANSFER);
		selection.setOrderBy(QString("%1 ASC, %2 ASC")
				                     .arg(DB_FIELD_TRANSFER_NAME)
				                     .arg(DB_FIELD_TRANSFER_DIRECTORY));
		selection.setWhere(QString("%1 = ? AND (%2 = ? OR %3 = ?)")
				                   .arg(DB_FIELD_TRANSFER_GROUPID)
				                   .arg(DB_FIELD_TRANSFER_FLAG)
				                   .arg(DB_FIELD_TRANSFER_FLAG));
		selection.whereArgs << m_groupId
		                    << TransferObject::Flag::Removed
		                    << TransferObject::Flag::Interrupted;

		gDatabase->castQuery(selection, *list());

		if (list()->empty()) {
			selection.setTableName(DB_DIVIS_TRANSFER);
			gDatabase->castQuery(selection, *list());
		}

		emit layoutChanged();
	}
}