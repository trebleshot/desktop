#pragma once

#define TS_SERVICE_NAME "TSComm"
#define TS_SERVICE_TYPE "_tscomm._tcp."
#define PORT_COMMUNICATION_DEFAULT 1128
#define PORT_SEAMLESS 58762
#define TIMEOUT_SOCKET_DEFAULT 5000
#define TIMEOUT_SOCKET_DEFAULT_LARGE 40000
#define BUFFER_LENGTH_DEFAULT 8096
#define NICKNAME_LENGTH_MAX 32
#define URI_APP_HOME "https://github.com/genonbeta/TrebleShot-Desktop"

#include "src/build.h"
#include <QApplication>
#include <QString>

typedef quint32 groupid;
typedef quint32 requestid;

QString getApplicationVersion();

unsigned short getApplicationVersionCode();

QString getDeviceId();

QString getUserNickname();

QString getDeviceTypeName();

QString getDeviceNameForOS();

void initAppEnvironment();