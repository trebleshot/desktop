#include <utility>

#include <utility>

#ifndef STRINGITEMMODEL_H
#define STRINGITEMMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QStringList>

class StringListModel : public QAbstractListModel {
Q_OBJECT

public:
    explicit StringListModel(QStringList strings, QObject *parent = nullptr)
            : QAbstractListModel(parent), m_stringList(std::move(strings))
    {
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

private:
    QStringList m_stringList;
};

#endif // STRINGITEMMODEL_H
