/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "AppSettings.h"


using namespace governikus;

AppSettings::AppSettings()
	: QObject()
	, mGeneralSettings()
	, mPreVerificationSettings()
	, mRemoteReaderSettings()
{
}


GeneralSettings& AppSettings::getGeneralSettings()
{
	return mGeneralSettings;
}


PreVerificationSettings& AppSettings::getPreVerificationSettings()
{
	return mPreVerificationSettings;
}


RemoteServiceSettings& AppSettings::getRemoteServiceSettings()
{
	return mRemoteReaderSettings;
}
