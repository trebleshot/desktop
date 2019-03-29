//
// Created by veli on 3/3/19.
//

#pragma once

#include <QtCore/QList>
#include <QtCore/QItemSelectionModel>

class ViewUtils {
public:

    static QList<int> getSelectionRows(QItemSelectionModel *model);

    static QList<int> getSelectionRows(const QModelIndexList &index);
};