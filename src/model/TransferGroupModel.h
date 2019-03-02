//
// Created by veli on 1/22/19.
//

#ifndef TREBLESHOT_TRANSFERGROUPLISTMODEL_H
#define TREBLESHOT_TRANSFERGROUPLISTMODEL_H

#include "src/util/TransferUtils.h"
#include <iostream>
#include <src/database/AccessDatabase.h>
#include <src/database/object/TransferGroup.h>
#include <src/database/object/TransferObject.h>
#include <src/database/object/NetworkDevice.h>
#include <src/util/AppUtils.h>
#include <QtCore/QAbstractListModel>
#include <QtCore/QDateTime>
#include <QIcon>
#include <QtGui/QIconEngine>
#include <QtCore/QMutex>

class TransferGroupModel : public QAbstractTableModel {
Q_OBJECT

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

public slots:

    void databaseChanged(const SqlSelection &change, ChangeType changeType);

protected:
    QList<TransferGroupInfo> m_list;
    QMutex m_mutex;
};


#endif //TREBLESHOT_TRANSFERGROUPLISTMODEL_H
