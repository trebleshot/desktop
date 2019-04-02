/*
* Copyright (C) 2019 Veli Tasalı
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <src/util/AppUtils.h>

QString getDeviceTypeName()
{
	return QString("Computer");
}

QString getApplicationVersion()
{
	return QApplication::applicationVersion();
}

unsigned short getApplicationVersionCode()
{
	return TREBLESHOT_VERSION_CODE;
}

QString getDeviceNameForOS()
{
#if defined(Q_OS_ANDROID)
	return QLatin1String("Android");
#elif defined(Q_OS_BLACKBERRY)
	return QLatin1String("Blackberry");
#elif defined(Q_OS_IOS)
	return QLatin1String("iOS");
#elif defined(Q_OS_MACOS)
	return QLatin1String("MacOS");
#elif defined(Q_OS_TVOS)
	return QLatin1String("TvOS");
#elif defined(Q_OS_WATCHOS)
	return QLatin1String("WatchOS");
#elif defined(Q_OS_WINCE)
	return QLatin1String("WinCE");
#elif defined(Q_OS_WIN)
	return QLatin1String("Windows");
#elif defined(Q_OS_LINUX)
	return QLatin1String("Linux");
#elif defined(Q_OS_UNIX)
	return QLatin1String("Unix");
#else
	return QLatin1String("unknown");
#endif
}

QString getDeviceId()
{
	return AppUtils::getDeviceId();
}

QString getUserNickname()
{
	return AppUtils::getDefaultSettings()
			.value("nickname", QString("TrebleShot on %1").arg(getDeviceNameForOS()))
			.toString();
}

void initAppEnvironment()
{
	qRegisterMetaType<groupid>("groupid");
	qRegisterMetaType<requestid>("requestid");
	qRegisterMetaType<TransferObject>("TransferObject");
	qRegisterMetaType<QList<NetworkDevice>>("QList<NetworkDevice>");
	qRegisterMetaType<QHostAddress>("QHostAddress");
}
