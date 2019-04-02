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

#include "SynchronizedList.h"

void ListMutex::accessList()
{
	m_counter++;

	if (m_counter == 1) {
		m_listMutex->lock();
		return;
	}

	qDebug() << this << "Not safe to lock. Already has" << m_counter << "handles.";

	if (!m_listMutex->tryLock(2000))
		qDebug() << this << "Failed to lock in 2 secs. This is an issue. Fix this to secure concurrent edits on lists.";
}

void ListMutex::releaseList()
{
	if (m_counter == 0) {
		qDebug() << this << "Nothing to unlock";
		return;
	}

	m_counter--;
	m_listMutex->unlock();
}

MutexEnablingScope::MutexEnablingScope(ListMutex *list) : m_list(list)
{
	list->accessList();
}

MutexEnablingScope::~MutexEnablingScope()
{
	m_list->releaseList();
}