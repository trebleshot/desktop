//
// Created by veli on 2/16/19.
//

#pragma once

#include <QtCore/QAbstractTableModel>
#include <src/database/object/NetworkDevice.h>

class NetworkDeviceModel : public QAbstractTableModel {
Q_OBJECT

public:
    enum ColumnNames {
        Name,
        LastUsageDate,
        Status,
        __itemCount // to count his enum
    };

    explicit NetworkDeviceModel(QObject *parent = nullptr);

	~NetworkDeviceModel();

    int columnCount(const QModelIndex &parent) const override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    const QList<NetworkDevice> *list();

public slots:
    void databaseChanged(const SqlSelection& change, ChangeType type);

protected:
    QList<NetworkDevice> *m_list;
};