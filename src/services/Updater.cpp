/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "Updater.h"

#include "AppSettings.h"
#include "DriverService.h"
#include "ProviderService.h"
#include "SecureStorage.h"
#include "SingletonHelper.h"
#include "VersionNumber.h"

#if defined(Q_OS_WIN) || defined(Q_OS_OSX) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
#include "fervor/fvupdater.h"
#endif

#include <QLoggingCategory>

using namespace governikus;

defineSingleton(Updater)


Updater::Updater()
	: mIconCache()
{
	SecureStorage& secureStorage = AppSettings::getInstance().getSecureStorage();
	secureStorage.load();

#if defined(Q_OS_WIN) || defined(Q_OS_OSX) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	auto url = VersionNumber::getApplicationVersion().isDeveloperVersion() ? secureStorage.getAppcastBetaUpdateUrl() : secureStorage.getAppcastUpdateUrl();
	FvUpdater::sharedUpdater()->SetFeedURL(url);

	//	qDebug() << "Driver update url =" << secureStorage.getDriverUpdateUrl();
	//	DriverService::getSharedInstance()->setUpdateUrl(secureStorage.getDriverUpdateUrl());
	//	DriverService::getSharedInstance()->setTrustedUpdateCertificates(secureStorage.getUpdateCertificates());
#endif

	qDebug() << "Provider update url =" << secureStorage.getProviderUpdateUrl();
	ProviderService::getSharedInstance()->setUpdateUrl(secureStorage.getProviderUpdateUrl());
	ProviderService::getSharedInstance()->setTrustedUpdateCertificates(secureStorage.getUpdateCertificates());
	connect(ProviderService::getSharedInstance(), &UpdateService::fireUpdateFinished, this, &Updater::onUpdateProviderSettingsFinished);

	mIconCache.init();
	mIconCache.setTrustedUpdateCertificates(secureStorage.getUpdateCertificates());
	qDebug() << "Icon update url base =" << secureStorage.getProviderIconUpdateUrlBase();
	mIconCache.setIconUpdateUrlBase(secureStorage.getProviderIconUpdateUrlBase());

	qDebug() << "Connecting icon cache with provider settings";
	connect(&AppSettings::getInstance().getProviderSettings(), &ProviderSettings::fireRequiredIcons, &mIconCache, &IconCache::onRequiredIcons);
	connect(&mIconCache, &IconCache::fireIconMapChanged, &AppSettings::getInstance().getProviderSettings(), &ProviderSettings::onIconMapChanged);
}


Updater& Updater::getInstance()
{
	return *Instance;
}


void Updater::update()
{
	QMetaObject::invokeMethod(this, "doUpdateRemoteSettings", Qt::QueuedConnection);
}


void Updater::checkAppUpdate(bool pSilent)
{
#if defined(Q_OS_WIN) || defined(Q_OS_OSX) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	if (pSilent)
	{
		FvUpdater::sharedUpdater()->CheckForUpdatesSilent();
	}
	else
	{
		FvUpdater::sharedUpdater()->CheckForUpdatesNotSilent();
	}
#else
	Q_UNUSED(pSilent)
#endif
}


const IconCache& Updater::getIconCache() const
{
	return mIconCache;
}


void Updater::doUpdateRemoteSettings()
{
#if defined(Q_OS_WIN) || defined(Q_OS_OSX) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	//	DriverService::getSharedInstance()->runUpdate();
#endif

	ProviderService::getSharedInstance()->runUpdate();
}


void Updater::onUpdateProviderSettingsFinished()
{
	// Connection between cache and settings is set up: request icon map to initialize settings.
	AppSettings::getInstance().getProviderSettings().requestIconMap();
}
