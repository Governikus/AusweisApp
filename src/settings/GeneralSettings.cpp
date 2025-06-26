/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "GeneralSettings.h"

#include "AutoStart.h"
#include "Env.h"
#include "LanguageLoader.h"
#include "Randomizer.h"
#include "VolatileSettings.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <utility>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(settings)


namespace
{
SETTINGS_NAME(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION, "persistentSettingsVersion")
SETTINGS_NAME(SETTINGS_NAME_AUTO_CLOSE_WINDOW, "autoCloseWindow")
SETTINGS_NAME(SETTINGS_NAME_AUTO_REDIRECT, "autoRedirect")
SETTINGS_NAME(SETTINGS_NAME_UI_STARTUP_MODULE, "uiStartupModule")
SETTINGS_NAME(SETTINGS_NAME_SHOW_ONBOARDING, "showOnboarding")
SETTINGS_NAME(SETTINGS_NAME_REMIND_USER_TO_CLOSE, "remindToClose")
SETTINGS_NAME(SETTINGS_NAME_REMIND_USER_OF_AUTO_REDIRECT, "remindOfAutoRedirect")
SETTINGS_NAME(SETTINGS_NAME_TRANSPORT_PIN_REMINDER, "transportPinReminder")
SETTINGS_NAME(SETTINGS_NAME_DEVELOPER_OPTIONS, "developerOptions")
SETTINGS_NAME(SETTINGS_NAME_DEVELOPER_MODE, "developerMode")
SETTINGS_NAME(SETTINGS_NAME_USE_SELF_AUTH_TEST_URI, "selfauthTestUri")
SETTINGS_NAME(SETTINGS_NAME_LANGUAGE, "language")
SETTINGS_NAME(SETTINGS_NAME_DEVICE_SURVEY_PENDING, "deviceSurveyPending")
SETTINGS_NAME(SETTINGS_NAME_PREFERRED_TECHNOLOGY, "preferredTechnology")
SETTINGS_NAME(SETTINGS_NAME_IN_APP_NOTIFICATIONS, "showInAppNotifications")
SETTINGS_NAME(SETTINGS_NAME_REQUEST_STORE_FEEDBACK, "requestStoreFeedback")
SETTINGS_NAME(SETTINGS_NAME_AUTO, "autoUpdateCheck")
SETTINGS_NAME(SETTINGS_NAME_VISUAL_PRIVACY, "visualPrivacy")
SETTINGS_NAME(SETTINGS_NAME_SHUFFLE_SCREEN_KEYBOARD, "shuffleScreenKeyboard")
SETTINGS_NAME(SETTINGS_NAME_CUSTOM_PROXY_HOST, "customProxyHost")
SETTINGS_NAME(SETTINGS_NAME_CUSTOM_PROXY_PORT, "customProxyPort")
SETTINGS_NAME(SETTINGS_NAME_CUSTOM_PROXY_TYPE, "customProxyType")
SETTINGS_NAME(SETTINGS_NAME_USE_CUSTOM_PROXY, "useCustomProxy")
SETTINGS_NAME(SETTINGS_NAME_DARK_MODE, "darkMode")
SETTINGS_NAME(SETTINGS_NAME_USE_SYSTEM_FONT, "useSystemFont")
SETTINGS_NAME(SETTINGS_NAME_ANIMATIONS, "animations")
SETTINGS_NAME(SETTINGS_NAME_ENABLE_CAN_ALLOWED, "enableCanAllowed")
SETTINGS_NAME(SETTINGS_NAME_SKIP_RIGHTS_ON_CAN_ALLOWED, "skipRightsOnCanAllowed")
SETTINGS_NAME(SETTINGS_NAME_IFD_SERVICE_TOKEN, "ifdServiceToken")
SETTINGS_NAME(SETTINGS_NAME_SMART_AVAILABLE, "smartAvailable")
SETTINGS_NAME(SETTINGS_NAME_TRAY_ICON_ENABLED, "enableTrayIcon")
} // namespace

GeneralSettings::GeneralSettings()
	: GeneralSettings(getStore())
{
}


GeneralSettings::GeneralSettings(QSharedPointer<QSettings> pStore)
	: AbstractSettings()
	, mAutoStart(false)
	, mStore(std::move(pStore))
	, mIsNewAppVersion(false)
{
	// With 2.0.0 the option to change the screen orientation was removed
	mStore->remove(QAnyStringView("screenOrientation"));

	// With 2.0.0 the history was removed
	const QAnyStringView history("history");
	if (mStore->childGroups().contains(history))
	{
		mStore->beginGroup(history);
		mStore->remove(QString());
		mStore->endGroup();
	}

	// With 2.1.0 the skipVersion was removed
	mStore->remove(QAnyStringView("skipVersion"));

	// With 2.2.0 the keylessPassword was removed
	mStore->remove(QAnyStringView("keylessPassword"));

	// With 2.3.0 the technology chooser was removed
	mStore->remove(QAnyStringView("lastTechnology"));

	const bool isNewInstallation = getPersistentSettingsVersion().isEmpty();
	if (isNewInstallation)
	{
		setAutoStartInternal(autoStartDefault());
	}
	mIsNewAppVersion = !isNewInstallation && getPersistentSettingsVersion() != QCoreApplication::applicationVersion();

#ifdef QT_NO_DEBUG
	mAutoStart = AutoStart::enabled();
#else
	mAutoStart = autoStartDefault();
#endif

	const auto trayIconSettingAdded = VersionNumber(QStringLiteral("2.3.0"));
	if (!isNewInstallation)
	{
		const auto& oldApplicationVersion = VersionNumber(getPersistentSettingsVersion());
		if (oldApplicationVersion < trayIconSettingAdded
				&& VersionNumber::getApplicationVersion() >= trayIconSettingAdded)
		{
			setTrayIconEnabled(isAutoStart());
		}
	}

	mStore->setValue(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION(), QCoreApplication::applicationVersion());
	save(mStore);
}


bool GeneralSettings::isShowNotificationsOsDefault() const
{
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
	return false;

#else
	return true;

#endif
}


bool GeneralSettings::isAutoStartAvailable() const
{
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
	return !Env::getSingleton<VolatileSettings>()->isUsedAsSDK();

#else
	// Linux: maybe XDG_CONFIG_HOME/autostart or XDG_CONFIG_DIRS/autostart can be added
	return false;

#endif
}


bool GeneralSettings::isAutoStart() const
{
	return isAutoStartAvailable() && mAutoStart;
}


bool GeneralSettings::autoStartIsSetByAdmin() const
{
	return AutoStart::isSetByAdmin();
}


bool GeneralSettings::isTrayIconEnabled() const
{
	return mStore->value(SETTINGS_NAME_TRAY_ICON_ENABLED(), trayIconDefault()).toBool();
}


void GeneralSettings::setTrayIconEnabled(bool pTrayIconEnabled)
{
	if (pTrayIconEnabled != isTrayIconEnabled())
	{
		mStore->setValue(SETTINGS_NAME_TRAY_ICON_ENABLED(), pTrayIconEnabled);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


void GeneralSettings::setAutoStartInternal(bool pAutoStart)
{
	if (pAutoStart != mAutoStart)
	{
#ifdef QT_NO_DEBUG
		if (AutoStart::set(pAutoStart))
#endif
		{
			mAutoStart = pAutoStart;
			Q_EMIT fireSettingsChanged();
		}
	}
}


void GeneralSettings::setAutoStart(bool pAutoStart)
{
	if (!isAutoStartAvailable())
	{
		qCDebug(settings) << "AutoStart is not available";
		return;
	}

	setAutoStartInternal(pAutoStart);
}


QString GeneralSettings::getPersistentSettingsVersion() const
{
	return mStore->value(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION(), QString()).toString();
}


bool GeneralSettings::isNewAppVersion() const
{
	return mIsNewAppVersion;
}


bool GeneralSettings::isAutoCloseWindowAfterAuthentication() const
{
	return mStore->value(SETTINGS_NAME_AUTO_CLOSE_WINDOW(), true).toBool();
}


void GeneralSettings::setAutoCloseWindowAfterAuthentication(bool pAutoClose)
{
	if (pAutoClose != isAutoCloseWindowAfterAuthentication())
	{
		mStore->setValue(SETTINGS_NAME_AUTO_CLOSE_WINDOW(), pAutoClose);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isAutoRedirectAfterAuthentication() const
{
	return mStore->value(SETTINGS_NAME_AUTO_REDIRECT(), true).toBool();
}


void GeneralSettings::setAutoRedirectAfterAuthentication(bool pAutoRedirect)
{
	if (pAutoRedirect != isAutoRedirectAfterAuthentication())
	{
		mStore->setValue(SETTINGS_NAME_AUTO_REDIRECT(), pAutoRedirect);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


QString GeneralSettings::getStartupModule() const
{
	return mStore->value(SETTINGS_NAME_UI_STARTUP_MODULE(), QString()).toString();
}


void GeneralSettings::setStartupModule(const QString& pModule)
{
	if (getStartupModule() != pModule)
	{
		mStore->setValue(SETTINGS_NAME_UI_STARTUP_MODULE(), pModule);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::getShowOnboarding() const
{
	return mStore->value(SETTINGS_NAME_SHOW_ONBOARDING(), true).toBool();
}


void GeneralSettings::setShowOnboarding(bool pShowOnboarding)
{
	if (getShowOnboarding() != pShowOnboarding)
	{
		mStore->setValue(SETTINGS_NAME_SHOW_ONBOARDING(), pShowOnboarding);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isRemindUserToClose() const
{
	return mStore->value(SETTINGS_NAME_REMIND_USER_TO_CLOSE(), true).toBool();
}


void GeneralSettings::setRemindUserToClose(bool pRemindUser)
{
	if (pRemindUser != isRemindUserToClose())
	{
		mStore->setValue(SETTINGS_NAME_REMIND_USER_TO_CLOSE(), pRemindUser);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isRemindUserOfAutoRedirect() const
{
	return mStore->value(SETTINGS_NAME_REMIND_USER_OF_AUTO_REDIRECT(), true).toBool();
}


void GeneralSettings::setRemindUserOfAutoRedirect(bool pRemindUser)
{
	if (pRemindUser != isRemindUserOfAutoRedirect())
	{
		mStore->setValue(SETTINGS_NAME_REMIND_USER_OF_AUTO_REDIRECT(), pRemindUser);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isTransportPinReminder() const
{
	return mStore->value(SETTINGS_NAME_TRANSPORT_PIN_REMINDER(), true).toBool();
}


void GeneralSettings::setTransportPinReminder(bool pTransportPinReminder)
{
	if (pTransportPinReminder != isTransportPinReminder())
	{
		mStore->setValue(SETTINGS_NAME_TRANSPORT_PIN_REMINDER(), pTransportPinReminder);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isDeveloperOptions() const
{
	return mStore->value(SETTINGS_NAME_DEVELOPER_OPTIONS(), false).toBool();
}


void GeneralSettings::setDeveloperOptions(bool pEnabled)
{
	if (pEnabled != isDeveloperOptions())
	{
		mStore->setValue(SETTINGS_NAME_DEVELOPER_OPTIONS(), pEnabled);
		save(mStore);
		Q_EMIT fireDeveloperOptionsChanged();
	}
}


bool GeneralSettings::isDeveloperMode() const
{
	if (Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		return Env::getSingleton<VolatileSettings>()->isDeveloperMode();
	}

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (!isDeveloperOptions())
	{
		return false;
	}
#endif

	return mStore->value(SETTINGS_NAME_DEVELOPER_MODE(), false).toBool();
}


void GeneralSettings::setDeveloperMode(bool pEnabled)
{
	if (pEnabled != isDeveloperMode())
	{
		mStore->setValue(SETTINGS_NAME_DEVELOPER_MODE(), pEnabled);
		save(mStore);
		Q_EMIT fireDeveloperOptionsChanged();
		Q_EMIT fireShowInAppNotificationsChanged();
	}
}


bool GeneralSettings::useSelfAuthTestUri() const
{
	return mStore->value(SETTINGS_NAME_USE_SELF_AUTH_TEST_URI(), false).toBool();
}


void GeneralSettings::setUseSelfauthenticationTestUri(bool pUse)
{
	if (pUse != useSelfAuthTestUri())
	{
		mStore->setValue(SETTINGS_NAME_USE_SELF_AUTH_TEST_URI(), pUse);
		save(mStore);
		Q_EMIT fireDeveloperOptionsChanged();
	}
}


QLocale::Language GeneralSettings::getLanguage() const
{
	const QString loadedLanguage = mStore->value(SETTINGS_NAME_LANGUAGE(), QString()).toString();
	if (loadedLanguage.isEmpty())
	{
		return QLocale::C;
	}

	return QLocale(loadedLanguage).language();
}


void GeneralSettings::setLanguage(const QLocale::Language pLanguage)
{
	if (pLanguage != getLanguage())
	{
		mStore->setValue(SETTINGS_NAME_LANGUAGE(), pLanguage == QLocale::C ? QString() : LanguageLoader::getLocaleCode(QLocale(pLanguage)));
		save(mStore);
		Q_EMIT fireLanguageChanged();
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::askForDeviceSurvey() const
{
	return !mStore->contains(SETTINGS_NAME_DEVICE_SURVEY_PENDING());
}


bool GeneralSettings::isDeviceSurveyPending() const
{
	return mStore->value(SETTINGS_NAME_DEVICE_SURVEY_PENDING(), false).toBool();
}


void GeneralSettings::setDeviceSurveyPending(bool pDeviceSurveyPending)
{
	if (askForDeviceSurvey() || pDeviceSurveyPending != isDeviceSurveyPending())
	{
		mStore->setValue(SETTINGS_NAME_DEVICE_SURVEY_PENDING(), pDeviceSurveyPending);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::askForStoreFeedback() const
{
	return !mStore->contains(SETTINGS_NAME_REQUEST_STORE_FEEDBACK());
}


bool GeneralSettings::isRequestStoreFeedback() const
{
	return mStore->value(SETTINGS_NAME_REQUEST_STORE_FEEDBACK(), false).toBool();
}


void GeneralSettings::setRequestStoreFeedback(bool pRequest)
{
	if (askForStoreFeedback() || pRequest != isRequestStoreFeedback())
	{
		mStore->setValue(SETTINGS_NAME_REQUEST_STORE_FEEDBACK(), pRequest);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


QString GeneralSettings::getPreferredTechnology() const
{
	return mStore->value(SETTINGS_NAME_PREFERRED_TECHNOLOGY(), QString()).toString();
}


void GeneralSettings::setPreferredTechnology(const QString& pTechnology)
{
	if (pTechnology != getPreferredTechnology())
	{
		mStore->setValue(SETTINGS_NAME_PREFERRED_TECHNOLOGY(), pTechnology);
		save(mStore);
		Q_EMIT firePreferredTechnologyChanged();
	}
}


bool GeneralSettings::isAutoUpdateAvailable() const
{
#if !defined(QT_NO_DEBUG) || defined(Q_OS_WIN)
	return true;

#else
	return false;

#endif
}


bool GeneralSettings::isAutoUpdateCheck() const
{
	if (autoUpdateCheckIsSetByAdmin())
	{
		mStore->remove(SETTINGS_NAME_AUTO());
		save(mStore);
	}

#if defined(Q_OS_WIN)
	return mStore->value(SETTINGS_NAME_AUTO(), true).toBool();

#else
	return mStore->value(SETTINGS_NAME_AUTO(), false).toBool();

#endif
}


bool GeneralSettings::autoUpdateCheckIsSetByAdmin() const
{
	return getStore(QSettings::Scope::SystemScope)->contains(SETTINGS_NAME_AUTO());
}


void GeneralSettings::setAutoUpdateCheck(bool pAutoUpdateCheck)
{
	if (!autoUpdateCheckIsSetByAdmin() && pAutoUpdateCheck != isAutoUpdateCheck())
	{
		mStore->setValue(SETTINGS_NAME_AUTO(), pAutoUpdateCheck);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isVisualPrivacy() const
{
	return mStore->value(SETTINGS_NAME_VISUAL_PRIVACY(), false).toBool();
}


void GeneralSettings::setVisualPrivacy(bool pVisualPrivacy)
{
	if (pVisualPrivacy != isVisualPrivacy())
	{
		mStore->setValue(SETTINGS_NAME_VISUAL_PRIVACY(), pVisualPrivacy);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isShuffleScreenKeyboard() const
{
	return mStore->value(SETTINGS_NAME_SHUFFLE_SCREEN_KEYBOARD(), false).toBool();
}


void GeneralSettings::setShuffleScreenKeyboard(bool pShuffleScreenKeyboard)
{
	if (pShuffleScreenKeyboard != isShuffleScreenKeyboard())
	{
		mStore->setValue(SETTINGS_NAME_SHUFFLE_SCREEN_KEYBOARD(), pShuffleScreenKeyboard);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isEnableCanAllowed() const
{
	return mStore->value(SETTINGS_NAME_ENABLE_CAN_ALLOWED(), false).toBool();
}


void GeneralSettings::setEnableCanAllowed(bool pEnableCanAllowed)
{
	if (pEnableCanAllowed != isEnableCanAllowed())
	{
		mStore->setValue(SETTINGS_NAME_ENABLE_CAN_ALLOWED(), pEnableCanAllowed);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isSkipRightsOnCanAllowed() const
{
	if (!isEnableCanAllowed())
	{
		return false;
	}
	return mStore->value(SETTINGS_NAME_SKIP_RIGHTS_ON_CAN_ALLOWED(), false).toBool();
}


void GeneralSettings::setSkipRightsOnCanAllowed(bool pSkipRightsOnCanAllowed)
{
	if (pSkipRightsOnCanAllowed != isSkipRightsOnCanAllowed())
	{
		mStore->setValue(SETTINGS_NAME_SKIP_RIGHTS_ON_CAN_ALLOWED(), pSkipRightsOnCanAllowed);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isShowInAppNotifications() const
{
	if (isDeveloperMode())
	{
		return true;
	}

	return mStore->value(SETTINGS_NAME_IN_APP_NOTIFICATIONS(), isShowNotificationsOsDefault()).toBool();
}


void GeneralSettings::setShowInAppNotifications(bool pShowInAppNotifications)
{
	if (pShowInAppNotifications != isShowInAppNotifications())
	{
		mStore->setValue(SETTINGS_NAME_IN_APP_NOTIFICATIONS(), pShowInAppNotifications);
		save(mStore);
		Q_EMIT fireShowInAppNotificationsChanged();
	}
}


bool GeneralSettings::isCustomProxyHost() const
{
	return !getCustomProxyHost().isEmpty();
}


QString GeneralSettings::getCustomProxyHost() const
{
	return mStore->value(SETTINGS_NAME_CUSTOM_PROXY_HOST(), QString()).toString();
}


bool GeneralSettings::isCustomProxyType() const
{
	return getCustomProxyType() != QNetworkProxy::ProxyType::NoProxy;
}


QNetworkProxy::ProxyType GeneralSettings::getCustomProxyType() const
{
	QNetworkProxy::ProxyType result;
	auto value = mStore->value(SETTINGS_NAME_CUSTOM_PROXY_TYPE(), QString()).toString().toLower();
	if (value == QLatin1String("socks5"))
	{
		result = QNetworkProxy::ProxyType::Socks5Proxy;
	}
	else if (value == QLatin1String("http"))
	{
		result = QNetworkProxy::ProxyType::HttpProxy;
	}
	else
	{
		result = QNetworkProxy::ProxyType::NoProxy;
	}
	return result;
}


bool GeneralSettings::isCustomProxyPort() const
{
	return getCustomProxyPort() != 0;
}


quint16 GeneralSettings::getCustomProxyPort() const
{
	return static_cast<quint16>(mStore->value(SETTINGS_NAME_CUSTOM_PROXY_PORT(), 0).toInt());
}


bool GeneralSettings::customProxyAttributesPresent() const
{
	return isCustomProxyHost() && isCustomProxyType() && isCustomProxyPort();
}


bool GeneralSettings::useCustomProxy() const
{
	bool useCustomProxy = mStore->value(SETTINGS_NAME_USE_CUSTOM_PROXY(), true).toBool();
	return useCustomProxy && customProxyAttributesPresent();
}


void GeneralSettings::setUseCustomProxy(bool pUseCustomProxy)
{
	if (useCustomProxy() != pUseCustomProxy)
	{
		mStore->setValue(SETTINGS_NAME_USE_CUSTOM_PROXY(), pUseCustomProxy);
		save(mStore);
		Q_EMIT fireProxyChanged();
	}
}


bool GeneralSettings::isUseSystemFont() const
{
	return mStore->value(SETTINGS_NAME_USE_SYSTEM_FONT(), false).toBool();
}


void GeneralSettings::setUseSystemFont(bool pUseSystemFont)
{
	if (isUseSystemFont() != pUseSystemFont)
	{
		mStore->setValue(SETTINGS_NAME_USE_SYSTEM_FONT(), pUseSystemFont);
		save(mStore);
		Q_EMIT fireUseSystemFontChanged();
	}
}


bool GeneralSettings::isUseAnimations() const
{
	return mStore->value(SETTINGS_NAME_ANIMATIONS(), true).toBool();
}


void GeneralSettings::setUseAnimations(bool pUseAnimations)
{
	if (isUseAnimations() != pUseAnimations)
	{
		mStore->setValue(SETTINGS_NAME_ANIMATIONS(), pUseAnimations);
		save(mStore);
		Q_EMIT fireUseAnimationsChanged();
	}
}


QString GeneralSettings::getDarkMode() const
{
	return mStore->value(SETTINGS_NAME_DARK_MODE(), QString()).toString();
}


void GeneralSettings::setDarkMode(const QString& pMode)
{
	if (getDarkMode() != pMode)
	{
		mStore->setValue(SETTINGS_NAME_DARK_MODE(), pMode);
		save(mStore);
		Q_EMIT fireDarkModeChanged();
	}
}


QString GeneralSettings::getIfdServiceToken()
{
	if (!mStore->contains(SETTINGS_NAME_IFD_SERVICE_TOKEN()))
	{
		auto serviceToken = Randomizer::getInstance().createUuid().toString();
		mStore->setValue(SETTINGS_NAME_IFD_SERVICE_TOKEN(), serviceToken);
		save(mStore);
	}

	return mStore->value(SETTINGS_NAME_IFD_SERVICE_TOKEN(), QString()).toString();
}


bool GeneralSettings::doSmartUpdate() const
{
	return !mStore->contains(SETTINGS_NAME_SMART_AVAILABLE());
}


bool GeneralSettings::isSmartAvailable() const
{
	return mStore->value(SETTINGS_NAME_SMART_AVAILABLE(), false).toBool();
}


void GeneralSettings::setSmartAvailable(bool pSmartAvailable)
{
	const bool valueChanged = pSmartAvailable != isSmartAvailable();
	if (doSmartUpdate() || valueChanged)
	{
		mStore->setValue(SETTINGS_NAME_SMART_AVAILABLE(), pSmartAvailable);
		save(mStore);
	}
	if (valueChanged)
	{
		Q_EMIT fireSmartAvailableChanged(pSmartAvailable);
	}
}


#ifdef Q_OS_WIN
void GeneralSettings::migrateSettings()
{
	if (!isAutoStartAvailable())
	{
		qCDebug(settings) << "AutoStart is not available, migration not possible.";
		return;
	}

	if (AutoStart::removeOldAutostart())
	{
		setAutoStart(true);
	}
}


#endif
