//
// Created by veli on 3/3/19.
//

#ifndef TREBLESHOT_VIEWUTILS_H
#define TREBLESHOT_VIEWUTILS_H


#include <QtCore/QList>
#include <QtCore/QItemSelectionModel>

class ViewUtils {
public:

    static QList<int> getSelectionRows(QItemSelectionModel *model);

    static QList<int> getSelectionRows(const QModelIndexList &index);
};


#endif //TREBLESHOT_VIEWUTILS_H
