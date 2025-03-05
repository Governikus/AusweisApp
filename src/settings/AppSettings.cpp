/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "AppSettings.h"


using namespace governikus;


AppSettings::AppSettings()
	: QObject()
	, mGeneralSettings()
	, mPreVerificationSettings()
	, mRemoteReaderSettings()
	, mSimulatorSettings()
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


SimulatorSettings& AppSettings::getSimulatorSettings()
{
	return mSimulatorSettings;
}
