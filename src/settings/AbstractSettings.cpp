/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */


#include "AbstractSettings.h"

#include <QCoreApplication>

using namespace governikus;

#ifndef QT_NO_DEBUG
QSharedPointer<QTemporaryDir> AbstractSettings::mTestDir;
#endif

AbstractSettings::AbstractSettings()
	: QObject()
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
			mTestDir.reset(new QTemporaryDir());
			Q_ASSERT(mTestDir->isValid());
		}
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, mTestDir->path());
		return QSharedPointer<QSettings>::create(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
	}
#endif
	return QSharedPointer<QSettings>::create();
}
