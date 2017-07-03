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
	, mProxySettings()
	, mSecureStorage()
	, mHistorySettings()
{
	connect(&mDriverSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mGeneralSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mPreVerificationSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mProviderSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mProxySettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mHistorySettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
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
	mProxySettings.load();
	mSecureStorage.load();
	mHistorySettings.load();
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
	mProxySettings.save();
	mHistorySettings.save();
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


ProxySettings& AppSettings::getProxySettings()
{
	return mProxySettings;
}


SecureStorage& AppSettings::getSecureStorage()
{
	return mSecureStorage;
}


HistorySettings& AppSettings::getHistorySettings()
{
	return mHistorySettings;
}
