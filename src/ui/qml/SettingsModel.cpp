/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "SettingsModel.h"

#include "AppSettings.h"
#include "HistorySettings.h"
#include "LanguageLoader.h"
#include "NetworkManager.h"
#include "PlatformHelper.h"
#include "Service.h"
#include "SingletonHelper.h"


#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#endif

using namespace governikus;


defineSingleton(SettingsModel)


SettingsModel::SettingsModel()
	: QObject()
	, mAdvancedSettings(false)
	, mIsStartedByAuth(false)
	, mShowBetaTesting(true)
{
	const HistorySettings& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
	connect(&settings, &HistorySettings::fireEnabledChanged, this, &SettingsModel::fireHistoryEnabledChanged);

	connect(Env::getSingleton<AppUpdateDataModel>(), &AppUpdateDataModel::fireAppUpdateDataChanged, this, &SettingsModel::fireAppUpdateDataChanged);

	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();

	connect(&generalSettings, &GeneralSettings::fireShowInAppNotificationsChanged, this, &SettingsModel::fireShowInAppNotificationsChanged);
	connect(&generalSettings, &GeneralSettings::fireDeveloperOptionsChanged, this, &SettingsModel::fireDeveloperOptionsChanged);

#ifdef Q_OS_ANDROID
	mIsStartedByAuth = QAndroidJniObject::callStaticMethod<jboolean>("com/governikus/ausweisapp2/MainActivity", "isStartedByAuth");
#endif
}


SettingsModel& SettingsModel::getInstance()
{
	return *Instance;
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
	if (getLanguage() != pLanguage)
	{
		GeneralSettings& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setLanguage(QLocale(pLanguage).language());
		settings.save();
		Q_EMIT fireLanguageChanged();
	}
}


bool SettingsModel::isAdvancedSettings() const
{
	return mAdvancedSettings;
}


void SettingsModel::setAdvancedSettings(bool pEnabled)
{
	if (mAdvancedSettings != pEnabled)
	{
		mAdvancedSettings = pEnabled;
		Q_EMIT fireAdvancedSettingsChanged();
	}
}


bool SettingsModel::isDeveloperOptions() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperOptions();
}


void SettingsModel::setDeveloperOptions(bool pEnable)
{
	if (isDeveloperOptions() != pEnable)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setDeveloperOptions(pEnable);
		settings.save();
	}
}


bool SettingsModel::isDeveloperMode() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode();
}


void SettingsModel::setDeveloperMode(bool pEnable)
{
	if (isDeveloperMode() != pEnable)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setDeveloperMode(pEnable);
		settings.save();
	}
}


bool SettingsModel::useSelfauthenticationTestUri() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().useSelfAuthTestUri();
}


void SettingsModel::setUseSelfauthenticationTestUri(bool pUse)
{
	if (useSelfauthenticationTestUri() != pUse)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setUseSelfauthenticationTestUri(pUse);
		settings.save();
	}
}


QString SettingsModel::getServerName() const
{
	return Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getServerName();
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


int SettingsModel::removeHistory(const QString& pPeriodToRemove)
{
	HistorySettings& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
	int removedItemCount = settings.deleteSettings(Enum<TimePeriod>::fromString(pPeriodToRemove, TimePeriod::UNKNOWN));
	settings.save();
	return removedItemCount;
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
	if (isHistoryEnabled() != pEnabled)
	{
		HistorySettings& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
		settings.setEnabled(pEnabled);
		settings.save();
	}
}


int SettingsModel::removeEntireHistory()
{
	HistorySettings& settings = Env::getSingleton<AppSettings>()->getHistorySettings();
	return settings.deleteSettings(TimePeriod::ALL_HISTORY);
}


bool SettingsModel::isUseScreenKeyboard() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isUseScreenKeyboard();
}


void SettingsModel::setUseScreenKeyboard(bool pUseScreenKeyboard)
{
	if (isUseScreenKeyboard() != pUseScreenKeyboard)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setUseScreenKeyboard(pUseScreenKeyboard);
		settings.save();
		Q_EMIT fireScreenKeyboardChanged();
	}
}


bool SettingsModel::isVisualPrivacy() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isVisualPrivacy();
}


void SettingsModel::setVisualPrivacy(bool pVisualPrivacy)
{
	if (isVisualPrivacy() != pVisualPrivacy)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setVisualPrivacy(pVisualPrivacy);
		settings.save();
		Q_EMIT fireScreenKeyboardChanged();
	}
}


bool SettingsModel::isShuffleScreenKeyboard() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isShuffleScreenKeyboard();
}


void SettingsModel::setShuffleScreenKeyboard(bool pShuffleScreenKeyboard)
{
	if (isShuffleScreenKeyboard() != pShuffleScreenKeyboard)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setShuffleScreenKeyboard(pShuffleScreenKeyboard);
		settings.save();
		Q_EMIT fireScreenKeyboardChanged();
	}
}


bool SettingsModel::isEnableCanAllowed() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isEnableCanAllowed();
}


void SettingsModel::setEnableCanAllowed(bool pEnableCanAllowed)
{
	if (isEnableCanAllowed() != pEnableCanAllowed)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setEnableCanAllowed(pEnableCanAllowed);
		settings.save();
		Q_EMIT fireCanAllowedChanged();
	}
}


bool SettingsModel::isSkipRightsOnCanAllowed() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isSkipRightsOnCanAllowed();
}


void SettingsModel::setSkipRightsOnCanAllowed(bool pSkipRightsOnCanAllowed)
{
	if (isSkipRightsOnCanAllowed() != pSkipRightsOnCanAllowed)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setSkipRightsOnCanAllowed(pSkipRightsOnCanAllowed);
		settings.save();
		Q_EMIT fireCanAllowedChanged();
	}
}


bool SettingsModel::isShowSetupAssistantOnStart() const
{
	auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	return generalSettings.isShowSetupAssistant() && !mIsStartedByAuth;
}


void SettingsModel::setShowSetupAssistantOnStart(bool pShowSetupAssistantOnStart)
{
	if (isShowSetupAssistantOnStart() != pShowSetupAssistantOnStart)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setShowSetupAssistant(pShowSetupAssistantOnStart);
		settings.save();
		Q_EMIT fireShowSetupAssistantOnStartChanged();
	}
}


bool SettingsModel::isAutoStartAvailable() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isAutoStartAvailable();
}


bool SettingsModel::isAutoStart() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isAutoStart();
}


bool SettingsModel::autoStartIsSetByAdmin() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().autoStartIsSetByAdmin();
}


void SettingsModel::setAutoStart(bool pEnabled)
{
	if (isAutoStart() != pEnabled)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setAutoStart(pEnabled);
		settings.save();
		Q_EMIT fireAutoStartChanged();
	}
}


bool SettingsModel::requestStoreFeedback() const
{
#ifdef Q_OS_ANDROID
	const bool startedByAuth = QAndroidJniObject::callStaticMethod<jboolean>("com/governikus/ausweisapp2/MainActivity", "isStartedByAuth");
	if (startedByAuth)
	{
		return false;
	}
#endif

	return Env::getSingleton<AppSettings>()->getGeneralSettings().isRequestStoreFeedback();
}


void SettingsModel::hideFutureStoreFeedbackDialogs()
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setRequestStoreFeedback(false);
}


bool SettingsModel::askForDeviceSurvey() const
{
	if (isPlatform(*this, Platform::IOS))
	{
		return false;
	}
	return Env::getSingleton<AppSettings>()->getGeneralSettings().askForDeviceSurvey();
}


void SettingsModel::setDeviceSurveyPending(bool pDeviceSurveyPending)
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setDeviceSurveyPending(pDeviceSurveyPending);
}


bool SettingsModel::isAutoCloseWindowAfterAuthentication() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isAutoCloseWindowAfterAuthentication();
}


void SettingsModel::setAutoCloseWindowAfterAuthentication(bool pEnabled)
{
	if (isAutoCloseWindowAfterAuthentication() != pEnabled)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setAutoCloseWindowAfterAuthentication(pEnabled);
		settings.save();
		Q_EMIT fireAutoCloseWindowAfterAuthenticationChanged();
	}
}


bool SettingsModel::isAutoUpdateAvailable() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isAutoUpdateAvailable();
}


bool SettingsModel::isAutoUpdateCheck() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isAutoUpdateCheck();
}


bool SettingsModel::autoUpdateCheckIsSetByAdmin() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().autoUpdateCheckIsSetByAdmin();
}


void SettingsModel::setAutoUpdateCheck(bool pAutoUpdateCheck)
{
	if (isAutoUpdateCheck() != pAutoUpdateCheck)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setAutoUpdateCheck(pAutoUpdateCheck);
		settings.save();
		Q_EMIT fireAutoUpdateCheckChanged();
	}
}


bool SettingsModel::isRemindUserToClose() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isRemindUserToClose();
}


void SettingsModel::setRemindUserToClose(bool pRemindUser)
{
	if (isRemindUserToClose() != pRemindUser)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setRemindUserToClose(pRemindUser);
		settings.save();
		Q_EMIT fireRemindUserToCloseChanged();
	}
}


bool SettingsModel::isTransportPinReminder() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isTransportPinReminder();
}


void SettingsModel::setTransportPinReminder(bool pTransportPinReminder)
{
	if (isTransportPinReminder() != pTransportPinReminder)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setTransportPinReminder(pTransportPinReminder);
		settings.save();
		Q_EMIT fireTransportPinReminderChanged();
	}
}


bool SettingsModel::isShowInAppNotifications() const
{

	return Env::getSingleton<AppSettings>()->getGeneralSettings().isShowInAppNotifications();
}


void SettingsModel::setShowInAppNotifications(bool pShowInAppNotifications)
{
	if (isShowInAppNotifications() != pShowInAppNotifications)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setShowInAppNotifications(pShowInAppNotifications);
		settings.save();
	}
}


bool SettingsModel::isShowNewUiHint() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isShowNewUiHint();
}


void SettingsModel::setShowNewUiHint(bool pShowNewUiHint)
{
	if (pShowNewUiHint != isShowNewUiHint())
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setShowNewUiHint(pShowNewUiHint);
		Q_EMIT fireShowNewUiHintChanged();
	}
}


void SettingsModel::updateApp()
{
	Env::getSingleton<Service>()->updateApp();
}


AppUpdateDataModel* SettingsModel::getAppUpdateData() const
{
	auto* dataModel = Env::getSingleton<AppUpdateDataModel>();
	return dataModel;
}


QUrl SettingsModel::getCustomProxyUrl() const
{
	auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	QUrl url;
	switch (settings.getCustomProxyType())
	{
		case QNetworkProxy::Socks5Proxy:
			url.setScheme(QLatin1String("socks5"));
			break;

		case QNetworkProxy::HttpProxy:
			url.setScheme(QLatin1String("http"));
			break;

		default:
			url.setScheme(QLatin1String("unknown"));
			break;
	}
	url.setHost(settings.getCustomProxyHost());
	url.setPort(settings.getCustomProxyPort());
	return url.toString();
}


bool SettingsModel::isCustomProxyAttributesPresent() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().customProxyAttributesPresent();
}


bool SettingsModel::isUseCustomProxy()
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().useCustomProxy();
}


void SettingsModel::setUseCustomProxy(bool pUseCustomProxy)
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setUseCustomProxy(pUseCustomProxy);
}
