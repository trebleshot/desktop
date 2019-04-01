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

#define gAccessList(list) MutexEnablingScope(list)

/**
 * Handles queued access to a class. Most probably to a list.
 */
class ListMutex {
	QMutex *m_listMutex = new QMutex(QMutex::NonRecursive);

public:
	/**
	 * Request access to the list when possible.
	 * @return Access granted if true.
	 */
	bool accessList();

	/**
	 * Release the existing lock.
	 * @note This produces an assert error if not locked.
	 */
	void releaseList();
};

/**
 * Manages the list that should be accessed synchronously.
 * @tparam T Object type to put into the list.
 */
template<typename T>
class SynchronizedList : public ListMutex {
	QList<T> *m_list = new QList<T>;

public:
	~SynchronizedList()
	{
		delete m_list;
	}

	/**
	 * Clear the list by deleting the old one
	 * and allocating another list object.
	 * @note Should already have access to the list.
	 */
	void clearList()
	{
		delete m_list;
		m_list = new QList<T>;
	}

	/**
	 * Get the actual instance of the list.
	 * @return Returns the list pointer.
	 */
	QList<T> *list() const
	{
		return m_list;
	}
};

/**
 * Access to the list by generating a temporary object.
 * After the object is deleted, if access was successful,
 * releases the lock.
 */
class MutexEnablingScope {
	ListMutex *m_list;
	bool m_accessed;

public:
	/**
	 * Requests access from the {@param list}.
	 * @param list The access queue manager.
	 */
	explicit MutexEnablingScope(ListMutex *list);

	/**
	 * Release the lock is access if access was provided.
	 */
	~MutexEnablingScope();

	/*
	 * Check the access state
	 */
	bool accessed() const;

	explicit operator bool() const;
};
