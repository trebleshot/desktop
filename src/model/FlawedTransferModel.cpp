//
// Created by veli on 3/5/19.
//

#include <src/util/AppUtils.h>
#include <src/util/TransferUtils.h>
#include "FlawedTransferModel.h"

FlawedTransferModel::FlawedTransferModel(groupid groupId, QObject *parent)
        : QAbstractTableModel(parent), m_list(new QList<TransferObject>)
{
    m_groupId = groupId;
    connect(gDatabase, &AccessDatabase::databaseChanged, this, &FlawedTransferModel::databaseChanged);
    databaseChanged(SqlSelection(), ChangeType::Any);
}

FlawedTransferModel::~FlawedTransferModel()
{
	delete m_list;
}

int FlawedTransferModel::columnCount(const QModelIndex &parent) const
{
    return ColumnNames::__itemCount;
}

int FlawedTransferModel::rowCount(const QModelIndex &parent) const
{
    return m_list->size();
}

QVariant FlawedTransferModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
                case ColumnNames::Status:
                    return tr("Error");
                case ColumnNames::FileName:
                    return tr("File name");
                default:
                    return QString("?");
            }
        } else
            return QString("%1").arg(section);
    }

    return QVariant();
}

QVariant FlawedTransferModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        const auto &currentObject = m_list->at(index.row());

        switch (index.column()) {
            case ColumnNames::FileName:
                return currentObject.friendlyName;
            case ColumnNames::Status:
                return TransferUtils::getFlagString(currentObject.flag);
            default:
                return QString("Data id %1x%2")
                        .arg(index.row())
                        .arg(index.column());
        }
    } else if (role == Qt::DecorationRole) {
        switch (index.column()) {
            case ColumnNames::FileName: {
                const auto &currentGroup = m_list->at(index.row());
                return QIcon(currentGroup.type == TransferObject::Type::Incoming
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

const QList<TransferObject> *FlawedTransferModel::list() const
{
    return m_list;
}

void FlawedTransferModel::databaseChanged(const SqlSelection &change, ChangeType type)
{
    if (change.valid() && change.tableName != DB_TABLE_TRANSFER && change.tableName != DB_DIVIS_TRANSFER)
        return;

    emit layoutAboutToBeChanged();
	delete m_list;
	
	m_list = new QList<TransferObject>;

    SqlSelection selection;
    selection.setTableName(DB_TABLE_TRANSFER);
    selection.setOrderBy(QString("%1 ASC, %2 ASC")
                                 .arg(DB_FIELD_TRANSFER_NAME)
                                 .arg(DB_FIELD_TRANSFER_DIRECTORY));
    selection.setWhere(QString("%1 = ? AND (%2 = ? OR %3 = ?)")
                               .arg(DB_FIELD_TRANSFER_GROUPID)
                               .arg(DB_FIELD_TRANSFER_FLAG)
                               .arg(DB_FIELD_TRANSFER_FLAG));
    selection.whereArgs << m_groupId
                        << TransferObject::Flag::Removed
                        << TransferObject::Flag::Interrupted;

    gDatabase->castQuery(selection, *m_list);

    if (m_list->empty()) {
        selection.setTableName(DB_DIVIS_TRANSFER);
        gDatabase->castQuery(selection, *m_list);
    }

    emit layoutChanged();
}