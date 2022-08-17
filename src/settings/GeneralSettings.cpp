/*!
 * \brief Contains the method definitions of the GeneralSettings class.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
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
SETTINGS_NAME(SETTINGS_NAME_SHOW_SETUP_ASSISTANT, "showSetupAssistant")
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


GeneralSettings::GeneralSettings(QSharedPointer<QSettings> pStoreGeneral)
	: AbstractSettings()
	, mAutoStart(false)
	, mStoreGeneral(std::move(pStoreGeneral))
	, mIsNewAppVersion(false)
{
	{
		// With 1.22.1 the showSetupAssistant was replaced with a general startupModule, so migrate the settings.
		if (!mStoreGeneral->contains(SETTINGS_NAME_UI_STARTUP_MODULE()))
		{
			if (!mStoreGeneral->value(SETTINGS_NAME_SHOW_SETUP_ASSISTANT(), true).toBool())
			{
				setStartupModule(QStringLiteral("DEFAULT"));
			}
			mStoreGeneral->remove(SETTINGS_NAME_SHOW_SETUP_ASSISTANT());
		}

		// With 1.22.0 the old widgets was removed, no need for UI selector
		mStoreGeneral->remove(QStringLiteral("selectedUi"));
		mStoreGeneral->remove(QStringLiteral("showNewUiHint"));
	}

	// Check if the key "autoCloseWindow" (introduced in changeset 199210b0b20c)
	// does not yet exist to detect a new installation. This key was the first one
	// in the settings general group.
	const bool isNewInstallation = getPersistentSettingsVersion().isEmpty();
	if (isNewInstallation)
	{
		setAutoStartInternal(GENERAL_SETTINGS_DEFAULT_AUTOSTART);
	}
	mIsNewAppVersion = !isNewInstallation && getPersistentSettingsVersion() != QCoreApplication::applicationVersion();
	if (isNewInstallation || mIsNewAppVersion)
	{
		mStoreGeneral->setValue(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION(), QCoreApplication::applicationVersion());
		mStoreGeneral->sync();
	}

#ifdef QT_NO_DEBUG
	mAutoStart = AutoStart::enabled();
#else
	mAutoStart = GENERAL_SETTINGS_DEFAULT_AUTOSTART;
#endif
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


void GeneralSettings::save()
{
	mStoreGeneral->setValue(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION(), QCoreApplication::applicationVersion());

	mStoreGeneral->sync();
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
	return mStoreGeneral->value(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION(), QString()).toString();
}


QString GeneralSettings::getSkipVersion() const
{
	return mStoreGeneral->value(SETTINGS_NAME_SKIP_VERSION(), QString()).toString();
}


void GeneralSettings::skipVersion(const QString& pVersion)
{
	mStoreGeneral->setValue(SETTINGS_NAME_SKIP_VERSION(), pVersion);
}


bool GeneralSettings::isNewAppVersion() const
{
	return mIsNewAppVersion;
}


bool GeneralSettings::isAutoCloseWindowAfterAuthentication() const
{
	return mStoreGeneral->value(SETTINGS_NAME_AUTO_CLOSE_WINDOW(), true).toBool();
}


void GeneralSettings::setAutoCloseWindowAfterAuthentication(bool pAutoClose)
{
	if (pAutoClose != isAutoCloseWindowAfterAuthentication())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_AUTO_CLOSE_WINDOW(), pAutoClose);
		Q_EMIT fireSettingsChanged();
	}
}


QString GeneralSettings::getStartupModule() const
{
	return mStoreGeneral->value(SETTINGS_NAME_UI_STARTUP_MODULE(), QString()).toString();
}


void GeneralSettings::setStartupModule(const QString& pModule)
{
	if (getStartupModule() != pModule)
	{
		mStoreGeneral->setValue(SETTINGS_NAME_UI_STARTUP_MODULE(), pModule);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isRemindUserToClose() const
{
	return mStoreGeneral->value(SETTINGS_NAME_REMIND_USER_TO_CLOSE(), true).toBool();
}


void GeneralSettings::setRemindUserToClose(bool pRemindUser)
{
	if (pRemindUser != isRemindUserToClose())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_REMIND_USER_TO_CLOSE(), pRemindUser);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isTransportPinReminder() const
{
	return mStoreGeneral->value(SETTINGS_NAME_TRANSPORT_PIN_REMINDER(), true).toBool();
}


void GeneralSettings::setTransportPinReminder(bool pTransportPinReminder)
{
	if (pTransportPinReminder != isTransportPinReminder())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_TRANSPORT_PIN_REMINDER(), pTransportPinReminder);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isDeveloperOptions() const
{
	return mStoreGeneral->value(SETTINGS_NAME_DEVELOPER_OPTIONS(), false).toBool();
}


void GeneralSettings::setDeveloperOptions(bool pEnabled)
{
	if (pEnabled != isDeveloperOptions())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_DEVELOPER_OPTIONS(), pEnabled);
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

	return mStoreGeneral->value(SETTINGS_NAME_DEVELOPER_MODE(), false).toBool();
}


void GeneralSettings::setDeveloperMode(bool pEnabled)
{
	if (pEnabled != isDeveloperMode())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_DEVELOPER_MODE(), pEnabled);
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

	return mStoreGeneral->value(SETTINGS_NAME_USE_SELF_AUTH_TEST_URI(), false).toBool();
}


void GeneralSettings::setUseSelfauthenticationTestUri(bool pUse)
{
	if (pUse != useSelfAuthTestUri())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_USE_SELF_AUTH_TEST_URI(), pUse);
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

	return mStoreGeneral->value(SETTINGS_NAME_SIMULATOR(), false).toBool();
}


void GeneralSettings::setSimulatorEnabled(bool pEnabled)
{
	if (pEnabled != isSimulatorEnabled())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_SIMULATOR(), pEnabled);
		Q_EMIT fireDeveloperOptionsChanged();
	}
}


QLocale::Language GeneralSettings::getLanguage() const
{
	const QString loadedLanguage = mStoreGeneral->value(SETTINGS_NAME_LANGUAGE(), QString()).toString();
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
		mStoreGeneral->setValue(SETTINGS_NAME_LANGUAGE(), pLanguage == QLocale::C ? QString() : LanguageLoader::getLocaleCode(QLocale(pLanguage)));
		Q_EMIT fireLanguageChanged();
		Q_EMIT fireSettingsChanged();
	}
}


QString GeneralSettings::getScreenOrientation() const
{
	return mStoreGeneral->value(SETTINGS_NAME_SCREEN_ORIENTATION(), QStringLiteral("auto")).toString();
}


void GeneralSettings::setScreenOrientation(const QString& pScreenOrientation)
{
	if (pScreenOrientation != getScreenOrientation())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_SCREEN_ORIENTATION(), pScreenOrientation);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::askForDeviceSurvey() const
{
	return !mStoreGeneral->contains(SETTINGS_NAME_DEVICE_SURVEY_PENDING());
}


bool GeneralSettings::isDeviceSurveyPending() const
{
	return mStoreGeneral->value(SETTINGS_NAME_DEVICE_SURVEY_PENDING(), false).toBool();
}


void GeneralSettings::setDeviceSurveyPending(bool pDeviceSurveyPending)
{
	if (askForDeviceSurvey() || pDeviceSurveyPending != isDeviceSurveyPending())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_DEVICE_SURVEY_PENDING(), pDeviceSurveyPending);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::askForStoreFeedback() const
{
	return !mStoreGeneral->contains(SETTINGS_NAME_REQUEST_STORE_FEEDBACK());
}


bool GeneralSettings::isRequestStoreFeedback() const
{
	return mStoreGeneral->value(SETTINGS_NAME_REQUEST_STORE_FEEDBACK(), false).toBool();
}


void GeneralSettings::setRequestStoreFeedback(bool pRequest)
{
	if (askForStoreFeedback() || pRequest != isRequestStoreFeedback())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_REQUEST_STORE_FEEDBACK(), pRequest);
		Q_EMIT fireSettingsChanged();
	}
}


QString GeneralSettings::getLastReaderPluginType() const
{
	return mStoreGeneral->value(SETTINGS_NAME_LAST_READER_PLUGIN_TYPE(), QString()).toString();
}


void GeneralSettings::setLastReaderPluginType(const QString& pLastReaderPluginType)
{
	if (pLastReaderPluginType != getLastReaderPluginType())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_LAST_READER_PLUGIN_TYPE(), pLastReaderPluginType);
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
		mStoreGeneral->remove(SETTINGS_NAME_AUTO());
	}

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
	return mStoreGeneral->value(SETTINGS_NAME_AUTO(), true).toBool();

#else
	return mStoreGeneral->value(SETTINGS_NAME_AUTO(), false).toBool();

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
		mStoreGeneral->setValue(SETTINGS_NAME_AUTO(), pAutoUpdateCheck);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isUseScreenKeyboard() const
{
	return mStoreGeneral->value(SETTINGS_NAME_KEYLESS_PASSWORD(), false).toBool();
}


void GeneralSettings::setUseScreenKeyboard(bool pUseScreenKeyboard)
{
	if (pUseScreenKeyboard != isUseScreenKeyboard())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_KEYLESS_PASSWORD(), pUseScreenKeyboard);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isVisualPrivacy() const
{
	return mStoreGeneral->value(SETTINGS_NAME_VISUAL_PRIVACY(), false).toBool();
}


void GeneralSettings::setVisualPrivacy(bool pVisualPrivacy)
{
	if (pVisualPrivacy != isVisualPrivacy())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_VISUAL_PRIVACY(), pVisualPrivacy);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isShuffleScreenKeyboard() const
{
	return mStoreGeneral->value(SETTINGS_NAME_SHUFFLE_SCREEN_KEYBOARD(), false).toBool();
}


void GeneralSettings::setShuffleScreenKeyboard(bool pShuffleScreenKeyboard)
{
	if (pShuffleScreenKeyboard != isShuffleScreenKeyboard())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_SHUFFLE_SCREEN_KEYBOARD(), pShuffleScreenKeyboard);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isEnableCanAllowed() const
{
	return mStoreGeneral->value(SETTINGS_NAME_ENABLE_CAN_ALLOWED(), false).toBool();
}


void GeneralSettings::setEnableCanAllowed(bool pEnableCanAllowed)
{
	if (pEnableCanAllowed != isEnableCanAllowed())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_ENABLE_CAN_ALLOWED(), pEnableCanAllowed);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isSkipRightsOnCanAllowed() const
{
	if (!isEnableCanAllowed())
	{
		return false;
	}
	return mStoreGeneral->value(SETTINGS_NAME_SKIP_RIGHTS_ON_CAN_ALLOWED(), false).toBool();
}


void GeneralSettings::setSkipRightsOnCanAllowed(bool pSkipRightsOnCanAllowed)
{
	if (pSkipRightsOnCanAllowed != isSkipRightsOnCanAllowed())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_SKIP_RIGHTS_ON_CAN_ALLOWED(), pSkipRightsOnCanAllowed);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isShowInAppNotifications() const
{
	if (isDeveloperMode())
	{
		return true;
	}

	return mStoreGeneral->value(SETTINGS_NAME_IN_APP_NOTIFICATIONS(), isShowNotificationsOsDefault()).toBool();
}


void GeneralSettings::setShowInAppNotifications(bool pShowInAppNotifications)
{
	if (pShowInAppNotifications != isShowInAppNotifications())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_IN_APP_NOTIFICATIONS(), pShowInAppNotifications);
		Q_EMIT fireShowInAppNotificationsChanged();
	}
}


bool GeneralSettings::isCustomProxyHost() const
{
	return !getCustomProxyHost().isEmpty();
}


QString GeneralSettings::getCustomProxyHost() const
{
	return mStoreGeneral->value(SETTINGS_NAME_CUSTOM_PROXY_HOST(), QString()).toString();
}


bool GeneralSettings::isCustomProxyType() const
{
	return getCustomProxyType() != QNetworkProxy::ProxyType::NoProxy;
}


QNetworkProxy::ProxyType GeneralSettings::getCustomProxyType() const
{
	QNetworkProxy::ProxyType result;
	auto value = mStoreGeneral->value(SETTINGS_NAME_CUSTOM_PROXY_TYPE(), QString()).toString().toLower();
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
	return static_cast<quint16>(mStoreGeneral->value(SETTINGS_NAME_CUSTOM_PROXY_PORT(), 0).toInt());
}


bool GeneralSettings::customProxyAttributesPresent() const
{
	return isCustomProxyHost() && isCustomProxyType() && isCustomProxyPort();
}


bool GeneralSettings::useCustomProxy() const
{
	bool useCustomProxy = mStoreGeneral->value(SETTINGS_NAME_USE_CUSTOM_PROXY(), true).toBool();
	return useCustomProxy && customProxyAttributesPresent();
}


void GeneralSettings::setUseCustomProxy(bool pUseCustomProxy)
{
	if (useCustomProxy() != pUseCustomProxy)
	{
		mStoreGeneral->setValue(SETTINGS_NAME_USE_CUSTOM_PROXY(), pUseCustomProxy);
		Q_EMIT fireProxyChanged();
	}
}


QString GeneralSettings::getIfdServiceToken()
{
	mStoreGeneral->sync();
	if (!mStoreGeneral->contains(SETTINGS_NAME_IFD_SERVICE_TOKEN()))
	{
		auto serviceToken = Randomizer::getInstance().createUuid().toString();
		mStoreGeneral->setValue(SETTINGS_NAME_IFD_SERVICE_TOKEN(), serviceToken);
		save();
	}

	return mStoreGeneral->value(SETTINGS_NAME_IFD_SERVICE_TOKEN(), QString()).toString();
}
