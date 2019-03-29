//
// Created by veli on 3/5/19.
//

#ifndef TREBLESHOT_FLAWEDTRANSFERMODEL_H
#define TREBLESHOT_FLAWEDTRANSFERMODEL_H

#include <QIcon>
#include <QtCore/QAbstractTableModel>
#include <src/database/object/TransferObject.h>
#include <src/util/SynchronizedList.h>

class FlawedTransferModel : public QAbstractTableModel, public SynchronizedList<TransferObject> {
	Q_OBJECT

public:
	enum ColumnNames {
		FileName,
		Status,
		__itemCount
	};

	explicit FlawedTransferModel(groupid groupId, QObject *parent = nullptr);

	int columnCount(const QModelIndex &parent) const override;

	int rowCount(const QModelIndex &parent) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	QVariant data(const QModelIndex &index, int role) const override;

public slots:

	void databaseChanged(const SqlSelection &change, ChangeType type);

protected:
	groupid m_groupId;
};

#endif //TREBLESHOT_FLAWEDTRANSFERMODEL_H
