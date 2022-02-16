/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "AutoStart.h"

#include "AbstractSettings.h"
#include "Env.h"
#include "VolatileSettings.h"

#include <QCoreApplication>

using namespace governikus;


QString AutoStart::appPath()
{
	QString applicationFilePath = QCoreApplication::applicationFilePath();
	applicationFilePath.replace(QLatin1Char('/'), QLatin1Char('\\'));
	applicationFilePath = QLatin1Char('"') + applicationFilePath + QLatin1Char('"');
	return applicationFilePath;
}


QSharedPointer<QSettings> AutoStart::getRegistryStore()
{
	return AbstractSettings::getStore(QStringLiteral("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"), QSettings::NativeFormat);
}


bool AutoStart::enabled()
{
	if (isSetByAdmin())
	{
		return true;
	}

	const auto& windowsBootUpSettings = getRegistryStore();
	return windowsBootUpSettings->contains(QCoreApplication::applicationName())
		   && windowsBootUpSettings->value(QCoreApplication::applicationName(), QString()).toString() == appPath();
}


bool AutoStart::isSetByAdmin()
{
	QSettings settings(QStringLiteral("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"), QSettings::NativeFormat);

	return settings.contains(QCoreApplication::applicationName());
}


bool AutoStart::setInternal(bool pEnabled)
{
	const auto& windowsBootUpSettings = getRegistryStore();

	if (isSetByAdmin() || !pEnabled)
	{
		windowsBootUpSettings->remove(QCoreApplication::applicationName());
	}
	else
	{
		windowsBootUpSettings->setValue(QCoreApplication::applicationName(), appPath());
	}

	return true;
}
