//
// Created by veli on 1/22/19.
//

#ifndef TREBLESHOT_TRANSFERGROUPLISTMODEL_H
#define TREBLESHOT_TRANSFERGROUPLISTMODEL_H

#include <QtCore/QAbstractListModel>
#include <iostream>
#include <src/util/AppUtils.h>
#include <src/database/object/TransferGroup.h>
#include <QtCore/QDateTime>
#include <src/database/object/NetworkDevice.h>

class TransferGroupListModel
        : public QAbstractListModel {

private:
    QList<TransferGroup *> *m_list;
    QList<QString> m_columnNames{
            tr("Devices"),
            tr("Size"),
            tr("Status"),
            tr("Date"),
    };

public:
    explicit TransferGroupListModel(QObject *parent = nullptr)
            : QAbstractListModel(parent)
    {
        auto *db = AppUtils::getDatabase();
        auto *selection = new SqlSelection;

        selection->setTableName(AccessDatabaseStructure::TABLE_TRANSFERGROUP);

        m_list = db->castQuery(*selection, new TransferGroup());
    }

    ~TransferGroupListModel() override
    {
        delete m_list;
    }

    int columnCount(const QModelIndex &parent) const override
    {
        return m_columnNames.size();
    }

    int rowCount(const QModelIndex &parent) const override
    {
        return m_list->size();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override
    {
        if (role != Qt::DisplayRole)
            return QVariant();

        if (orientation == Qt::Horizontal)
            return m_columnNames[section];
        else
            return QString("%1").arg(section);
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role == Qt::DisplayRole) {
            const TransferGroup *currentGroup = m_list->at(index.row());

            switch (index.column()) {
                case 0: {
                    auto *selection = new SqlSelection();

                    selection->setTableName(AccessDatabaseStructure::TABLE_TRANSFERASSIGNEE)
                            ->setWhere(QString("`%1` = ?").arg(AccessDatabaseStructure::FIELD_TRANSFERASSIGNEE_GROUPID));
                    selection->whereArgs << currentGroup->groupId;

                    QList<TransferAssignee *> *assigneeList = AppUtils::getDatabase()->castQuery(*selection, new TransferAssignee());
                    QString devicesString;

                    for (const TransferAssignee *assignee : assigneeList->toStdList()) {
                        try {
                            auto *device = new NetworkDevice(assignee->deviceId);

                            AppUtils::getDatabase()->reconstruct(device);

                            if (devicesString.length() > 0)
                                devicesString.append(", ");

                            devicesString.append(device->nickname);

                            delete device;
                        } catch (...) {
                            // We will not add this device to the list.
                            std::cout << "An assignee failed to reconstruct";
                        }
                    }

                    if (devicesString.length() == 0)
                        devicesString.append("-");

                    delete assigneeList;

                    return devicesString;
                }
                case 1: {

                }
                case 2:
                case 3:
                    return QDateTime::fromMSecsSinceEpoch(currentGroup->dateCreated)
                            .toString(Qt::DateFormat::SystemLocaleShortDate);
                default:
                    return QString("Data id %1x%2")
                            .arg(index.row())
                            .arg(index.column());
            }
        }

        return QVariant();
    }
};


#endif //TREBLESHOT_TRANSFERGROUPLISTMODEL_H
