//
// Created by veli on 2/16/19.
//

#include <src/util/AppUtils.h>
#include <QtCore/QDateTime>
#include "NetworkDeviceModel.h"

NetworkDeviceModel::NetworkDeviceModel(QObject *parent)
        : QAbstractTableModel(parent), m_list(new QList<NetworkDevice>)
{
    connect(gDatabase, &AccessDatabase::databaseChanged, this, &NetworkDeviceModel::databaseChanged);
    databaseChanged(SqlSelection(), ChangeType::Any);
}

int NetworkDeviceModel::columnCount(const QModelIndex &parent) const
{
    return ColumnNames::__itemCount;
}

int NetworkDeviceModel::rowCount(const QModelIndex &parent) const
{
    return m_list->size();
}

void NetworkDeviceModel::databaseChanged(const SqlSelection &change, ChangeType changeType)
{
    if (change.valid() && change.tableName != DB_TABLE_DEVICES)
        return;

    emit layoutAboutToBeChanged();
	delete m_list;
	m_list = new QList<NetworkDevice>;

    SqlSelection selection;
    selection.setTableName(DB_TABLE_DEVICES);
    selection.setOrderBy(DB_FIELD_DEVICES_LASTUSAGETIME, false);

    gDatabase->castQuery(selection, *m_list);

    emit layoutChanged();
}

QVariant NetworkDeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
                case ColumnNames::Status:
                    return tr("Status");
                case ColumnNames::Name:
                    return tr("Name");
                case ColumnNames::LastUsageDate:
                    return tr("Last usage");
                default:
                    return QString("?");
            }
        } else
            return QString("%1").arg(section);
    }

    return QVariant();
}

QVariant NetworkDeviceModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        const auto &thisDevice = m_list->at(index.row());

        switch (index.column()) {
            case ColumnNames::Name:
                return thisDevice.nickname;
            case ColumnNames::Status:
                return thisDevice.isRestricted
                       ? QString("Restricted")
                       : QString("Normal");
            case ColumnNames::LastUsageDate:
                return QDateTime::fromTime_t(static_cast<uint>(thisDevice.lastUsageTime))
                        .toString("ddd, d MMM");
            default:
                return QString("Data id %1x%2")
                        .arg(index.row())
                        .arg(index.column());
        }
    }

    return QVariant();
}

const QList<NetworkDevice> *NetworkDeviceModel::list()
{
    return m_list;
}