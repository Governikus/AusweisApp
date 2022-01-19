/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "UpdatableFile.h"

#include "Downloader.h"
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
} // namespace


const QString& UpdatableFile::getName() const
{
	return mName;
}


QDateTime UpdatableFile::cacheTimestamp() const
{
	const QString timestampFormat = QStringLiteral("yyyyMMddhhmmss");
	const auto timestampFormatSize = timestampFormat.size();
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


QUrl UpdatableFile::updateUrl(const QString& pSection, const QString& pName) const
{
	const QUrl updateServerBaseUrl = Env::getSingleton<SecureStorage>()->getUpdateServerBaseUrl();

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


QString UpdatableFile::makeSectionCachePath(const QString& pSection) const
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
	const auto* const downloader = Env::getSingleton<Downloader>();
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

		cleanupAfterUpdate([this](){
				clearDirty();
			});
	}
}


void UpdatableFile::onDownloadFailed(const QUrl& pUpdateUrl, GlobalStatus::Code pErrorCode)
{
	if (pUpdateUrl == mUpdateUrl)
	{
		cleanupAfterUpdate([pErrorCode](){
				qCCritical(fileprovider) << "Download failed with error code:" << pErrorCode;
			});
	}
}


void UpdatableFile::onDownloadUnnecessary(const QUrl& pUpdateUrl)
{
	if (pUpdateUrl == mUpdateUrl)
	{
		Q_EMIT fireNoUpdateAvailable();

		cleanupAfterUpdate([this](){
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
		qCCritical(fileprovider) << "Not all data could be written to file:" << pFilePath;
		file.close();
		file.remove();
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
	if (mName.isEmpty() && mDefaultPath.isEmpty())
	{
		qCWarning(fileprovider) << "Both name and default path are empty!";
		Q_ASSERT(false);
	}
}


QUrl UpdatableFile::lookupUrl()
{
	QString path = lookupPath();
	if (path.startsWith(QLatin1String(":/")))
	{
		return QStringLiteral("qrc://") + path.remove(0, 1);
	}
	else
	{
		return QStringLiteral("file:///") + path;
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


bool UpdatableFile::forEachLookupPath(const std::function<bool(const QString&)>& pValidate)
{
	QStringList paths;
	if (!mDefaultPath.isEmpty())
	{
		paths += mDefaultPath;
	}

	if (!mName.isEmpty())
	{
		const QString pathInQrc = qrcPath();
		if (!pathInQrc.isEmpty())
		{
			paths.prepend(pathInQrc);
		}

		const QString pathInCache = cachePath();
		if (!pathInCache.isEmpty())
		{
			paths.prepend(pathInCache);
		}

		if (isDirty())
		{
			update();
		}
	}

	for (const QString& path : qAsConst(paths))
	{
		if (pValidate(path))
		{
			return true;
		}
	}
	return false;
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

		auto* const downloader = Env::getSingleton<Downloader>();
		connect(downloader, &Downloader::fireDownloadSuccess, this, &UpdatableFile::onDownloadSuccess);
		connect(downloader, &Downloader::fireDownloadFailed, this, &UpdatableFile::onDownloadFailed);
		connect(downloader, &Downloader::fireDownloadUnnecessary, this, &UpdatableFile::onDownloadUnnecessary);

		downloader->download(mUpdateUrl, cacheTimestamp());
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
	if (mName.isEmpty())
	{
		return;
	}

	if (mSectionCachePath.isEmpty())
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
