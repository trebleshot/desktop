//
// Created by veli on 1/22/19.
//

#include "TransferGroupModel.h"

TransferGroupModel::TransferGroupModel(QObject *parent)
        : QAbstractTableModel(parent), m_list(new QList<TransferGroupInfo *>)
{
    SqlSelection selection;

    selection.setTableName(DbStructure::TABLE_TRANSFERGROUP);
    selection.setOrderBy(DbStructure::FIELD_TRANSFERGROUP_DATECREATED, false);

    auto *dbList = gDatabase->castQuery(selection, TransferGroup());

    for (auto *transferGroup : *dbList) {
        auto copy = TransferUtils::getInfo(*transferGroup);
        m_list->append(&copy);
    }

    delete dbList;
}

TransferGroupModel::~TransferGroupModel()
{
    delete m_list;
}

int TransferGroupModel::columnCount(const QModelIndex &parent) const
{
    return ColumnNames::__itemCount;
}

int TransferGroupModel::rowCount(const QModelIndex &parent) const
{
    return m_list->size();
}

QVariant TransferGroupModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant TransferGroupModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        auto *currentGroup = m_list->at(index.row());

        switch (index.column()) {
            case ColumnNames::Devices: {
                QString devicesString;

                if (currentGroup->assignees.empty())
                    devicesString.append("-");
                else {
                    for (const auto &assigneeInfo : currentGroup->assignees) {
                        if (devicesString.length() > 0)
                            devicesString.append(",");

                        devicesString.append(assigneeInfo.device.nickname);
                    }
                }

                return devicesString;
            }
            case ColumnNames::Status:
                return QString("%1 of %2")
                        .arg(currentGroup->completed)
                        .arg(currentGroup->total);
            case ColumnNames::Size:
                return TransferUtils::sizeExpression(currentGroup->totalBytes, false);
            case ColumnNames::Date:
                return QDateTime::fromTime_t(static_cast<uint>(currentGroup->group.dateCreated))
                        .toString("ddd, d MMM");
            default:
                return QString("Data id %1x%2")
                        .arg(index.row())
                        .arg(index.column());
        }
    } else if (role == Qt::DecorationRole) {
        switch (index.column())
            case ColumnNames::Devices: {
                auto *currentGroup = m_list->at(index.row());
                return QIcon(currentGroup->hasIncoming
                             ? ":/icon/arrow_down"
                             : ":/icon/arrow_up");
            }
    }

    return QVariant();
}
