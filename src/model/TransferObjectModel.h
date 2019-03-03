//
// Created by veli on 3/4/19.
//

#ifndef TREBLESHOT_TRANSFEROBJECTMODEL_H
#define TREBLESHOT_TRANSFEROBJECTMODEL_H


#include <QtCore/QAbstractTableModel>
#include <src/database/object/TransferObject.h>
#include <QIcon>

class TransferObjectModel : public QAbstractTableModel {
Q_OBJECT

public:
    enum ColumnNames {
        FileName,
        Size,
        Status,
        Directory,
        __itemCount
    };

    explicit TransferObjectModel(groupid groupId, QObject *parent = nullptr);

    int columnCount(const QModelIndex &parent) const override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    const QList<TransferObject> &list() const;

public slots:

    void databaseChanged(const SqlSelection &change, ChangeType type);

protected:
    QList<TransferObject> m_list;
    groupid m_groupId;
};


#endif //TREBLESHOT_TRANSFEROBJECTMODEL_H
