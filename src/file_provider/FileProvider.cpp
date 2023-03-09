/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "FileProvider.h"

#include "SingletonHelper.h"

#include <QLoggingCategory>
#include <QMutexLocker>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(fileprovider)

defineSingleton(FileProvider)


FileProvider::FileProvider()
	: mUpdatableFiles()
	, mGetFileMutex()
{
}


QSharedPointer<UpdatableFile> FileProvider::getFile(const QString& pSection, const QString& pName, const QString& pDefaultPath)
{
	const QMutexLocker locker(&mGetFileMutex);

	const QString key = pSection + QLatin1Char('/') + pName;
	const QSharedPointer<UpdatableFile> existingF = mUpdatableFiles.value(key, QSharedPointer<UpdatableFile>());
	if (existingF.isNull())
	{
		const QSharedPointer<UpdatableFile> newF(new UpdatableFile(pSection, pName, pDefaultPath), &QObject::deleteLater);
		if (!pName.isEmpty())
		{
			mUpdatableFiles.insert(key, newF);
		}

		return newF;
	}
	else
	{
		existingF->setDefaultPath(pDefaultPath);

		return existingF;
	}
}
