#ifndef CONFIG_H
#define CONFIG_H

#define PORT_COMMUNICATION_DEFAULT 1128
#define TIMEOUT_SOCKET_DEFAULT 5000
#define TIMEOUT_SOCKET_DEFAULT_LARGE 40000
#define NICKNAME_LENGTH_MAX 32
#define URI_APP_HOME "https://github.com/genonbeta/TrebleShot-Desktop"

#include "src/build.h"
#include <QApplication>
#include <QString>

QString getApplicationVersion();

unsigned short getApplicationVersionCode();

QString getDeviceId();

QString getUserNickname();

QString getDeviceTypeName();

QString getDeviceNameForOS();

#endif // CONFIG_H