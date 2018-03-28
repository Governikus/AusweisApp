/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "UpdatableFile.h"

#include "Downloader.h"
#include "Env.h"
#include "SecureStorage.h"

#include <QDir>
#include <QFile>
#include <QLoggingCategory>
#include <QStandardPaths>

#ifndef QT_NO_DEBUG
#include <QTemporaryDir>
#endif

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(fileprovider)

namespace
{
const QLatin1Char Sep('/');
}


const QString& UpdatableFile::getName()
{
	return mName;
}


QDateTime UpdatableFile::cacheTimestamp() const
{
	const QString timestampFormat = QStringLiteral("yyyyMMddhhmmss");
	const int timestampFormatSize = timestampFormat.size();
	const QString pathInCache = cachePath();

	// The path must contain at least 1 character for the filename,
	// the '_' separator, and the timestamp.
	if (pathInCache.size() < 2 + timestampFormatSize)
	{
		return QDateTime();
	}

	return QDateTime::fromString(pathInCache.right(timestampFormatSize), timestampFormat);
}


const QString& UpdatableFile::getSectionCachePath() const
{
	return mSectionCachePath;
}


QString UpdatableFile::qrcPath() const
{
	const QString prefix = QStringLiteral("updatable-files");
	const QString path = QStringLiteral(":/") + prefix + Sep + mSection + Sep + mName;

	return QFile::exists(path) ? path : QString();
}


QString UpdatableFile::cachePath() const
{
	QDir folder(mSectionCachePath);
	const QStringList nameFilter = QStringList({mName + QStringLiteral("_*")});
	const QStringList matchingFiles = folder.entryList(nameFilter, QDir::Files, QDir::Name | QDir::Reversed);

	// Files are saved in the cache with the suffix _<timestamp>, where
	// the timestamp has the format "yyyyMMddhhmmss".
	// Therefore, the first matching file in the list has the newest timestamp.
	return matchingFiles.isEmpty() ? QString() : mSectionCachePath + Sep + matchingFiles.first();
}


QUrl UpdatableFile::updateUrl(const QString& pSection, const QString& pName)
{
	const QUrl updateServerBaseUrl = SecureStorage::getInstance().getUpdateServerBaseUrl();

	const QString section = pSection.isEmpty() ? QString() : Sep + pSection;
	return QUrl(updateServerBaseUrl.toString() + section + Sep + pName);
}


QString UpdatableFile::dirtyFilePath() const
{
	return mSectionCachePath.isEmpty() ? QString() : mSectionCachePath + Sep + mName + QStringLiteral(".dirty");
}


QString UpdatableFile::sectionCachePath(const QString& pSection) const
{
	const QStringList cachePaths = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
	if (cachePaths.isEmpty())
	{
		qCWarning(fileprovider) << "No cache paths found!";

		return QString();
	}

	const QString cacheBasePath = cachePaths.first();
	if (cacheBasePath.isEmpty())
	{
		qCWarning(fileprovider) << "Cache base folder is invalid (empty).";

		return QString();
	}

	return cacheBasePath + Sep + pSection;
}


QString UpdatableFile::makeSectionCachePath(const QString& pSection)
{
	QString sectionFolderPath = sectionCachePath(pSection);

#ifndef QT_NO_DEBUG
	if (QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		static QTemporaryDir testDir;
		Q_ASSERT(testDir.isValid());
		sectionFolderPath = testDir.path() + Sep + pSection;
	}
#endif

	const QDir sectionFolder(sectionFolderPath);
	if (sectionFolder.exists())
	{
		return sectionFolderPath;
	}

	if (sectionFolder.mkpath(sectionFolderPath))
	{
		qCDebug(fileprovider) << "Created cache folder:" << sectionFolderPath;

		return sectionFolderPath;
	}

	qCWarning(fileprovider) << "Cannot create cache folder:" << sectionFolderPath;

	return QString();
}


void UpdatableFile::cleanupAfterUpdate(const std::function<void()>& pCustomAction)
{
	Downloader* const downloader = Env::getSingleton<Downloader>();
	disconnect(downloader, &Downloader::fireDownloadSuccess, this, &UpdatableFile::onDownloadSuccess);
	disconnect(downloader, &Downloader::fireDownloadFailed, this, &UpdatableFile::onDownloadFailed);
	disconnect(downloader, &Downloader::fireDownloadUnnecessary, this, &UpdatableFile::onDownloadUnnecessary);

	pCustomAction();

	mUpdateRunning = false;
}


void UpdatableFile::onDownloadSuccess(const QUrl& pUpdateUrl, const QDateTime& pNewTimestamp, const QByteArray& pData)
{
	if (pUpdateUrl == mUpdateUrl)
	{
		const QString dateFormat = QStringLiteral("yyyyMMddhhmmss");
		const QString filePath = mSectionCachePath + Sep + mName + QLatin1Char('_') + pNewTimestamp.toString(dateFormat);

		if (writeDataToFile(pData, filePath))
		{
			Q_EMIT fireUpdated();
		}
		else
		{
			qCCritical(fileprovider) << "Could not write downloaded file" << filePath;
		}

		cleanupAfterUpdate([&](){
					clearDirty();
				});
	}
}


void UpdatableFile::onDownloadFailed(const QUrl& pUpdateUrl, GlobalStatus::Code pErrorCode)
{
	if (pUpdateUrl == mUpdateUrl)
	{
		cleanupAfterUpdate([ = ](){
					qCCritical(fileprovider) << "Download failed with error code:" << pErrorCode;
				});
	}
}


void UpdatableFile::onDownloadUnnecessary(const QUrl& pUpdateUrl)
{
	if (pUpdateUrl == mUpdateUrl)
	{
		cleanupAfterUpdate([&](){
					clearDirty();
				});
	}
}


bool UpdatableFile::writeDataToFile(const QByteArray& pData, const QString& pFilePath, bool pOverwrite)
{
	QFile file(pFilePath);
	if (!pOverwrite && file.exists())
	{
		qCCritical(fileprovider) << "File already exists, aborting writing file:" << pFilePath;
		return false;
	}
	if (!file.open(QIODevice::WriteOnly))
	{
		qCCritical(fileprovider) << "File could not be opened for writing:" << pFilePath;
		return false;
	}
	if (file.write(pData) != pData.size())
	{
		qCCritical(fileprovider) << "Not all data could not be written to file:" << pFilePath;
		file.close();
		return false;
	}

	qCDebug(fileprovider) << "Data written to file:" << pFilePath;
	file.close();
	return true;
}


UpdatableFile::UpdatableFile(const QString& pSection, const QString& pName, const QString& pDefaultPath)
	: mSection(pSection)
	, mName(pName)
	, mDefaultPath(pDefaultPath)
	, mSectionCachePath(makeSectionCachePath(pSection))
	, mUpdateUrl(updateUrl(pSection, pName))
	, mUpdateRunning(false)
{
}


QUrl UpdatableFile::lookupUrl()
{
	QString path = lookupPath();
	if (path.startsWith(QLatin1String(":/")))
	{
		return QStringLiteral("qrc://") + path.midRef(1);
	}
	else
	{
		return QStringLiteral("file://") + path;
	}
}


QString UpdatableFile::lookupPath()
{
	if (mName.isEmpty())
	{
		return mDefaultPath;
	}

	QString result;
	const QString pathInCache = cachePath();
	if (pathInCache.isEmpty())
	{
		const QString pathInQrc = qrcPath();
		result = pathInQrc.isEmpty() ? mDefaultPath : pathInQrc;
	}
	else
	{
		result = pathInCache;
	}

	if (isDirty())
	{
		update();
	}

	return result;
}


void UpdatableFile::setDefaultPath(const QString& pPath)
{
	if (mDefaultPath != pPath)
	{
		mDefaultPath = pPath;
	}
}


const QString& UpdatableFile::getDefaultPath() const
{
	return mDefaultPath;
}


void UpdatableFile::update()
{
	qCDebug(fileprovider) << "Update Triggered for Name:" << mName << " with URL:" << mUpdateUrl;
	if (!mUpdateRunning && !mName.isEmpty())
	{
		mUpdateRunning = true;

		Downloader* const downloader = Env::getSingleton<Downloader>();
		connect(downloader, &Downloader::fireDownloadSuccess, this, &UpdatableFile::onDownloadSuccess);
		connect(downloader, &Downloader::fireDownloadFailed, this, &UpdatableFile::onDownloadFailed);
		connect(downloader, &Downloader::fireDownloadUnnecessary, this, &UpdatableFile::onDownloadUnnecessary);

		const QDateTime timestamp = cacheTimestamp();
		if (timestamp.isValid())
		{
			downloader->downloadIfNew(mUpdateUrl, timestamp);
		}
		else
		{
			downloader->download(mUpdateUrl);
		}
	}
}


bool UpdatableFile::isDirty() const
{
	if (mName.isEmpty())
	{
		return false;
	}

	return QFile::exists(dirtyFilePath());
}


void UpdatableFile::clearDirty() const
{
	if (mSectionCachePath.isEmpty() && !mName.isEmpty())
	{
		qCCritical(fileprovider) << "Cannot clear invalid file:" << mSection << Sep << mName;

		return;
	}

	const QString filePath = dirtyFilePath();
	QFile file(filePath);
	if (file.exists() && !file.remove())
	{
		qCCritical(fileprovider) << "Cannot remove file:" << filePath;
	}
}


void UpdatableFile::markDirty() const
{
	if (mSectionCachePath.isEmpty() && !mName.isEmpty())
	{
		qCCritical(fileprovider) << "Cannot mark invalid file:" << mSection << Sep << mName;

		return;
	}

	const QString filePath = dirtyFilePath();
	QFile file(filePath);
	if (!file.exists())
	{
		if (!file.open(QIODevice::WriteOnly))
		{
			qCCritical(fileprovider) << "Cannot create file:" << filePath;
		}

		file.close();
	}
}
