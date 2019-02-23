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
Q_OBJECT

public:
    QString brand;
    QString model;
    QString nickname;
    QString deviceId;
    QString versionName;
    int versionNumber;
    int tmpSecureKey;
    time_t lastUsageTime;
    bool isTrusted = false;
    bool isRestricted = false;
    bool isLocalAddress = false;

    NetworkDevice(const NetworkDevice &device)
    {

    }

    explicit NetworkDevice(const QString &deviceId = nullptr, QObject *parent = nullptr);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const QSqlRecord &record) override;

    void operator=(const NetworkDevice& other) {
        brand = other.brand;
    }
};

class DeviceConnection : public DatabaseObject {
public:
    QString adapterName;
    QHostAddress hostAddress;
    QString deviceId;
    time_t lastCheckedDate;

    explicit DeviceConnection(QObject *parent = nullptr);

    explicit DeviceConnection(const QString &deviceId, const QString &adapterName, QObject *parent = nullptr);

    explicit DeviceConnection(const QHostAddress &hostAddress, QObject *parent = nullptr);

    SqlSelection getWhere() const override;

    DbObjectMap getValues() const override;

    void onGeneratingValues(const QSqlRecord &record) override;
};


#endif //TREBLESHOT_NETWORKDEVICE_H
