/*!
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#include "SettingsModel.h"

#include "AppSettings.h"
#include "HistorySettings.h"
#include "LanguageLoader.h"

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#endif

using namespace governikus;


SettingsModel::SettingsModel()
	: mShowTutorialOnStart(false)
{
	const HistorySettings& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
	connect(&settings, &HistorySettings::fireEnabledChanged, this, &SettingsModel::fireHistoryEnabledChanged);


#ifdef Q_OS_ANDROID
	auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	if (generalSettings.isShowSetupAssistant())
	{
		generalSettings.setShowSetupAssistant(false);
		generalSettings.save();

		bool startedByAuth = QAndroidJniObject::callStaticMethod<jboolean>("com/governikus/ausweisapp2/MainActivity", "isStartedByAuth");
		if (!startedByAuth)
		{
			mShowTutorialOnStart = true;
		}
	}
#endif
}


QString SettingsModel::getEmptyString()
{
	return QString();
}


QString SettingsModel::getLanguage() const
{
	return LanguageLoader::getInstance().getUsedLocale().bcp47Name();
}


void SettingsModel::setLanguage(const QString& pLanguage)
{
	GeneralSettings& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	settings.setLanguage(QLocale(pLanguage).language());
	settings.save();

	Q_EMIT fireLanguageChanged();
}


bool SettingsModel::isDeveloperMode() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode();
}


void SettingsModel::setDeveloperMode(bool pEnable)
{
	if (isDeveloperMode() == pEnable)
	{
		return;
	}

	Env::getSingleton<AppSettings>()->getGeneralSettings().setDeveloperMode(pEnable);
	Env::getSingleton<AppSettings>()->getGeneralSettings().save();
	Q_EMIT fireDeveloperModeChanged();
}


bool SettingsModel::useSelfauthenticationTestUri() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().useSelfAuthTestUri();
}


void SettingsModel::setUseSelfauthenticationTestUri(bool pUse)
{
	if (useSelfauthenticationTestUri() == pUse)
	{
		return;
	}

	Env::getSingleton<AppSettings>()->getGeneralSettings().setUseSelfauthenticationTestUri(pUse);
	Env::getSingleton<AppSettings>()->getGeneralSettings().save();
	Q_EMIT fireUseSelfauthenticationTestUriChanged();
}


QString SettingsModel::getServerName() const
{
	return Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getServerName();
}


bool SettingsModel::isValidServerName(const QString& name) const
{
	return !name.isEmpty();
}


void SettingsModel::setServerName(const QString& name)
{
	RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	settings.setServerName(name);
	settings.save();
}


void SettingsModel::removeTrustedCertificate(const QString& pFingerprint)
{
	Env::getSingleton<AppSettings>()->getRemoteServiceSettings().removeTrustedCertificate(pFingerprint);
}


bool SettingsModel::getPinPadMode() const
{
	return Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
}


void SettingsModel::setPinPadMode(bool pPinPadMode)
{
	RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	settings.setPinPadMode(pPinPadMode);
	settings.save();
}


bool SettingsModel::isHistoryEnabled() const
{
	const HistorySettings& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
	return settings.isEnabled();
}


void SettingsModel::setHistoryEnabled(bool pEnabled)
{
	HistorySettings& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
	settings.setEnabled(pEnabled);
	settings.save();
}


int SettingsModel::removeHistory(const QString& pPeriodToRemove)
{
	HistorySettings& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
	int removedItemCount = settings.deleteSettings(Enum<TimePeriod>::fromString(pPeriodToRemove, TimePeriod::UNKNOWN));
	settings.save();
	return removedItemCount;
}


bool SettingsModel::askForDeviceSurvey() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().askForDeviceSurvey();
}


void SettingsModel::setDeviceSurveyPending(bool pDeviceSurveyPending)
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setDeviceSurveyPending(pDeviceSurveyPending);
}
