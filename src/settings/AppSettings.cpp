/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "AppSettings.h"


using namespace governikus;

AppSettings::AppSettings()
	: AbstractSettings()
	, mGeneralSettings()
	, mPreVerificationSettings()
	, mHistorySettings()
	, mRemoteReaderSettings()
{
	connect(&mGeneralSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mPreVerificationSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mHistorySettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
	connect(&mRemoteReaderSettings, &AbstractSettings::fireSettingsChanged, this, &AbstractSettings::fireSettingsChanged);
}


AppSettings::~AppSettings()
{
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
