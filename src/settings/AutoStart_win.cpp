/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
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


} // namespace

bool AutoStart::enabled()
{
	if (isSetByAdmin())
	{
		return true;
	}

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


bool AutoStart::isSetByAdmin()
{
	QSettings settings(QStringLiteral("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"), QSettings::NativeFormat);
	return settings.contains(QCoreApplication::applicationName());
}


void AutoStart::set(bool pEnabled)
{
	QSettings windowsBootUpSettings(registryPath(), QSettings::NativeFormat);

	if (isSetByAdmin() || !pEnabled)
	{
		windowsBootUpSettings.remove(QCoreApplication::applicationName());
	}
	else
	{
		windowsBootUpSettings.setValue(QCoreApplication::applicationName(), appPath());
	}
}
