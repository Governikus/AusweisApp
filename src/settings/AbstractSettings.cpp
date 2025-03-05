/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */


#include "AbstractSettings.h"

#include "Backup.h"

#include <QCoreApplication>

using namespace governikus;

#ifndef QT_NO_DEBUG
QSharedPointer<QTemporaryDir> AbstractSettings::mTestDir;
#endif

AbstractSettings::AbstractSettings()
	: QObject()
{
}


QString AbstractSettings::getOrganization()
{
#if defined(Q_OS_IOS) || defined(Q_OS_MACOS)
	const auto& organization = QCoreApplication::organizationDomain();

#else
	const auto& organization = QCoreApplication::organizationName();

#endif

	return organization.isEmpty() ? QCoreApplication::applicationName() : organization;
}


void AbstractSettings::save(const QSharedPointer<QSettings>& pSettings)
{
	pSettings->sync();
	Backup::disable(pSettings);
}


QSharedPointer<QSettings> AbstractSettings::getStore(QSettings::Scope pScope, const QString& pFilename, QSettings::Format pFormat)
{
#ifndef QT_NO_DEBUG
	if (QCoreApplication::applicationName().startsWith(QLatin1StringView("Test")))
	{
		if (mTestDir.isNull())
		{
			mTestDir.reset(new QTemporaryDir());
			Q_ASSERT(mTestDir->isValid());
		}
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, mTestDir->path());
		return QSharedPointer<QSettings>::create(QSettings::IniFormat, QSettings::UserScope, getOrganization(), QCoreApplication::applicationName());
	}
#endif

	Q_ASSERT(pFilename.isEmpty() == (pFormat == QSettings::InvalidFormat));
	return pFilename.isEmpty()
			   ? QSharedPointer<QSettings>::create(pScope, getOrganization(), QStringLiteral("AusweisApp2"))
			   : QSharedPointer<QSettings>::create(pFilename, pFormat);
}


QSharedPointer<QSettings> AbstractSettings::getStore(const QString& pFilename, QSettings::Format pFormat)
{
	return getStore(QSettings::Scope::UserScope, pFilename, pFormat);
}
