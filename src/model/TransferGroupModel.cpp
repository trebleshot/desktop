//
// Created by veli on 1/22/19.
//

#include "TransferGroupModel.h"

TransferGroupModel::TransferGroupModel(QObject *parent)
        : QAbstractTableModel(parent), m_list(new QList<TransferGroupInfo>)
{
    connect(gDatabase, &AccessDatabase::databaseChanged, this, &TransferGroupModel::databaseChanged);
    databaseChanged(SqlSelection(), ChangeType::Any);
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
        const auto &currentGroup = m_list->at(index.row());

        switch (index.column()) {
            case ColumnNames::Devices: {
                QString devicesString;

                if (currentGroup.assignees.empty())
                    devicesString.append("-");
                else {
                    for (const auto &assigneeInfo : currentGroup.assignees) {
                        if (devicesString.length() > 0)
                            devicesString.append(", ");

                        devicesString.append(assigneeInfo.device.nickname);
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
                return QDateTime::fromTime_t(static_cast<uint>(currentGroup.group.dateCreated))
                        .toString("ddd, d MMM");
            default:
                return QString("Data id %1x%2")
                        .arg(index.row())
                        .arg(index.column());
        }
    } else if (role == Qt::DecorationRole) {
        switch (index.column()) {
            case ColumnNames::Devices: {
                const auto &currentGroup = m_list->at(index.row());
                return QIcon(currentGroup.hasIncoming
                             ? ":/icon/arrow_down"
                             : ":/icon/arrow_up");
            }
            default: {
                // do nothing
            }
        }
    }

    return QVariant();
}

void TransferGroupModel::databaseChanged(const SqlSelection &change, ChangeType changeType)
{
    if (change.valid() && change.tableName != DB_TABLE_TRANSFERGROUP && change.tableName != DB_TABLE_TRANSFER)
        return;

    emit layoutAboutToBeChanged();
	delete m_list;
	m_list = new QList<TransferGroupInfo>;

	QList<TransferGroup> groupList;
    SqlSelection selection;
    selection.setTableName(DB_TABLE_TRANSFERGROUP);
    selection.setOrderBy(DB_FIELD_TRANSFERGROUP_DATECREATED, false);
	
	gDatabase->castQuery(selection, groupList);

    for (const auto &transferGroup : groupList)
        m_list->append(TransferUtils::getInfo(transferGroup));

    emit layoutChanged();
}

const QList<TransferGroupInfo> *TransferGroupModel::list() const
{
    return m_list;
}
