/*!
 * \brief Model implementation for the settings.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "SettingsModel.h"

#include "AppSettings.h"

using namespace governikus;


bool SettingsModel::isDeveloperMode() const
{
	return AppSettings::getInstance().getGeneralSettings().isDeveloperMode();
}


void SettingsModel::setDeveloperMode(bool pEnable)
{
	if (isDeveloperMode() == pEnable)
	{
		return;
	}

	AppSettings::getInstance().getGeneralSettings().setDeveloperMode(pEnable);
	AppSettings::getInstance().getGeneralSettings().save();
	Q_EMIT fireDeveloperModeChanged();
}


bool SettingsModel::useSelfauthenticationTestUri() const
{
	return AppSettings::getInstance().getGeneralSettings().useSelfauthenticationTestUri();
}


void SettingsModel::setUseSelfauthenticationTestUri(bool pUse)
{
	if (useSelfauthenticationTestUri() == pUse)
	{
		return;
	}

	AppSettings::getInstance().getGeneralSettings().setUseSelfauthenticationTestUri(pUse);
	AppSettings::getInstance().getGeneralSettings().save();
	Q_EMIT fireUseSelfauthenticationTestUriChanged();
}
