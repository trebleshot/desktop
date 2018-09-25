//
// Created by veli on 9/25/18.
//

#ifndef TREBLESHOT_NETWORKDEVICE_H
#define TREBLESHOT_NETWORKDEVICE_H


#include <QtCore/QString>
#include <src/database/accessdatabase.h>

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

    NetworkDevice(QString deviceId = nullptr, QObject *parent = nullptr);

    SqlSelection *getWhere();

    QSqlRecord getValues(AccessDatabase *db);

    void onGeneratingValues(QSqlRecord record);
};


#endif //TREBLESHOT_NETWORKDEVICE_H
