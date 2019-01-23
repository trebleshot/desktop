//
// Created by veli on 12/7/18.
//

#ifndef TREBLESHOT_APPUTILS_H
#define TREBLESHOT_APPUTILS_H

#include <src/database/AccessDatabase.h>


class AppUtils {
public:
    static AccessDatabase *getDatabase()
    {
        static AccessDatabase *accessDatabase = nullptr;

        if (accessDatabase == nullptr) {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("local.db");

            if (db.open()) {
                cout << "Database has opened" << endl;

                accessDatabase = new AccessDatabase(&db);
                accessDatabase->initialize();
            }
        }

        return accessDatabase;
    }

    static void loadInfo();
};


#endif //TREBLESHOT_APPUTILS_H
