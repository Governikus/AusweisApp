/*
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "IconCache.h"

#include "AppSettings.h"
#include "IconUpdateBackend.h"

#include <QDir>
#include <QLoggingCategory>
#include <QStandardPaths>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(update)


static const QLatin1String QRC_PREFIX("qrc:///");


QString IconCache::getIconPathInCache(const QString& pIcon) const
{
	return mIconCacheFolderPath + pIcon;
}


QStringList IconCache::getIconsToDownload(const QStringList& pRequiredIcons) const
{
	QStringList result;
	for (const auto& icon : pRequiredIcons)
	{
		if (icon.startsWith(QRC_PREFIX))
		{
			continue;
		}

		// Only download files that are not in the cache already.
		QFile file(getIconPathInCache(icon));
		if (!file.exists())
		{
			result += icon;
		}
	}

	qCDebug(update) << "Icons to download:" << result;

	return result;
}


QUrl IconCache::getRemoteIconUrl(const QString& pIcon) const
{
	return QUrl(mIconUpdateUrlBase + pIcon);
}


void IconCache::processNextIcon()
{
	// Nothing to do, possibly after last icon has been downloaded.
	if (mPendingIcons.isEmpty())
	{
		const QMap<QString, QString> iconMap = makeIconMap(mRequiredIcons);

		mRequiredIcons.clear();
		mIconService.clear();

		Q_EMIT fireIconMapChanged(iconMap);
	}
	else
	{
		const QString& icon = mPendingIcons.constFirst();

		mIconService.reset(new IconService(mIconCacheFolderPath, icon));

		mIconService->setUpdateUrl(getRemoteIconUrl(icon));
		mIconService->setTrustedUpdateCertificates(mUpdateCertificates);

		connect(mIconService.data(), &IconService::fireProcessingFinished, this, &IconCache::onProcessingFinished);

		mIconService->runUpdate();
	}
}


bool IconCache::canBeMapped(const QString& pIcon) const
{
	const QString pathToCheck = pIcon.startsWith(QRC_PREFIX) ? pIcon : getIconPathInCache(pIcon);
	QFile file(pathToCheck);

	return file.exists();
}


QMap<QString, QString> IconCache::makeIconMap(const QStringList& pIcons) const
{
	QMap<QString, QString> result;

	for (const auto& icon : pIcons)
	{
		// If the icon is a normal file, check that it is in the cache.
		if (canBeMapped(icon))
		{
			result.insert(icon, getLocalIconUrl(icon));
		}
	}

	return result;
}


void IconCache::onProcessingFinished(const QString& pIcon)
{
	qCDebug(update) << "Finished processing icon" << pIcon;

	if (mPendingIcons.size() > 0)
	{
		if (mPendingIcons.first() == pIcon)
		{
			mPendingIcons.removeFirst();
			processNextIcon();
		}
		else
		{
			// Should never happen: mPendingIcons.first() != pIcon would indicate some bug.
			qCWarning(update) << "Failed to process an icon, aborting update.";

			mRequiredIcons.clear();
			mPendingIcons.clear();
		}
	}
}


IconCache::IconCache()
	: mIconCacheFolderPath()
	, mIconUpdateUrlBase()
	, mIconService()
{
}


IconCache::~IconCache()
{
}


void IconCache::setTrustedUpdateCertificates(const QVector<QSslCertificate>& pUpdateCertificates)
{
	mUpdateCertificates = pUpdateCertificates;
}


void IconCache::setIconUpdateUrlBase(const QString& pIconUpdateUrlBase)
{
	mIconUpdateUrlBase = pIconUpdateUrlBase;
}


QString IconCache::getLocalIconUrl(const QString& pIcon) const
{
	return pIcon.startsWith(QRC_PREFIX) ? pIcon : QUrl::fromLocalFile(getIconPathInCache(pIcon)).toString();
}


bool IconCache::init()
{
	const QStringList cachePaths = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
	if (cachePaths.isEmpty())
	{
		qCWarning(update) << "No cache paths found!";

		return false;
	}
	else
	{
		const QString cacheBasePath = cachePaths.first();
		if (cacheBasePath.isEmpty())
		{
			qCWarning(update) << "Cache base folder is invalid (empty).";

			return false;
		}

		mIconCacheFolderPath = cacheBasePath + QStringLiteral("/icons/");
		const QDir cacheFolder(mIconCacheFolderPath);

		qCDebug(update) << "Initializing cache folder:" << mIconCacheFolderPath;

		return cacheFolder.exists() || cacheFolder.mkpath(mIconCacheFolderPath);
	}
}


void IconCache::onRequiredIcons(const QStringList& pIcons)
{
	// Only run if not running already.
	if (mPendingIcons.isEmpty())
	{
		mRequiredIcons = pIcons;
		mPendingIcons = getIconsToDownload(mRequiredIcons);

		processNextIcon();
	}
}
