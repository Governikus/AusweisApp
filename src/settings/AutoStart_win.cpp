/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "AutoStart.h"

#include <QCoreApplication>
#include <QSettings>
#include <QString>


using namespace governikus;


bool AutoStart::enabled()
{
	QSettings windowsBootUpSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
	return windowsBootUpSettings.contains(QCoreApplication::applicationName());
}


void AutoStart::set(bool pEnabled)
{
	QSettings windowsBootUpSettings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

	if (pEnabled)
	{
		QString applicationFilePath = QCoreApplication::applicationFilePath();
		applicationFilePath.replace(QStringLiteral("/"), QString("\\"));
		windowsBootUpSettings.setValue(QCoreApplication::applicationName(), "\"" + applicationFilePath + "\"");
	}
	else
	{
		windowsBootUpSettings.remove(QCoreApplication::applicationName());
	}
}
