//
// Created by veli on 12/7/18.
//

#include "AppUtils.h"

AccessDatabase *AppUtils::getDatabase()
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

void AppUtils::loadInfo()
{

}
