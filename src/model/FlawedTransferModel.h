//
// Created by veli on 3/5/19.
//

#ifndef TREBLESHOT_FLAWEDTRANSFERMODEL_H
#define TREBLESHOT_FLAWEDTRANSFERMODEL_H

#include <QIcon>
#include <QtCore/QAbstractTableModel>
#include <src/database/object/TransferObject.h>

class FlawedTransferModel : public QAbstractTableModel {
	Q_OBJECT

public:
	enum ColumnNames {
		FileName,
		Status,
		__itemCount
	};

	explicit FlawedTransferModel(groupid groupId, QObject *parent = nullptr);

	~FlawedTransferModel();

	int columnCount(const QModelIndex &parent) const override;

	int rowCount(const QModelIndex &parent) const override;

	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	QVariant data(const QModelIndex &index, int role) const override;

	const QList<TransferObject> *list() const;

public slots:

	void databaseChanged(const SqlSelection &change, ChangeType type);

protected:
	QList<TransferObject> *m_list;
	groupid m_groupId;
};

#endif //TREBLESHOT_FLAWEDTRANSFERMODEL_H
