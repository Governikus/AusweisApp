/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "Updater.h"

#include "AppSettings.h"
#include "AppUpdateService.h"
#include "DriverService.h"
#include "ProviderService.h"
#include "SecureStorage.h"
#include "SingletonHelper.h"
#include "VersionNumber.h"

#include <QLoggingCategory>

using namespace governikus;

defineSingleton(Updater)


Updater::Updater()
	: mIconCache()
{
	SecureStorage& secureStorage = AppSettings::getInstance().getSecureStorage();
	secureStorage.load();

	auto url = VersionNumber::getApplicationVersion().isDeveloperVersion() ? secureStorage.getAppcastBetaUpdateUrl() : secureStorage.getAppcastUpdateUrl();
	AppUpdateService::getInstance().setUpdateUrl(url);
	AppUpdateService::getInstance().setTrustedUpdateCertificates(secureStorage.getUpdateCertificates());
	connect(&AppUpdateService::getInstance(), &AppUpdateService::fireAppUpdateCheckFinished, this, &Updater::fireAppUpdateCheckFinished);

#if defined(Q_OS_WIN) || defined(Q_OS_OSX) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	//	qDebug() << "Driver update url =" << secureStorage.getDriverUpdateUrl();
	//	DriverService::getInstance().setUpdateUrl(secureStorage.getDriverUpdateUrl());
	//	DriverService::getInstance().setTrustedUpdateCertificates(secureStorage.getUpdateCertificates());
#endif

	qDebug() << "Provider update url =" << secureStorage.getProviderUpdateUrl();
	ProviderService::getInstance().setUpdateUrl(secureStorage.getProviderUpdateUrl());
	ProviderService::getInstance().setTrustedUpdateCertificates(secureStorage.getUpdateCertificates());
	connect(&ProviderService::getInstance(), &UpdateService::fireUpdateFinished, this, &Updater::onUpdateProviderSettingsFinished);

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


void Updater::checkAppUpdate()
{
	AppUpdateService::getInstance().runUpdate();
}


const IconCache& Updater::getIconCache() const
{
	return mIconCache;
}


const AppUpdateData& Updater::getUpdateData() const
{
	return AppUpdateService::getInstance().getUpdateData();
}


void Updater::skipVersion(const QString& pVersion)
{
	AppUpdateService::getInstance().skipVersion(pVersion);
}


void Updater::doUpdateRemoteSettings()
{
#if defined(Q_OS_WIN) || defined(Q_OS_OSX) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	//	DriverService::getInstance().runUpdate();
#endif

	ProviderService::getInstance().runUpdate();
}


void Updater::onUpdateProviderSettingsFinished()
{
	// Connection between cache and settings is set up: request icon map to initialize settings.
	AppSettings::getInstance().getProviderSettings().requestIconMap();
}
