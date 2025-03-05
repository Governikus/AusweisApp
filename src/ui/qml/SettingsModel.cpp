/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "SettingsModel.h"

#include "AppSettings.h"
#include "LanguageLoader.h"
#include "ReaderManager.h"
#include "Service.h"
#include "VolatileSettings.h"

#include <QQmlEngine>

#ifdef Q_OS_ANDROID
	#include <QJniObject>
#endif


using namespace governikus;


SettingsModel::SettingsModel()
	: QObject()
	, mAdvancedSettings(false)
	, mIsStartedByAuth(false)
	, mShowBetaTesting(!qEnvironmentVariableIsSet("SUPPRESS_BETA_LOGO"))
{
	connect(Env::getSingleton<AppUpdateDataModel>(), &AppUpdateDataModel::fireAppUpdateDataChanged, this, &SettingsModel::fireAppUpdateDataChanged);

	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	connect(&generalSettings, &GeneralSettings::fireShowInAppNotificationsChanged, this, &SettingsModel::fireShowInAppNotificationsChanged);
	connect(&generalSettings, &GeneralSettings::fireDeveloperOptionsChanged, this, &SettingsModel::fireDeveloperOptionsChanged);
	connect(&generalSettings, &GeneralSettings::fireProxyChanged, this, &SettingsModel::fireUseCustomProxyChanged);
	connect(&generalSettings, &GeneralSettings::fireUseSystemFontChanged, this, &SettingsModel::fireUseSystemFontChanged);
	connect(&generalSettings, &GeneralSettings::fireUseAnimationsChanged, this, &SettingsModel::fireUseAnimationsChanged);
	connect(&generalSettings, &GeneralSettings::fireDarkModeChanged, this, &SettingsModel::fireDarkModeChanged);
	connect(&generalSettings, &GeneralSettings::firePreferredTechnologyChanged, this, &SettingsModel::firePreferredTechnologyChanged);

	const auto& simulatorSettings = Env::getSingleton<AppSettings>()->getSimulatorSettings();
	connect(&simulatorSettings, &SimulatorSettings::fireEnabledChanged, this, &SettingsModel::fireSimulatorChanged);
	connect(&simulatorSettings, &SimulatorSettings::fireEnabledChanged, this, &SettingsModel::firePreferredTechnologyChanged);

	const auto* readerManager = Env::getSingleton<ReaderManager>();
	connect(readerManager, &ReaderManager::fireStatusChanged, this, &SettingsModel::firePreferredTechnologyChanged);

#ifdef Q_OS_ANDROID
	mIsStartedByAuth = QJniObject::callStaticMethod<jboolean>("com/governikus/ausweisapp2/MainActivity", "isStartedByAuth");
#endif
}


QString SettingsModel::getLanguage() const
{
	return LanguageLoader::getLocaleCode();
}


void SettingsModel::setLanguage(const QString& pLanguage) const
{
	if (getLanguage() != pLanguage)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setLanguage(QLocale(pLanguage).language());
		// do not emit fireLanguageChanged here! The AppController will emit this!
	}
}


void SettingsModel::onTranslationChanged()
{
	Q_EMIT fireLanguageChanged();
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


void SettingsModel::setDeveloperOptions(bool pEnable) const
{
	if (isDeveloperOptions() != pEnable)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setDeveloperOptions(pEnable);
	}
}


bool SettingsModel::isDeveloperMode() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isDeveloperMode();
}


void SettingsModel::setDeveloperMode(bool pEnable) const
{
	if (isDeveloperMode() != pEnable)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setDeveloperMode(pEnable);
	}
}


bool SettingsModel::useSelfauthenticationTestUri() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().useSelfAuthTestUri();
}


void SettingsModel::setUseSelfauthenticationTestUri(bool pUse) const
{
	if (useSelfauthenticationTestUri() != pUse)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setUseSelfauthenticationTestUri(pUse);
	}
}


QString SettingsModel::getDeviceName() const
{
	return Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getDeviceName();
}


void SettingsModel::setDeviceName(const QString& name)
{
	auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	settings.setDeviceName(name);
	Q_EMIT fireDeviceNameChanged();
}


bool SettingsModel::getPinPadMode() const
{
	return Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getPinPadMode();
}


void SettingsModel::setPinPadMode(bool pPinPadMode)
{
	if (getPinPadMode() != pPinPadMode)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		settings.setPinPadMode(pPinPadMode);
		Q_EMIT firePinPadModeChanged();
	}
}


bool SettingsModel::getShowAccessRights() const
{
	return Env::getSingleton<AppSettings>()->getRemoteServiceSettings().getShowAccessRights();
}


void SettingsModel::setShowAccessRights(bool pShowAccessRights)
{
	if (getShowAccessRights() != pShowAccessRights)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
		settings.setShowAccessRights(pShowAccessRights);
		Q_EMIT fireShowAccessRightsChanged();
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
		Q_EMIT fireCanAllowedChanged();
	}
}


bool SettingsModel::isSimulatorEnabled() const
{
	return Env::getSingleton<AppSettings>()->getSimulatorSettings().isEnabled();
}


void SettingsModel::setSimulatorEnabled(bool pEnabled) const
{
	if (isSimulatorEnabled() != pEnabled)
	{
		auto& simulatorSettings = Env::getSingleton<AppSettings>()->getSimulatorSettings();
		simulatorSettings.setEnabled(pEnabled);
	}
}


UiModule SettingsModel::getStartupModule() const
{
	if (mIsStartedByAuth)
	{
		return UiModule::IDENTIFY;
	}

	if (getShowOnboarding() && !getOnboardingShown())
	{
		return UiModule::ONBOARDING;
	}

	const auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	return Enum<UiModule>::fromString(generalSettings.getStartupModule(), UiModule::DEFAULT);
}


void SettingsModel::setStartupModule(UiModule pModule)
{
	if (getStartupModule() != pModule)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setStartupModule(getEnumName(pModule));
		Q_EMIT fireStartupModuleChanged();
	}
}


bool SettingsModel::getShowOnboarding() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().getShowOnboarding();
}


void SettingsModel::setShowOnboarding(bool pShowOnboarding)
{
	if (getShowOnboarding() != pShowOnboarding)
	{
		Env::getSingleton<AppSettings>()->getGeneralSettings().setShowOnboarding(pShowOnboarding);
		Q_EMIT fireStartupModuleChanged();
	}
}


bool SettingsModel::getOnboardingShown() const
{
	return Env::getSingleton<VolatileSettings>()->onboardingShown();
}


void SettingsModel::setOnboardingShown(bool pOnboardingShown)
{
	if (getOnboardingShown() != pOnboardingShown)
	{
		Env::getSingleton<VolatileSettings>()->setOnboardingShown(pOnboardingShown);
		Q_EMIT fireStartupModuleChanged();
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
		Q_EMIT fireAutoStartChanged();
	}
}


bool SettingsModel::requestStoreFeedback() const
{
#ifdef Q_OS_ANDROID
	const bool startedByAuth = QJniObject::callStaticMethod<jboolean>("com/governikus/ausweisapp2/MainActivity", "isStartedByAuth");
	if (startedByAuth)
	{
		return false;
	}
#endif

	return Env::getSingleton<AppSettings>()->getGeneralSettings().isRequestStoreFeedback();
}


void SettingsModel::hideFutureStoreFeedbackDialogs() const
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setRequestStoreFeedback(false);
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
		Q_EMIT fireAutoCloseWindowAfterAuthenticationChanged();
	}
}


bool SettingsModel::isAutoRedirectAfterAuthentication() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isAutoRedirectAfterAuthentication();
}


void SettingsModel::setAutoRedirectAfterAuthentication(bool pEnabled)
{
	if (isAutoRedirectAfterAuthentication() != pEnabled)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setAutoRedirectAfterAuthentication(pEnabled);
		Q_EMIT fireAutoRedirectAfterAuthenticationChanged();
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
		Q_EMIT fireAutoUpdateCheckChanged();
	}
}


bool SettingsModel::isTrayIconEnabled() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isTrayIconEnabled();
}


void SettingsModel::setTrayIconEnabled(bool pTrayIconEnabled)
{
	if (isTrayIconEnabled() != pTrayIconEnabled)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setTrayIconEnabled(pTrayIconEnabled);
		Q_EMIT fireTrayIconEnabledChanged();
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
		Q_EMIT fireTransportPinReminderChanged();
	}
}


bool SettingsModel::isShowInAppNotifications() const
{

	return Env::getSingleton<AppSettings>()->getGeneralSettings().isShowInAppNotifications();
}


void SettingsModel::setShowInAppNotifications(bool pShowInAppNotifications) const
{
	if (isShowInAppNotifications() != pShowInAppNotifications)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setShowInAppNotifications(pShowInAppNotifications);
	}
}


void SettingsModel::updateAppcast() const
{
	Env::getSingleton<Service>()->updateAppcast();
}


AppUpdateDataModel* SettingsModel::getAppUpdateData() const
{
	auto* dataModel = Env::getSingleton<AppUpdateDataModel>();
	QQmlEngine::setObjectOwnership(dataModel, QQmlEngine::CppOwnership);
	return dataModel;
}


QUrl SettingsModel::getCustomProxyUrl() const
{
	const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	QUrl url;
	switch (settings.getCustomProxyType())
	{
		case QNetworkProxy::Socks5Proxy:
			url.setScheme(QStringLiteral("socks5"));
			break;

		case QNetworkProxy::HttpProxy:
			url.setScheme(QStringLiteral("http"));
			break;

		default:
			url.setScheme(QStringLiteral("unknown"));
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


bool SettingsModel::isUseCustomProxy() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().useCustomProxy();
}


void SettingsModel::setUseCustomProxy(bool pUseCustomProxy) const
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setUseCustomProxy(pUseCustomProxy);
}


bool SettingsModel::isUseSystemFont() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isUseSystemFont();
}


void SettingsModel::setUseSystemFont(bool pUseSystemFont) const
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setUseSystemFont(pUseSystemFont);
}


bool SettingsModel::isUseAnimations() const
{
	return Env::getSingleton<AppSettings>()->getGeneralSettings().isUseAnimations();
}


void SettingsModel::setUseAnimations(bool pUseAnimations) const
{
	if (isUseAnimations() != pUseAnimations)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		settings.setUseAnimations(pUseAnimations);
	}
}


SettingsModel::ModeOption SettingsModel::getDarkMode() const
{
	return Enum<ModeOption>::fromString(
			Env::getSingleton<AppSettings>()->getGeneralSettings().getDarkMode(),
			ModeOption::OFF);
}


void SettingsModel::setDarkMode(ModeOption pMode) const
{
	Env::getSingleton<AppSettings>()->getGeneralSettings().setDarkMode(getEnumName(pMode));
}


ReaderManagerPluginType SettingsModel::getPreferredTechnology() const
{
	const auto& technologyString = Env::getSingleton<AppSettings>()->getGeneralSettings().getPreferredTechnology();
	const auto technology = Enum<ReaderManagerPluginType>::fromString(technologyString, ReaderManagerPluginType::UNKNOWN);
	const auto simulator = Env::getSingleton<AppSettings>()->getSimulatorSettings().isEnabled();
	if (technology == ReaderManagerPluginType::UNKNOWN || (technology == ReaderManagerPluginType::SIMULATOR && !simulator))
	{
		const auto& pluginInfo = Env::getSingleton<ReaderManager>()->getPluginInfo(ReaderManagerPluginType::NFC);
		if (pluginInfo.isAvailable())
		{
			return ReaderManagerPluginType::NFC;
		}

		return ReaderManagerPluginType::REMOTE_IFD;
	}

	return technology;
}


void SettingsModel::setPreferredTechnology(ReaderManagerPluginType pTechnology) const
{
	auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	settings.setPreferredTechnology(getEnumName(pTechnology));
}


void SettingsModel::resetHideableDialogs() const
{
#ifndef QT_NO_DEBUG
	GeneralSettings& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	settings.setTransportPinReminder(true);
	settings.setRemindUserToClose(true);
	settings.setRequestStoreFeedback(true);
	settings.setStartupModule(QString());
	settings.setShowOnboarding(true);
#endif
}


bool SettingsModel::getShowBetaTesting() const
{
	return
#ifndef QT_NO_DEBUG
		mShowBetaTesting &&
#endif
		VersionNumber::getApplicationVersion().isDeveloperVersion();
}


void SettingsModel::setShowBetaTesting(bool pShowBetaTesting)
{
	if (mShowBetaTesting != pShowBetaTesting)
	{
		mShowBetaTesting = pShowBetaTesting;
		Q_EMIT fireDeveloperOptionsChanged();
	}
}


QString SettingsModel::getAppendTransportPin() const
{
#ifdef QT_NO_DEBUG
	return QString();

#else
	return qEnvironmentVariable("APPEND_TRANSPORT_PIN");

#endif
}


void SettingsModel::setAppendTransportPin(const QString& pNumber)
{
#ifdef QT_NO_DEBUG
	Q_UNUSED(pNumber)
#else
	if (pNumber.isEmpty())
	{
		qunsetenv("APPEND_TRANSPORT_PIN");
	}
	else
	{
		qputenv("APPEND_TRANSPORT_PIN", pNumber.toLocal8Bit());
	}
	Q_EMIT fireDeveloperOptionsChanged();
#endif
}
