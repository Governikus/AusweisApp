/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */


#include "AbstractSettings.h"

#include <QCoreApplication>

using namespace governikus;

#ifndef QT_NO_DEBUG
QSharedPointer<QTemporaryDir> AbstractSettings::mTestDir;
#endif

AbstractSettings::AbstractSettings()
{
}


AbstractSettings::~AbstractSettings()
{
}


QSharedPointer<QSettings> AbstractSettings::getStore()
{
#ifndef QT_NO_DEBUG
	if (QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		if (mTestDir.isNull())
		{
			mTestDir.reset(new QTemporaryDir);
			Q_ASSERT(mTestDir->isValid());
		}
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, mTestDir->path());
		return QSharedPointer<QSettings>(new QSettings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName()));
	}
#endif
	return QSharedPointer<QSettings>(new QSettings(QCoreApplication::organizationName(), QCoreApplication::applicationName()));
}


void AbstractSettings::update(const AbstractSettings&)
{
	Q_ASSERT_X(false, "update", "method should be overridden by subclass");
}


QVariant AbstractSettings::getSettingsValue(const QSharedPointer<QSettings>& pSettings, const QString& key)
{
	if (pSettings)
	{
		return pSettings->contains(key) ? pSettings->value(key) : pSettings->value(key.toLower());
	}
	else
	{
		return QVariant();
	}
}
