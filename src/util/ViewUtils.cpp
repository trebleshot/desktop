//
// Created by veli on 3/3/19.
//

#include "ViewUtils.h"

QList<int> ViewUtils::getSelectionRows(QItemSelectionModel *model)
{
    return getSelectionRows(model->selectedIndexes());
}

QList<int> ViewUtils::getSelectionRows(const QModelIndexList &index)
{
    QList<int> list;

    for (const auto &modelIndex : index) {
        if (!modelIndex.isValid() || modelIndex.column() != 0)
            continue;

        list << modelIndex.row();
    }

    return list;
}
