/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Contains the method definitions of the GeneralSettings class.
 */

#include "GeneralSettings.h"

#include "AutoStart.h"
#include "BuildHelper.h"
#include "Env.h"
#include "LanguageLoader.h"
#include "Randomizer.h"
#include "VolatileSettings.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QOperatingSystemVersion>
#include <utility>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(settings)

namespace
{
SETTINGS_NAME(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION, "persistentSettingsVersion")
SETTINGS_NAME(SETTINGS_NAME_SKIP_VERSION, "skipVersion")
SETTINGS_NAME(SETTINGS_NAME_AUTO_CLOSE_WINDOW, "autoCloseWindow")
SETTINGS_NAME(SETTINGS_NAME_UI_STARTUP_MODULE, "uiStartupModule")
SETTINGS_NAME(SETTINGS_NAME_REMIND_USER_TO_CLOSE, "remindToClose")
SETTINGS_NAME(SETTINGS_NAME_TRANSPORT_PIN_REMINDER, "transportPinReminder")
SETTINGS_NAME(SETTINGS_NAME_DEVELOPER_OPTIONS, "developerOptions")
SETTINGS_NAME(SETTINGS_NAME_DEVELOPER_MODE, "developerMode")
SETTINGS_NAME(SETTINGS_NAME_USE_SELF_AUTH_TEST_URI, "selfauthTestUri")
SETTINGS_NAME(SETTINGS_NAME_SIMULATOR, "simulator")
SETTINGS_NAME(SETTINGS_NAME_LANGUAGE, "language")
SETTINGS_NAME(SETTINGS_NAME_SCREEN_ORIENTATION, "screenOrientation")
SETTINGS_NAME(SETTINGS_NAME_DEVICE_SURVEY_PENDING, "deviceSurveyPending")
SETTINGS_NAME(SETTINGS_NAME_LAST_READER_PLUGIN_TYPE, "lastTechnology")
SETTINGS_NAME(SETTINGS_NAME_IN_APP_NOTIFICATIONS, "showInAppNotifications")
SETTINGS_NAME(SETTINGS_NAME_REQUEST_STORE_FEEDBACK, "requestStoreFeedback")
SETTINGS_NAME(SETTINGS_NAME_AUTO, "autoUpdateCheck")
SETTINGS_NAME(SETTINGS_NAME_KEYLESS_PASSWORD, "keylessPassword")
SETTINGS_NAME(SETTINGS_NAME_VISUAL_PRIVACY, "visualPrivacy")
SETTINGS_NAME(SETTINGS_NAME_SHUFFLE_SCREEN_KEYBOARD, "shuffleScreenKeyboard")
SETTINGS_NAME(SETTINGS_NAME_CUSTOM_PROXY_HOST, "customProxyHost")
SETTINGS_NAME(SETTINGS_NAME_CUSTOM_PROXY_PORT, "customProxyPort")
SETTINGS_NAME(SETTINGS_NAME_CUSTOM_PROXY_TYPE, "customProxyType")
SETTINGS_NAME(SETTINGS_NAME_USE_CUSTOM_PROXY, "useCustomProxy")
SETTINGS_NAME(SETTINGS_NAME_ENABLE_CAN_ALLOWED, "enableCanAllowed")
SETTINGS_NAME(SETTINGS_NAME_SKIP_RIGHTS_ON_CAN_ALLOWED, "skipRightsOnCanAllowed")
SETTINGS_NAME(SETTINGS_NAME_IFD_SERVICE_TOKEN, "ifdServiceToken")
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
	const bool isNewInstallation = getPersistentSettingsVersion().isEmpty();
	if (isNewInstallation)
	{
		setAutoStartInternal(GENERAL_SETTINGS_DEFAULT_AUTOSTART);
	}
	mIsNewAppVersion = !isNewInstallation && getPersistentSettingsVersion() != QCoreApplication::applicationVersion();

#ifdef QT_NO_DEBUG
	mAutoStart = AutoStart::enabled();
#else
	mAutoStart = GENERAL_SETTINGS_DEFAULT_AUTOSTART;
#endif

	mStore->setValue(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION(), QCoreApplication::applicationVersion());
	save(mStore);
}


bool GeneralSettings::isShowNotificationsOsDefault() const
{
#if defined(Q_OS_WIN)
	return QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows10;

#elif defined(Q_OS_MACOS)
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


QString GeneralSettings::getSkipVersion() const
{
	return mStore->value(SETTINGS_NAME_SKIP_VERSION(), QString()).toString();
}


void GeneralSettings::skipVersion(const QString& pVersion)
{
	mStore->setValue(SETTINGS_NAME_SKIP_VERSION(), pVersion);
	save(mStore);
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
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (!isDeveloperOptions())
	{
		return false;
	}
#endif

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


bool GeneralSettings::isSimulatorEnabled() const
{
#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (!isDeveloperOptions())
	{
		return false;
	}
#endif

	return mStore->value(SETTINGS_NAME_SIMULATOR(), false).toBool();
}


void GeneralSettings::setSimulatorEnabled(bool pEnabled)
{
	if (pEnabled != isSimulatorEnabled())
	{
		mStore->setValue(SETTINGS_NAME_SIMULATOR(), pEnabled);
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


QString GeneralSettings::getScreenOrientation() const
{
	return mStore->value(SETTINGS_NAME_SCREEN_ORIENTATION(), QStringLiteral("auto")).toString();
}


void GeneralSettings::setScreenOrientation(const QString& pScreenOrientation)
{
	if (pScreenOrientation != getScreenOrientation())
	{
		mStore->setValue(SETTINGS_NAME_SCREEN_ORIENTATION(), pScreenOrientation);
		save(mStore);
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


QString GeneralSettings::getLastReaderPluginType() const
{
	return mStore->value(SETTINGS_NAME_LAST_READER_PLUGIN_TYPE(), QString()).toString();
}


void GeneralSettings::setLastReaderPluginType(const QString& pLastReaderPluginType)
{
	if (pLastReaderPluginType != getLastReaderPluginType())
	{
		mStore->setValue(SETTINGS_NAME_LAST_READER_PLUGIN_TYPE(), pLastReaderPluginType);
		save(mStore);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isAutoUpdateAvailable() const
{
#if !defined(QT_NO_DEBUG) || defined(Q_OS_WIN) || defined(Q_OS_MACOS)
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

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
	return mStore->value(SETTINGS_NAME_AUTO(), true).toBool();

#else
	return mStore->value(SETTINGS_NAME_AUTO(), false).toBool();

#endif
}


bool GeneralSettings::autoUpdateCheckIsSetByAdmin() const
{
	return QSettings(QSettings::Scope::SystemScope).contains(SETTINGS_NAME_AUTO());
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


bool GeneralSettings::isUseScreenKeyboard() const
{
	return mStore->value(SETTINGS_NAME_KEYLESS_PASSWORD(), false).toBool();
}


void GeneralSettings::setUseScreenKeyboard(bool pUseScreenKeyboard)
{
	if (pUseScreenKeyboard != isUseScreenKeyboard())
	{
		mStore->setValue(SETTINGS_NAME_KEYLESS_PASSWORD(), pUseScreenKeyboard);
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
