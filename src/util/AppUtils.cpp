//
// Created by veli on 12/7/18.
//

#include <QtCore/QUuid>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QtWidgets/QMessageBox>
#include <QtNetwork/QNetworkSession>
#include <QtCore/QJsonObject>
#include "AppUtils.h"

void AppUtils::applyAdapterName(DeviceConnection *connection)
{
    QNetworkConfigurationManager manager;

    const QList<QNetworkConfiguration> activeConfigurations
            = manager.allConfigurations(QNetworkConfiguration::StateFlag::Active);

    for (const QNetworkConfiguration &config : activeConfigurations) {
        QNetworkSession session(config);
        QString interfaceName(session.interface().name());

        qDebug() << "Trying for the interface: " << interfaceName;

        for (const QNetworkAddressEntry &address : session.interface().addressEntries()) {
            QHostAddress currentHostAddress = address.ip();

            if (currentHostAddress.toIPv4Address() <= 0)
                continue;

            QString ipV4Address = currentHostAddress.toString();

            qDebug() << "info: " << address.ip().toString();
            qDebug() << ipV4Address.left(ipV4Address.lastIndexOf("."));

            if (ipV4Address.left(ipV4Address.lastIndexOf("."))
                == connection->ipAddress.left(connection->ipAddress.lastIndexOf("."))) {
                qDebug() << "matches";
                connection->adapterName = interfaceName;
                return;
            }
        }
    }

    connection->adapterName = KEYWORD_UNKNOWN_INTERFACE;
}

void AppUtils::applyDeviceToJSON(QJsonObject &object)
{
    NetworkDevice *device = getLocalDevice();
    QJsonObject deviceInfo;
    QJsonObject appInfo;

    deviceInfo.insert(KEYWORD_DEVICE_INFO_SERIAL, device->deviceId);
    deviceInfo.insert(KEYWORD_DEVICE_INFO_BRAND, device->brand);
    deviceInfo.insert(KEYWORD_DEVICE_INFO_MODEL, device->model);
    deviceInfo.insert(KEYWORD_DEVICE_INFO_USER, device->nickname);

    appInfo.insert(KEYWORD_APP_INFO_VERSION_CODE, device->versionNumber);
    appInfo.insert(KEYWORD_APP_INFO_VERSION_NAME, device->versionName);

    object.insert(KEYWORD_APP_INFO, appInfo);
    object.insert(KEYWORD_DEVICE_INFO, deviceInfo);

    delete device;
}

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

NetworkDevice *AppUtils::getLocalDevice()
{
    NetworkDevice *thisDevice = new NetworkDevice(getDeviceId());

    thisDevice->brand = getDeviceTypeName();
    thisDevice->model = getDeviceNameForOS();
    thisDevice->nickname = getUserNickname();
    thisDevice->versionName = getApplicationVersion();
    thisDevice->versionNumber = getApplicationVersionCode();

    return thisDevice;
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
