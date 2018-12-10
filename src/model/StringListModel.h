#ifndef STRINGITEMMODEL_H
#define STRINGITEMMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QStringList>

class StringListModel : public QAbstractListModel {
Q_OBJECT

public:
    StringListModel(const QStringList &strings, QObject *parent = 0)
            : QAbstractListModel(parent), stringList(strings)
    {
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role) const;

    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

private:
    QStringList stringList;
};

#endif // STRINGITEMMODEL_H
