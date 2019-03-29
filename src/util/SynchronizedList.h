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
