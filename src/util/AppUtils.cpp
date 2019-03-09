//
// Created by veli on 12/7/18.
//

#include <QtCore/QUuid>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtWidgets/QMessageBox>
#include <QtNetwork/QNetworkSession>
#include <QtCore/QJsonObject>
#include <QtCore/QStandardPaths>
#include <QtGui/QDesktopServices>
#include <QtCore/QDir>
#include "AppUtils.h"

bool AppUtils::applyAdapterName(DeviceConnection &connection)
{
    quint32 ipv4Address = connection.hostAddress.toIPv4Address();

    if (ipv4Address <= 0)
        return false;

    QNetworkConfigurationManager manager;

    const QList<QNetworkConfiguration> &activeConfigurations
            = manager.allConfigurations(QNetworkConfiguration::StateFlag::Active);

    for (const QNetworkConfiguration &config : activeConfigurations) {
        QNetworkSession session(config);
        const QString &interfaceName(session.interface().name());

        for (const QNetworkAddressEntry &address : session.interface().addressEntries()) {
            quint32 broadcast = address.broadcast().toIPv4Address();

            if (broadcast <= 0)
                continue;

            if (broadcast - 255 < ipv4Address && broadcast >= ipv4Address) {
                connection.adapterName = interfaceName;
                return true;
            }
        }
    }

    connection.adapterName = nullptr;

    return false;
}

void AppUtils::applyDeviceToJSON(QJsonObject &object)
{
    const NetworkDevice &device = getLocalDevice();

    QJsonObject deviceInfo{
            {KEYWORD_DEVICE_INFO_SERIAL, device.id},
            {KEYWORD_DEVICE_INFO_BRAND,  device.brand},
            {KEYWORD_DEVICE_INFO_MODEL,  device.model},
            {KEYWORD_DEVICE_INFO_USER,   device.nickname}
    };

    QJsonObject appInfo{
            {KEYWORD_APP_INFO_VERSION_CODE, device.versionNumber},
            {KEYWORD_APP_INFO_VERSION_NAME, device.versionName}
    };

    object.insert(KEYWORD_APP_INFO, appInfo);
    object.insert(KEYWORD_DEVICE_INFO, deviceInfo);
}

AccessDatabase *AppUtils::getDatabase()
{
    static AccessDatabase *accessDatabase = nullptr;

    if (accessDatabase == nullptr)
        accessDatabase = newDatabaseInstance(QApplication::instance());

    return accessDatabase;
}

AccessDatabaseSignaller *AppUtils::getDatabaseSignaller()
{
    static AccessDatabaseSignaller *signaller = nullptr;

    if (signaller == nullptr)
        signaller = new AccessDatabaseSignaller(getDatabase());

    return signaller;
}

QSettings &AppUtils::getDefaultSettings()
{
    static QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Genonbeta",
                              QApplication::applicationName());

    return settings;
}

NetworkDevice AppUtils::getLocalDevice()
{
    NetworkDevice thisDevice(getDeviceId());

    thisDevice.brand = getDeviceTypeName();
    thisDevice.model = getDeviceNameForOS();
    thisDevice.nickname = getUserNickname();
    thisDevice.versionName = getApplicationVersion();
    thisDevice.versionNumber = getApplicationVersionCode();

    return thisDevice;
}

QString AppUtils::getDeviceId()
{
    QSettings &settings = getDefaultSettings();

    if (!settings.contains("deviceUUID"))
        settings.setValue("deviceUUID", QUuid::createUuid().toString());

    return settings.value("deviceUUID", QString()).toString();
}

AccessDatabase *AppUtils::newDatabaseInstance(QObject *parent)
{
    QSqlDatabase *db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE"));
    const auto &location = QStandardPaths::writableLocation(QStandardPaths::StandardLocation::AppDataLocation);
    QDir saveDir(location);

    if (!saveDir.exists() && !saveDir.mkdir(location))
        return nullptr;

    db->setDatabaseName(saveDir.filePath("default.db"));

    if (db->open()) {
        cout << "Database has opened" << endl;

        auto *database = new AccessDatabase(db, parent);
        database->initialize();

        return database;
    }

    return nullptr;
}

TransferTaskManager *AppUtils::getTransferTaskManager()
{
    static auto *taskManager = new TransferTaskManager;
    return taskManager;
}