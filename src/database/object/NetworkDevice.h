//
// Created by veli on 9/25/18.
//

#ifndef TREBLESHOT_NETWORKDEVICE_H
#define TREBLESHOT_NETWORKDEVICE_H

#include <QtNetwork/QHostAddress>
#include <QtCore/QString>
#include <src/database/AccessDatabase.h>

class DeviceConnection;

class NetworkDevice : public DatabaseObject {
public:
    QString brand;
    QString model;
    QString nickname;
    QString deviceId;
    QString versionName;
    int versionNumber = 0;
    int tmpSecureKey = 0;
    time_t lastUsageTime = 0;
    bool isTrusted = false;
    bool isRestricted = false;
    bool isLocalAddress = false;

    explicit NetworkDevice(const QString &deviceId = nullptr);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;
};

class DeviceConnection : public DatabaseObject {
public:
    QString adapterName;
    QHostAddress hostAddress;
    QString deviceId;
    time_t lastCheckedDate = 0;

    explicit DeviceConnection() = default;

    explicit DeviceConnection(const QString &deviceId, const QString &adapterName);

    explicit DeviceConnection(const QHostAddress &hostAddress);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;
};


#endif //TREBLESHOT_NETWORKDEVICE_H
