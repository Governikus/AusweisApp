/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "AppSettings.h"


using namespace governikus;

AppSettings::AppSettings()
	: QObject()
	, mGeneralSettings()
	, mPreVerificationSettings()
	, mHistorySettings()
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


HistorySettings& AppSettings::getHistorySettings()
{
	return mHistorySettings;
}


RemoteServiceSettings& AppSettings::getRemoteServiceSettings()
{
	return mRemoteReaderSettings;
}
