//
// Created by veli on 1/22/19.
//

#ifndef TREBLESHOT_TRANSFERGROUPLISTMODEL_H
#define TREBLESHOT_TRANSFERGROUPLISTMODEL_H

#include <QtCore/QAbstractListModel>
#include <iostream>
#include <src/util/AppUtils.h>

class TransferGroupListModel
        : public QAbstractListModel {
Q_OBJECT

private:
    QString *m_columnNames;

public:
    explicit TransferGroupListModel(QObject *parent = nullptr)
            : QAbstractListModel(parent)
    {
        //m_columnNames = AppUtils::getDatabase();
    }

    ~TransferGroupListModel() override
    {
        if (m_columnNames != nullptr)
            delete m_columnNames;
    }

    int columnCount(const QModelIndex &parent) const override
    {
        return 12;
    }

    int rowCount(const QModelIndex &parent) const override
    {
        return 0;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (role != Qt::DisplayRole)
            return QVariant();

        if (orientation == Qt::Horizontal)
            return QString("Column %1").arg(section);
        else
            return QString("%1").arg(section);
    }


    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role == Qt::DisplayRole)
            return QString::asprintf("Data id %dx%d", index.row(), index.column());

        return QVariant();
    }
};


#endif //TREBLESHOT_TRANSFERGROUPLISTMODEL_H
