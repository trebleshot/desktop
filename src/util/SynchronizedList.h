/*
* Copyright (C) 2019 Veli Tasalı
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

#include <QList>
#include <QMutex>

#define gAccessList(list) MutexEnablingScope(list).accessed()

class ListMutex {
	QMutex *m_listMutex = new QMutex(QMutex::NonRecursive);

public:
	bool accessList();

	void releaseList();
};

template<typename T>
class SynchronizedList : public ListMutex {
	QList<T> *m_list = new QList<T>;

public:
	~SynchronizedList() {
		delete m_list;
	}

	void clearList() {
		delete m_list;
		m_list = new QList<T>;
	}

	QList<T> *list() const {
		return m_list;
	}
};

class MutexEnablingScope {
	ListMutex *m_list;
	bool m_accessed;

public:
	explicit MutexEnablingScope(ListMutex *list);

	~MutexEnablingScope();

	bool accessed();
};
