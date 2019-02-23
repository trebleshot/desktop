//
// Created by veli on 12/7/18.
//

#ifndef TREBLESHOT_APPUTILS_H
#define TREBLESHOT_APPUTILS_H

#include "src/config/Config.h"
#include "src/config/Keyword.h"
#include "src/database/AccessDatabase.h"
#include <QApplication>
#include <QtCore/QSettings>
#include <src/database/object/NetworkDevice.h>

class AppUtils {
public:
    static bool applyAdapterName(DeviceConnection &connection);

    static void applyDeviceToJSON(QJsonObject &object);

    static AccessDatabase *getDatabase();

    static AccessDatabaseSignaller *getDatabaseSignaller();

    static QSettings &getDefaultSettings();

    static QString getDeviceId();

    static NetworkDevice getLocalDevice();

    static AccessDatabase *newDatabaseInstance(QObject *parent = nullptr);
};


#endif //TREBLESHOT_APPUTILS_H
