/*!
 * \brief Contains the method definitions of the GeneralSettings class.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "GeneralSettings.h"

#include "AppSettings.h"
#include "AutoStart.h"
#include "Env.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QOperatingSystemVersion>
#include <QtConcurrent/QtConcurrentRun>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(settings)

namespace
{
SETTINGS_NAME(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION, "persistentSettingsVersion")
SETTINGS_NAME(SETTINGS_NAME_SKIP_VERSION, "skipVersion")
SETTINGS_NAME(SETTINGS_NAME_AUTO_CLOSE_WINDOW, "autoCloseWindow")
SETTINGS_NAME(SETTINGS_NAME_SHOW_SETUP_ASSISTANT, "showSetupAssistant")
SETTINGS_NAME(SETTINGS_NAME_SHOW_NEW_UI_HINT, "showNewUiHint")
SETTINGS_NAME(SETTINGS_NAME_REMIND_USER_TO_CLOSE, "remindToClose")
SETTINGS_NAME(SETTINGS_NAME_TRANSPORT_PIN_REMINDER, "transportPinReminder")
SETTINGS_NAME(SETTINGS_NAME_DEVELOPER_OPTIONS, "developerOptions")
SETTINGS_NAME(SETTINGS_NAME_DEVELOPER_MODE, "developerMode")
SETTINGS_NAME(SETTINGS_NAME_USE_SELF_AUTH_TEST_URI, "selfauthTestUri")
SETTINGS_NAME(SETTINGS_NAME_LANGUAGE, "language")
SETTINGS_NAME(SETTINGS_NAME_SELECTED_UI, "selectedUi")
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
} // namespace

GeneralSettings::GeneralSettings()
	: AbstractSettings()
	, mStoreGeneral(getStore())
	, mStoreCommon(getStore())
{
	{
		// With 1.14.0 the reader and provider are no longer stored in the settings

		mStoreCommon->beginGroup(QStringLiteral("readers"));
		mStoreCommon->remove(QString()); // remove the whole group
		mStoreCommon->endGroup();

		mStoreCommon->beginGroup(QStringLiteral("providers"));
		mStoreCommon->remove(QString()); // remove the whole group
		mStoreCommon->endGroup();

		// With 1.20.1 the common values are moved to the general values

		mStoreCommon->beginGroup(QStringLiteral("common"));
		if (!autoUpdateCheckIsSetByAdmin() && mStoreCommon->contains(SETTINGS_NAME_AUTO()))
		{
			mStoreGeneral->setValue(SETTINGS_NAME_AUTO(), mStoreCommon->value(SETTINGS_NAME_AUTO()).toBool());
		}
		if (mStoreCommon->contains(SETTINGS_NAME_KEYLESS_PASSWORD()))
		{
			mStoreGeneral->setValue(SETTINGS_NAME_KEYLESS_PASSWORD(), mStoreCommon->value(SETTINGS_NAME_KEYLESS_PASSWORD()).toBool());
		}
		if (mStoreCommon->contains(SETTINGS_NAME_SHUFFLE_SCREEN_KEYBOARD()))
		{
			mStoreGeneral->setValue(SETTINGS_NAME_SHUFFLE_SCREEN_KEYBOARD(), mStoreCommon->value(SETTINGS_NAME_SHUFFLE_SCREEN_KEYBOARD()).toBool());
		}
		mStoreCommon->remove(QString()); // remove the whole group
		mStoreCommon->endGroup();
	}

	// QFuture.result() crashes under linux and win if uninitalized
	mAutoStart = QtConcurrent::run([] {
				return !GENERAL_SETTINGS_DEFAULT_AUTOSTART;
			});
	mAutoStart.waitForFinished();

	// Check if the key "autoCloseWindow" (introduced in changeset 199210b0b20c)
	// does not yet exist to detect a new installation. This key was the first one
	// in the settings general group.
	const bool isNewInstallation = getPersistentSettingsVersion().isEmpty();
	if (isNewInstallation)
	{
		mStoreGeneral->setValue(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION(), QCoreApplication::applicationVersion());
		setAutoStart(GENERAL_SETTINGS_DEFAULT_AUTOSTART);
		mStoreGeneral->sync();
		setShowNewUiHint(false);
	}

#if defined(QT_NO_DEBUG) && (defined(Q_OS_ANDROID) || defined(Q_OS_IOS))
	setUseSelfauthenticationTestUri(false);
#endif

#ifdef QT_NO_DEBUG
	// Iterate autostart entries in order to remove broken login items on macos.
	// This process might take up to 15s per entry.
	mAutoStart = QtConcurrent::run(AutoStart::enabled);
#endif
}


GeneralSettings::~GeneralSettings()
{
	mAutoStart.waitForFinished();
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
	mStoreCommon->sync();
}


bool GeneralSettings::isAutoStartAvailable() const
{
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
	return true;

#else
	// Linux: maybe XDG_CONFIG_HOME/autostart or XDG_CONFIG_DIRS/autostart can be added
	return false;

#endif
}


bool GeneralSettings::isAutoStart() const
{
	return mAutoStart;
}


bool GeneralSettings::autoStartIsSetByAdmin() const
{
	return AutoStart::isSetByAdmin();
}


void GeneralSettings::setAutoStart(bool pAutoStart)
{
	if (pAutoStart != mAutoStart)
	{
#ifdef QT_NO_DEBUG
		AutoStart::set(pAutoStart);
#endif
		mAutoStart.waitForFinished();
		mAutoStart = QtConcurrent::run([pAutoStart] {
					return pAutoStart;
				});
		Q_EMIT fireSettingsChanged();
	}
}


const QString GeneralSettings::getPersistentSettingsVersion() const
{
	return mStoreGeneral->value(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION(), QString()).toString();
}


QString GeneralSettings::getSkipVersion()
{
	return mStoreGeneral->value(SETTINGS_NAME_SKIP_VERSION(), QString()).toString();
}


void GeneralSettings::skipVersion(const QString& pVersion)
{
	mStoreGeneral->setValue(SETTINGS_NAME_SKIP_VERSION(), pVersion);
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


bool GeneralSettings::isShowSetupAssistant() const
{
	return mStoreGeneral->value(SETTINGS_NAME_SHOW_SETUP_ASSISTANT(), true).toBool();
}


void GeneralSettings::setShowSetupAssistant(bool pShowSetupAssistant)
{
	if (pShowSetupAssistant != isShowSetupAssistant())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_SHOW_SETUP_ASSISTANT(), pShowSetupAssistant);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isShowNewUiHint() const
{
	return mStoreGeneral->value(SETTINGS_NAME_SHOW_NEW_UI_HINT(), true).toBool();
}


void GeneralSettings::setShowNewUiHint(bool pShowNewUiHint)
{
	if (pShowNewUiHint != isShowNewUiHint())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_SHOW_NEW_UI_HINT(), pShowNewUiHint);
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
	const bool developerMode = mStoreGeneral->value(SETTINGS_NAME_DEVELOPER_MODE(), false).toBool();
	if (developerMode && Env::getSingleton<AppSettings>()->isUsedAsSDK())
	{
		qCDebug(settings) << "Running as SDK. Developer mode is disallowed.";
		return false;
	}

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (!isDeveloperOptions())
	{
		return false;
	}
#endif

	return developerMode;
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
		mStoreGeneral->setValue(SETTINGS_NAME_LANGUAGE(), pLanguage == QLocale::C ? QString() : QLocale(pLanguage).bcp47Name());
		Q_EMIT fireLanguageChanged();
		Q_EMIT fireSettingsChanged();
	}
}


QString GeneralSettings::getSelectedUi() const
{
	return mStoreGeneral->value(SETTINGS_NAME_SELECTED_UI(), QStringLiteral("qml")).toString();
}


void GeneralSettings::setSelectedUi(const QString& pSelectedUi)
{
	if (pSelectedUi != getSelectedUi())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_SELECTED_UI(), pSelectedUi);
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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
	QSettings settings(QSettings::Scope::SystemScope);
#else
#ifdef Q_OS_MACOS
	QSettings settings(QSettings::Scope::SystemScope, QCoreApplication::organizationDomain(), QCoreApplication::applicationName());
#else
	QSettings settings(QSettings::Scope::SystemScope, QCoreApplication::organizationName(), QCoreApplication::applicationName());
#endif
#endif

	return settings.contains(SETTINGS_NAME_AUTO());
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
