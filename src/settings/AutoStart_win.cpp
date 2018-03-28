/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "AutoStart.h"

#include <QCoreApplication>
#include <QSettings>
#include <QString>

using namespace governikus;

namespace
{

static QString appPath()
{
	QString applicationFilePath = QCoreApplication::applicationFilePath();
	applicationFilePath.replace(QLatin1Char('/'), QLatin1Char('\\'));
	applicationFilePath = QLatin1Char('"') + applicationFilePath + QLatin1Char('"');
	return applicationFilePath;
}


static QString registryPath()
{
	return QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run");
}


}

bool AutoStart::enabled()
{
	QSettings windowsBootUpSettings(registryPath(), QSettings::NativeFormat);
	if (!windowsBootUpSettings.contains(QCoreApplication::applicationName()))
	{
		return false;
	}

	if (windowsBootUpSettings.value(QCoreApplication::applicationName(), QString()).toString() != appPath())
	{
		// Update entries from previous versions, e.g.
		// C:\Program Files (x86)\AusweisApp2 1.13.0 -> C:\Program Files (x86)\AusweisApp2 1.14.0
		set(true);
	}

	return true;
}


void AutoStart::set(bool pEnabled)
{
	QSettings windowsBootUpSettings(registryPath(), QSettings::NativeFormat);

	if (pEnabled)
	{
		windowsBootUpSettings.setValue(QCoreApplication::applicationName(), appPath());
	}
	else
	{
		windowsBootUpSettings.remove(QCoreApplication::applicationName());
	}
}
