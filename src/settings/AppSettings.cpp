/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "AppSettings.h"

#include "SingletonHelper.h"

#ifdef Q_OS_ANDROID
	#include <QtAndroid>
#endif

using namespace governikus;

defineSingleton(AppSettings)


AppSettings::AppSettings()
	: AbstractSettings()
	, mUsedAsSdk(false)
	, mGeneralSettings()
	, mPreVerificationSettings()
	, mHistorySettings()
	, mRemoteReaderSettings()
{
#ifdef Q_OS_ANDROID
	if (QtAndroid::androidService().isValid())
	{
		mUsedAsSdk = true;
	}
#endif

	connect(&mGeneralSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mPreVerificationSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mHistorySettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mRemoteReaderSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
}


AppSettings::~AppSettings()
{
}


AppSettings& AppSettings::getInstance()
{
	return *Instance;
}


bool AppSettings::isUsedAsSDK() const
{
	return mUsedAsSdk;
}


void AppSettings::setUsedAsSDK(bool pSdk)
{
	mUsedAsSdk = pSdk;
}


void AppSettings::save()
{
	mGeneralSettings.save();
	mPreVerificationSettings.save();
	mHistorySettings.save();
	mRemoteReaderSettings.save();
}


GeneralSettings& AppSettings::getGeneralSettings()
{
	return mGeneralSettings;
}


PreVerificationSettings& AppSettings::getPreVerificationSettings()
{
	return mPreVerificationSettings;
}


HistorySettings& AppSettings::getHistorySettings()
{
	return mHistorySettings;
}


RemoteServiceSettings& AppSettings::getRemoteServiceSettings()
{
	return mRemoteReaderSettings;
}
