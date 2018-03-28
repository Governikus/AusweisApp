/*!
 * \brief Contains the method definitions of the GeneralSettings class.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "GeneralSettings.h"

#include "AutoStart.h"

#include <QCoreApplication>
#include <QLoggingCategory>
#include <QtConcurrent/QtConcurrentRun>


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(settings)

namespace
{
SETTINGS_NAME(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION, "persistentSettingsVersion")
SETTINGS_NAME(SETTINGS_NAME_SKIP_VERSION, "skipVersion")
SETTINGS_NAME(SETTINGS_NAME_AUTO_CLOSE_WINDOW, "autoCloseWindow")
SETTINGS_NAME(SETTINGS_NAME_SHOW_SETUP_ASSISTANT, "showSetupAssistant")
SETTINGS_NAME(SETTINGS_NAME_REMIND_USER_TO_CLOSE, "remindToClose")
SETTINGS_NAME(SETTINGS_NAME_TRANSPORT_PIN_REMINDER, "transportPinReminder")
SETTINGS_NAME(SETTINGS_NAME_DEVELOPER_MODE, "developerMode")
SETTINGS_NAME(SETTINGS_NAME_USE_SELF_AUTH_TEST_URI, "selfauthTestUri")
SETTINGS_NAME(SETTINGS_NAME_LANGUAGE, "language")

SETTINGS_NAME(SETTINGS_GROUP_NAME_COMMON, "common")
SETTINGS_NAME(SETTINGS_NAME_AUTO, "autoUpdateCheck")
SETTINGS_NAME(SETTINGS_NAME_KEYLESS_PASSWORD, "keylessPassword")
SETTINGS_NAME(SETTINGS_NAME_LAST_READER_PLUGIN_TYPE, "lastTechnology")
}

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
	}

	mStoreCommon->beginGroup(SETTINGS_GROUP_NAME_COMMON());

	// QFuture.result() crashes under linux and win if uninitalized
	mAutoStart = QtConcurrent::run([] {
				return !GENERAL_SETTINGS_DEFAULT_AUTOSTART;
			});
	mAutoStart.waitForFinished();

	// Check if the key "autoCloseWindow" (introduced in changeset 199210b0b20c)
	// does not yet exist to detect a new installation. This key was the first one
	// in the settings general group.
	const bool isNewInstallation = !mStoreGeneral->allKeys().contains(SETTINGS_NAME_AUTO_CLOSE_WINDOW());
	if (isNewInstallation)
	{
		mStoreGeneral->setValue(SETTINGS_NAME_AUTO_CLOSE_WINDOW(), true);
		setAutoStart(GENERAL_SETTINGS_DEFAULT_AUTOSTART);
		setTransportPinReminder(true);
		mStoreGeneral->sync();
	}

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


void GeneralSettings::save()
{
	mStoreGeneral->setValue(SETTINGS_NAME_PERSISTENT_SETTINGS_VERSION(), QCoreApplication::applicationVersion());

	mStoreGeneral->sync();
	mStoreCommon->sync();
}


bool GeneralSettings::isAutoStart() const
{
	return mAutoStart;
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
	// The standard value should be true but we need to set it to false for backwards
	// compatibility. It is set to true in the constructor if we have a new
	// installation. The the standard value used in this function will only be
	// used if the user upgrades from AusweisApp 1.6.3 or an older version.
	return mStoreGeneral->value(SETTINGS_NAME_TRANSPORT_PIN_REMINDER(), false).toBool();
}


void GeneralSettings::setTransportPinReminder(bool pTransportPinReminder)
{
	if (pTransportPinReminder != isTransportPinReminder())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_TRANSPORT_PIN_REMINDER(), pTransportPinReminder);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isDeveloperMode() const
{
	const bool developerMode = mStoreGeneral->value(SETTINGS_NAME_DEVELOPER_MODE(), false).toBool();
	if (developerMode && appIsBackgroundService())
	{
		qCDebug(settings) << "Running as a background service. Developer mode is disallowed.";
		return false;
	}

	return developerMode;
}


void GeneralSettings::setDeveloperMode(bool pEnabled)
{
	if (pEnabled != isDeveloperMode())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_DEVELOPER_MODE(), pEnabled);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::useSelfAuthTestUri() const
{
	return mStoreGeneral->value(SETTINGS_NAME_USE_SELF_AUTH_TEST_URI(), false).toBool();
}


void GeneralSettings::setUseSelfauthenticationTestUri(bool pUse)
{
	if (pUse != useSelfAuthTestUri())
	{
		mStoreGeneral->setValue(SETTINGS_NAME_USE_SELF_AUTH_TEST_URI(), pUse);
		Q_EMIT fireSettingsChanged();
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
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isAutoUpdateCheck() const
{
	return mStoreCommon->value(SETTINGS_NAME_AUTO(), true).toBool();
}


void GeneralSettings::setAutoUpdateCheck(bool pAutoCheck)
{
	if (pAutoCheck != isAutoUpdateCheck())
	{
		mStoreCommon->setValue(SETTINGS_NAME_AUTO(), pAutoCheck);
		Q_EMIT fireSettingsChanged();
	}
}


bool GeneralSettings::isUseScreenKeyboard() const
{
	return mStoreCommon->value(SETTINGS_NAME_KEYLESS_PASSWORD(), false).toBool();
}


void GeneralSettings::setUseScreenKeyboard(bool pKeylessPassword)
{
	if (pKeylessPassword != isUseScreenKeyboard())
	{
		mStoreCommon->setValue(SETTINGS_NAME_KEYLESS_PASSWORD(), pKeylessPassword);
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
