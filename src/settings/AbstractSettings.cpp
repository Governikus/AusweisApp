/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
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


void AbstractSettings::save(const QSharedPointer<QSettings>& pSettings)
{
	pSettings->sync();
	Backup::disable(pSettings);
}


QSharedPointer<QSettings> AbstractSettings::getStore(const QString& pFilename, QSettings::Format pFormat)
{
#ifndef QT_NO_DEBUG
	if (QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		if (mTestDir.isNull())
		{
			mTestDir.reset(new QTemporaryDir());
			Q_ASSERT(mTestDir->isValid());
		}
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, mTestDir->path());
		return QSharedPointer<QSettings>::create(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QStringLiteral("AusweisApp2"));
	}
#endif

	Q_ASSERT(pFilename.isEmpty() == (pFormat == QSettings::InvalidFormat));
#if defined(Q_OS_IOS) || defined(Q_OS_MACOS)
	const auto& organization = QCoreApplication::organizationDomain();

#else
	const auto& organization = QCoreApplication::organizationName();

#endif

	return pFilename.isEmpty() ? QSharedPointer<QSettings>::create(organization, QStringLiteral("AusweisApp2")) : QSharedPointer<QSettings>::create(pFilename, pFormat);
}
