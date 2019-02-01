//
// Created by veli on 12/7/18.
//

#include <QtCore/QUuid>
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

QSettings &AppUtils::getDefaultSettings()
{
    static QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Genonbeta",
                              QApplication::applicationName());

    return settings;
}

QString AppUtils::getDeviceId()
{
    QSettings &settings = getDefaultSettings();

    if (!settings.contains("deviceUUID"))
        settings.setValue("deviceUUID", QUuid::createUuid().toString());

    return settings.value("deviceUUID", QString()).toString();
}

void AppUtils::loadInfo()
{

}
