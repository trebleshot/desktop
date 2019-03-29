//
// Created by veli on 9/25/18.
//

#pragma once

#include <QtNetwork/QHostAddress>
#include <QtCore/QString>
#include <src/database/AccessDatabase.h>

class DeviceConnection;

class NetworkDevice : public DatabaseObject {
public:
    QString id;
    QString brand;
    QString model;
    QString nickname;
    QString versionName;
    int versionNumber = 0;
    int tmpSecureKey = 0;
    time_t lastUsageTime = 0;
    bool isTrusted = false;
    bool isRestricted = false;
    bool isLocalAddress = false;

    explicit NetworkDevice(const QString &id = nullptr);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;

    void onRemovingObject(AccessDatabase *db, DatabaseObject *parent) override;
};

class DeviceConnection : public DatabaseObject {
public:
    QString adapterName;
    QHostAddress hostAddress;
    QString deviceId;
    time_t lastCheckedDate = 0;

    DeviceConnection() = default;

    explicit DeviceConnection(const QString &deviceId, const QString &adapterName);

    explicit DeviceConnection(const QHostAddress &hostAddress);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const DbObjectMap &record) override;
};