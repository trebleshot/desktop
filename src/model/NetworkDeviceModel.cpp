/*
* Copyright (C) 2019 Veli Tasalı, created on 2/16/19
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <src/util/AppUtils.h>
#include <QtCore/QDateTime>
#include "NetworkDeviceModel.h"

NetworkDeviceModel::NetworkDeviceModel(QObject *parent)
        : QAbstractTableModel(parent), m_list(new QList<NetworkDevice>)
{
    connect(gDatabase, &AccessDatabase::databaseChanged, this, &NetworkDeviceModel::databaseChanged);
    databaseChanged(SqlSelection(), ChangeType::Any);
}

NetworkDeviceModel::~NetworkDeviceModel()
{
	delete m_list;
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