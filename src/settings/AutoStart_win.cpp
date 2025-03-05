/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "AutoStart.h"

#include "AbstractSettings.h"

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


bool AutoStart::removeOldAutostart()
{
	const auto& windowsBootUpSettings = getRegistryStore();
	if (windowsBootUpSettings->contains(QStringLiteral("AusweisApp2")))
	{
		qDebug() << "Detected old autostart, migrating old autostart setting...";
		windowsBootUpSettings->remove(QStringLiteral("AusweisApp2"));
		return true;
	}
	return false;
}
