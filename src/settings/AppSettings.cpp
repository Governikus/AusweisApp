/*!
 * AppSettings.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AppSettings.h"

#include "SingletonHelper.h"

using namespace governikus;

defineSingleton(AppSettings)


AppSettings::AppSettings()
	: AbstractSettings()
	, mDriverSettings()
	, mGeneralSettings()
	, mPreVerificationSettings()
	, mProviderSettings()
	, mSecureStorage()
	, mHistorySettings()
	, mRemoteReaderSettings()
{
	connect(&mDriverSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mGeneralSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mPreVerificationSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mProviderSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
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


void AppSettings::load()
{
	mDriverSettings.load();
	mGeneralSettings.load();
	mPreVerificationSettings.load();
	mProviderSettings.load();
	mSecureStorage.load();
	mHistorySettings.load();
	mRemoteReaderSettings.load();
}


bool AppSettings::isUnsaved() const
{
	AppSettings oldSettings;
	oldSettings.load();
	return oldSettings != *this;
}


void AppSettings::save()
{
	mDriverSettings.save();
	mGeneralSettings.save();
	mPreVerificationSettings.save();
	mProviderSettings.save();
	mHistorySettings.save();
	mRemoteReaderSettings.save();
}


DriverSettings& AppSettings::getDriverSettings()
{
	return mDriverSettings;
}


GeneralSettings& AppSettings::getGeneralSettings()
{
	return mGeneralSettings;
}


PreVerificationSettings& AppSettings::getPreVerificationSettings()
{
	return mPreVerificationSettings;
}


ProviderSettings& AppSettings::getProviderSettings()
{
	return mProviderSettings;
}


SecureStorage& AppSettings::getSecureStorage()
{
	return mSecureStorage;
}


HistorySettings& AppSettings::getHistorySettings()
{
	return mHistorySettings;
}


RemoteReaderSettings& AppSettings::getRemoteReaderSettings()
{
	return mRemoteReaderSettings;
}
