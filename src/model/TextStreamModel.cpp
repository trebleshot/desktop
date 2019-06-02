/*
* Copyright (C) 2019 Veli Tasalı, created on 6/1/19
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

#include "TextStreamModel.h"

TextStreamModel::TextStreamModel(QObject *parent)
		: QAbstractTableModel(parent), DatabaseLoader(parent)
{
	DatabaseLoader::databaseChanged();
}

int TextStreamModel::columnCount(const QModelIndex &parent) const
{
	return ColumnName::__itemCount;
}

int TextStreamModel::rowCount(const QModelIndex &parent) const
{
	return list()->size();
}

void TextStreamModel::databaseChanged(const SqlSelection &change, ChangeType type)
{
	if (!change.valid() || change.tableName == DB_TABLE_CLIPBOARD) {
		emit layoutAboutToBeChanged();

		SqlSelection selection;
		selection.setTableName(DB_TABLE_CLIPBOARD);
		selection.setOrderBy(DB_FIELD_CLIPBOARD_TIME, false);

		{
			MutexEnablingScope scope(this);
			clearList();
			gDatabase->castQuery(selection, *list());
		}

		emit layoutChanged();
	}
}

QVariant TextStreamModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			switch (section) {
				case ColumnName::Text:
					return tr("Text");
				case ColumnName::Time:
					return tr("Date");
				default:
					return QString("?");
			}
		} else
			return QString("%1").arg(section);
	}

	return QVariant();
}

QVariant TextStreamModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
		const auto &thisDevice = list()->at(index.row());

		switch (index.column()) {
			case ColumnName::Text:
				return thisDevice.text;
			case ColumnName::Time:
				return QDateTime::fromTime_t(static_cast<uint>(thisDevice.dateCreated))
						.toString("ddd, d MMM");
			default:
				return QString("Data id %1x%2")
						.arg(index.row())
						.arg(index.column());
		}
	}

	return QVariant();
}