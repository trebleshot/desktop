/*
* Copyright (C) 2019 Veli Tasalı, created on 3/3/19
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

#include <QtCore/QList>
#include <QtCore/QItemSelectionModel>
#include "SynchronizedList.h"

class ViewUtils {
public:

	static QList<int> getSelectionRows(QItemSelectionModel *model);

	static QList<int> getSelectionRows(const QModelIndexList &index);

	template<typename T>
	static bool gatherSelections(QItemSelectionModel *model, SynchronizedList<T> *parentList, QList<T> &resultList)
	{
		const auto &selectedRows = getSelectionRows(model);

		if (!selectedRows.empty()) {
			MutexEnablingScope mutexScope(parentList);

			for (const int row : selectedRows)
				if (row >= 0 && row < parentList->list()->size())
					resultList.append(parentList->list()->at(row));

			return !resultList.empty();
		}

		return false;
	}
};