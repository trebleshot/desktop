#pragma once

#include <QList>
#include <QMutex>

#define gAccessList(list) ListSynchronizerScopedAccess(list) == true

class ListSynchronizer {
	QMutex *m_listMutex = new QMutex(QMutex::NonRecursive); // Non-recursive

public:
	bool accessList() {
		return m_listMutex->tryLock(2000);
	}

	void releaseList() {
		m_listMutex->unlock();
	}
};

template<typename T>
class SynchronizedList : public ListSynchronizer {
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

class ListSynchronizerScopedAccess {
	ListSynchronizer *m_list;
	bool m_accessed;

public:
	ListSynchronizerScopedAccess(ListSynchronizer *list) : m_list(list) {
		m_accessed = list->accessList();
	}

	~ListSynchronizerScopedAccess() {
		if (m_accessed)
			m_list->releaseList();
	}

	bool operator==(bool compare) {
		return compare == m_accessed;
	}
};
