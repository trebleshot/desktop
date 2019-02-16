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

class TransferGroupModel
        : public QAbstractTableModel {
    QList<TransferGroupInfo> *m_list;

public:
    enum ColumnNames {
        Devices,
        Size,
        Status,
        Date,
        __itemCount
    };

    explicit TransferGroupModel(QObject *parent = nullptr);

    ~TransferGroupModel() override;

    int columnCount(const QModelIndex &parent) const override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex &index, int role) const override;
};


#endif //TREBLESHOT_TRANSFERGROUPLISTMODEL_H
