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

class TransferGroupListModel
        : public QAbstractTableModel {
    QList<TransferGroupInfo> *m_list;

public:
    enum ColumnNames {
        Devices,
        Size,
        Status,
        Date
    };

    explicit TransferGroupListModel(QObject *parent = nullptr)
            : QAbstractTableModel(parent), m_list(new QList<TransferGroupInfo>)
    {
        auto *db = AppUtils::getDatabase();
        auto *selection = new SqlSelection;

        selection->setTableName(DbStructure::TABLE_TRANSFERGROUP);

        auto *dbList = db->castQuery(*selection, new TransferGroup());

        for (auto *group : dbList->toStdList()) {
            m_list->append(TransferUtils::getInfo(group));
        }

        delete dbList;
        delete selection;
    }

    ~TransferGroupListModel() override
    {
        delete m_list;
    }

    int columnCount(const QModelIndex &parent) const override
    {
        return sizeof(ColumnNames);
    }

    int rowCount(const QModelIndex &parent) const override
    {
        return m_list->size();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (role == Qt::DisplayRole) {
            if (orientation == Qt::Horizontal) {
                switch (section) {
                    case ColumnNames::Status:
                        return tr("Status");
                    case ColumnNames::Devices:
                        return tr("Devices");
                    case ColumnNames::Date:
                        return tr("Date");
                    case ColumnNames::Size:
                        return tr("Size");
                    default:
                        return QString("?");
                }
            } else
                return QString("%1").arg(section);
        }

        return QVariant();
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role == Qt::DisplayRole) {
            const TransferGroupInfo &currentGroup = m_list->at(index.row());

            switch (index.column()) {
                case ColumnNames::Devices: {
                    QString devicesString;

                    if (currentGroup.assignees.empty())
                        devicesString.append("-");
                    else {
                        for (auto assigneeInfo : currentGroup.assignees) {
                            if (devicesString.length() > 0)
                                devicesString.append(",");

                            devicesString.append(assigneeInfo.device->nickname);
                        }
                    }

                    return devicesString;
                }
                case ColumnNames::Status:
                    return QString("%1 of %2")
                            .arg(currentGroup.completed)
                            .arg(currentGroup.total);
                case ColumnNames::Size:
                    return TransferUtils::sizeExpression(currentGroup.totalBytes, false);
                case ColumnNames::Date:
                    return QDateTime::fromTime_t(static_cast<uint>(currentGroup.group->dateCreated))
                            .toString("ddd, d MMM");
                default:
                    return QString("Data id %1x%2")
                            .arg(index.row())
                            .arg(index.column());
            }
        } else if (role == Qt::DecorationRole) {
            switch (index.column())
                case ColumnNames::Devices:
                    return QIcon(":/icon/arrow_down");
        }

        return QVariant();
    }
};


#endif //TREBLESHOT_TRANSFERGROUPLISTMODEL_H
