#include "SynchronizedList.h"

bool ListMutex::accessList()
{
    return m_listMutex->tryLock(2000);
}

void ListMutex::releaseList()
{
    m_listMutex->unlock();
}

MutexEnablingScope::MutexEnablingScope(ListMutex *list) : m_list(list) {
    m_accessed = list->accessList();
}

MutexEnablingScope::~MutexEnablingScope()
{
    if (m_accessed)
        m_list->releaseList();
}

bool MutexEnablingScope::accessed()
{
	return m_accessed;
}
