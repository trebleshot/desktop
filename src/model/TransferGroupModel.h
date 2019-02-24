//
// Created by veli on 1/22/19.
//

#ifndef TREBLESHOT_TRANSFERGROUPLISTMODEL_H
#define TREBLESHOT_TRANSFERGROUPLISTMODEL_H

#include "src/util/TransferUtils.h"
#include <QtCore/QAbstractListModel>
#include <iostream>
#include <src/util/AppUtils.h>
#include <src/database/object/TransferGroup.h>
#include <QtCore/QDateTime>
#include <src/database/object/NetworkDevice.h>
#include <QIcon>
#include <QtGui/QIconEngine>
#include <src/database/object/TransferObject.h>
#include <QtCore/QMutex>

class TransferGroupModel : public QAbstractTableModel {
Q_OBJECT

    QList<TransferGroupInfo> m_list;
    QMutex m_mutex;

public:
    enum ColumnNames {
        Devices,
        Size,
        Status,
        Date,
        __itemCount
    };

    explicit TransferGroupModel(QObject *parent = nullptr);

    int columnCount(const QModelIndex &parent) const override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    const QList<TransferGroupInfo> &list()
    {
        return m_list;
    }
};


#endif //TREBLESHOT_TRANSFERGROUPLISTMODEL_H
