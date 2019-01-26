//
// Created by veli on 12/7/18.
//

#ifndef TREBLESHOT_APPUTILS_H
#define TREBLESHOT_APPUTILS_H

#include <src/database/AccessDatabase.h>


class AppUtils {
public:
    static AccessDatabase *getDatabase();

    static void loadInfo();
};


#endif //TREBLESHOT_APPUTILS_H
