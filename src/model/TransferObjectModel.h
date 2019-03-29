//
// Created by veli on 3/4/19.
//

#pragma once

#include <QtCore/QAbstractTableModel>
#include <src/database/object/TransferObject.h>
#include <src/util/SynchronizedList.h>
#include <QIcon>

class TransferObjectModel : public QAbstractTableModel, public SynchronizedList<TransferObject> {
Q_OBJECT

public:
    enum ColumnNames {
        FileName,
        Size,
        Status,
        Directory,
        __itemCount
    };

    explicit TransferObjectModel(groupid groupId, const QString &deviceId = QString(), QObject *parent = nullptr);

    int columnCount(const QModelIndex &parent) const override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    void setDeviceId(const QString &deviceId);

public slots:

    void databaseChanged(const SqlSelection &change, ChangeType type);

protected:
    QString m_deviceId;
    groupid m_groupId;
};