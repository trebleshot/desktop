#ifndef CONFIG_H
#define CONFIG_H

#define PORT_COMMUNICATION_DEFAULT 1128
#define TIMEOUT_SOCKET_DEFAULT 5000

#include "src/build.h"
#include <QApplication>
#include <QString>



QString getApplicationVersion();

unsigned short getApplicationVersionCode();

QString getDeviceId();

QString getDeviceTypeName();

QString getDeviceNameForOS();

#endif // CONFIG_H