//
// Created by veli on 9/25/18.
//

#ifndef TREBLESHOT_NETWORKDEVICE_H
#define TREBLESHOT_NETWORKDEVICE_H


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
    int lastUsageTime;
    bool isTrusted = false;
    bool isRestricted = false;
    bool isLocalAddress = false;

    explicit NetworkDevice(QString deviceId = nullptr, QObject *parent = nullptr);

    SqlSelection *getWhere() override;

    QSqlRecord getValues(AccessDatabase *db) override;

    void onGeneratingValues(QSqlRecord record) override;
};

class DeviceConnection : public DatabaseObject {
public:
    QString adapterName;
    QString ipAddress;
    QString deviceId;
    unsigned long long lastCheckedDate;

    explicit DeviceConnection(QObject *parent = nullptr);

    explicit DeviceConnection(QString deviceId, QString adapterName, QObject *parent = nullptr);

    explicit DeviceConnection(QString ipAddress, QObject *parent = nullptr);

    SqlSelection *getWhere() override;

    QSqlRecord getValues(AccessDatabase *db) override;

    void onGeneratingValues(QSqlRecord record) override;
};


#endif //TREBLESHOT_NETWORKDEVICE_H
