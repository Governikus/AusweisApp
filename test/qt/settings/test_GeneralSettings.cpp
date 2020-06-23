/*!
 * \brief Unit tests for \ref GeneralSettings
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include <QCoreApplication>
#include <QFile>
#include <QSignalSpy>
#include <QtTest>

#include "GeneralSettings.h"

#include "AppSettings.h"
#include "Env.h"

using namespace governikus;

class test_GeneralSettings
	: public QObject
{
	Q_OBJECT

	private:
		bool getNotificationsOsDefault()
		{
#if defined(Q_OS_WIN)
			return QOperatingSystemVersion::current() < QOperatingSystemVersion::Windows10;

#elif defined(Q_OS_MACOS)
			return false;

#else
			return true;

#endif
		}

	private Q_SLOTS:
		void init()
		{
			AbstractSettings::mTestDir.clear();
		}


		void testAutoCloseWindowAfterAuthentication()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			bool initial = settings.isAutoCloseWindowAfterAuthentication();
			bool newValue = !initial;

			settings.setAutoCloseWindowAfterAuthentication(newValue);
			QCOMPARE(settings.isAutoCloseWindowAfterAuthentication(), newValue);
			settings.save();

			settings.setAutoCloseWindowAfterAuthentication(initial);
			QCOMPARE(settings.isAutoCloseWindowAfterAuthentication(), initial);
			settings.save();
		}


		void testAutoCheck()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			bool initial = settings.isAutoUpdateCheck();

			settings.setAutoUpdateCheck(!initial);
			QCOMPARE(settings.isAutoUpdateCheck(), !initial);
			settings.save();

			settings.setAutoUpdateCheck(initial);
			QCOMPARE(settings.isAutoUpdateCheck(), initial);
			settings.save();
		}


		void testAutoStart()
		{
			#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			bool initial = settings.isAutoStart();

			settings.setAutoStart(!initial);
			QCOMPARE(settings.isAutoStart(), !initial);
			settings.save();

			settings.setAutoStart(initial);
			QCOMPARE(settings.isAutoStart(), initial);
			settings.save();

			#else
			QSKIP("Autostart currently only on windows and mac os");
			#endif
		}


		void testUseScreenKeyboard()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			bool initial = settings.isUseScreenKeyboard();

			settings.setUseScreenKeyboard(!initial);
			QCOMPARE(settings.isUseScreenKeyboard(), !initial);
			settings.save();

			settings.setUseScreenKeyboard(initial);
			QCOMPARE(settings.isUseScreenKeyboard(), initial);
			settings.save();
		}


		void testVisualPrivacy()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			bool initial = settings.isVisualPrivacy();

			settings.setVisualPrivacy(!initial);
			QCOMPARE(settings.isVisualPrivacy(), !initial);
			settings.save();

			settings.setVisualPrivacy(initial);
			QCOMPARE(settings.isVisualPrivacy(), initial);
			settings.save();
		}


		void testShuffleScreenKeyboard()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			bool initial = settings.isShuffleScreenKeyboard();

			settings.setShuffleScreenKeyboard(!initial);
			QCOMPARE(settings.isShuffleScreenKeyboard(), !initial);
			settings.save();

			settings.setShuffleScreenKeyboard(initial);
			QCOMPARE(settings.isShuffleScreenKeyboard(), initial);
			settings.save();
		}


		void testCanAllowed()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			settings.setEnableCanAllowed(true);
			QVERIFY(settings.isEnableCanAllowed());
			QVERIFY(!settings.isSkipRightsOnCanAllowed());

			settings.setSkipRightsOnCanAllowed(true);
			QVERIFY(settings.isSkipRightsOnCanAllowed());

			settings.setEnableCanAllowed(false);
			QVERIFY(!settings.isEnableCanAllowed());
			QVERIFY(!settings.isSkipRightsOnCanAllowed());

			settings.setEnableCanAllowed(true);
			QVERIFY(settings.isEnableCanAllowed());
			QVERIFY(settings.isSkipRightsOnCanAllowed());

			settings.setEnableCanAllowed(false);
			settings.setSkipRightsOnCanAllowed(false);
			QVERIFY(!settings.isEnableCanAllowed());
			QVERIFY(!settings.isSkipRightsOnCanAllowed());
			settings.save();
		}


		void testDefaultValues()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
			QCOMPARE(settings.isAutoUpdateCheck(), true);
#else
			QCOMPARE(settings.isAutoUpdateCheck(), false);
#endif

			QCOMPARE(settings.isAutoCloseWindowAfterAuthentication(), true);
			QCOMPARE(settings.isAutoStart(), GENERAL_SETTINGS_DEFAULT_AUTOSTART);
			QCOMPARE(settings.isDeveloperOptions(), false);
			QCOMPARE(settings.isDeveloperMode(), false);
			QCOMPARE(settings.useSelfAuthTestUri(), false);
			QCOMPARE(settings.isUseScreenKeyboard(), false);
			QCOMPARE(settings.isEnableCanAllowed(), false);
			QCOMPARE(settings.isSkipRightsOnCanAllowed(), false);
			QCOMPARE(settings.isShowSetupAssistant(), true);
			QCOMPARE(settings.isShowNewUiHint(), true);
			QCOMPARE(settings.isShowInAppNotifications(), getNotificationsOsDefault());
			QCOMPARE(settings.isRemindUserToClose(), true);
			QCOMPARE(settings.isTransportPinReminder(), true);
			QCOMPARE(settings.getPersistentSettingsVersion(), QString());
			QCOMPARE(settings.getLastReaderPluginType(), QString());
		}


		void testRemindStartupWizard()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			bool initial = settings.isShowSetupAssistant();
			bool newValue = !initial;

			settings.setShowSetupAssistant(newValue);
			QCOMPARE(settings.isShowSetupAssistant(), newValue);
			settings.save();

			settings.setShowSetupAssistant(initial);
			QCOMPARE(settings.isShowSetupAssistant(), initial);
			settings.save();
		}


		void testShowNewUiHint()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			bool initial = settings.isShowNewUiHint();
			bool newValue = !initial;

			settings.setShowNewUiHint(newValue);
			QCOMPARE(settings.isShowNewUiHint(), newValue);
			settings.save();

			settings.setShowNewUiHint(initial);
			QCOMPARE(settings.isShowNewUiHint(), initial);
			settings.save();
		}


		void testDeveloperOptions()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			bool initial = settings.isDeveloperOptions();
			bool newValue = !initial;

			settings.setDeveloperOptions(newValue);
			QCOMPARE(settings.isDeveloperOptions(), newValue);
			settings.save();

			settings.setDeveloperOptions(initial);
			QCOMPARE(settings.isDeveloperOptions(), initial);
			settings.save();
		}


		void testEnableDeveloperMode()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			bool initial = settings.isDeveloperMode();
			bool newValue = !initial;

			settings.setDeveloperMode(newValue);
			QCOMPARE(settings.isDeveloperMode(), initial);
			settings.save();

			settings.setDeveloperMode(initial);
			QCOMPARE(settings.isDeveloperMode(), initial);
			settings.save();

			settings.setDeveloperOptions(true);

			settings.setDeveloperMode(newValue);
			QCOMPARE(settings.isDeveloperMode(), newValue);
			settings.save();

			settings.setDeveloperMode(initial);
			QCOMPARE(settings.isDeveloperMode(), initial);
			settings.save();

			settings.setDeveloperOptions(false);
			settings.save();
		}


		void testUseSelfAuthTestUri()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			bool initial = settings.useSelfAuthTestUri();
			bool newValue = !initial;

			settings.setUseSelfauthenticationTestUri(newValue);
			QCOMPARE(settings.useSelfAuthTestUri(), initial);
			settings.save();

			settings.setUseSelfauthenticationTestUri(initial);
			QCOMPARE(settings.useSelfAuthTestUri(), initial);
			settings.save();

			settings.setDeveloperOptions(true);

			settings.setUseSelfauthenticationTestUri(newValue);
			QCOMPARE(settings.useSelfAuthTestUri(), newValue);
			settings.save();

			settings.setUseSelfauthenticationTestUri(initial);
			QCOMPARE(settings.useSelfAuthTestUri(), initial);
			settings.save();

			settings.setDeveloperOptions(false);
			settings.save();
		}


		void testEnableNotificationsOnDeveloperMode()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			settings.setDeveloperOptions(true);
			settings.setDeveloperMode(false);

			QSignalSpy spyShowInAppNotifications(&settings, &GeneralSettings::fireShowInAppNotificationsChanged);

			const bool osDefaultNotifications = getNotificationsOsDefault();

			QCOMPARE(settings.isShowInAppNotifications(), osDefaultNotifications);

			settings.setDeveloperMode(true);
			QCOMPARE(settings.isDeveloperMode(), true);
			QCOMPARE(settings.isShowInAppNotifications(), true);
			QCOMPARE(spyShowInAppNotifications.count(), 1);
			settings.save();

			settings.setDeveloperMode(false);
			QCOMPARE(settings.isDeveloperMode(), false);
			QCOMPARE(settings.isShowInAppNotifications(), osDefaultNotifications);
			QCOMPARE(spyShowInAppNotifications.count(), 2);
			settings.save();
		}


		void testRemindUserToClose()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			bool initial = settings.isRemindUserToClose();
			bool newValue = !initial;

			settings.setRemindUserToClose(newValue);
			QCOMPARE(settings.isRemindUserToClose(), newValue);
			settings.save();

			settings.setRemindUserToClose(initial);
			QCOMPARE(settings.isRemindUserToClose(), initial);
			settings.save();
		}


		void testPersistentSettingsVersion()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			QCoreApplication::setApplicationVersion(QStringLiteral("X.Y.Z"));
			// the persistent settings version contains the application version number
			// last saving the settings --> in this test the settings have
			// never been saved, so the empty string is expected
			QCOMPARE(settings.getPersistentSettingsVersion(), QString());

			settings.save();
			QCOMPARE(settings.getPersistentSettingsVersion(), QCoreApplication::applicationVersion());
		}


		void testTransportPinReminder()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			bool initialValue = settings.isTransportPinReminder();
			bool newValue = !initialValue;

			settings.save();
			settings.setTransportPinReminder(newValue);
			QCOMPARE(settings.isTransportPinReminder(), newValue);
			settings.save();

			settings.setTransportPinReminder(initialValue);
			QCOMPARE(settings.isTransportPinReminder(), initialValue);
			settings.save();
		}


		void testLanguage()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			const QLocale::Language initialValue = settings.getLanguage();
			const QLocale::Language newValue = QLocale::English;

			settings.save();
			settings.setLanguage(newValue);
			QCOMPARE(settings.getLanguage(), newValue);
			settings.save();

			settings.setLanguage(initialValue);
			QCOMPARE(settings.getLanguage(), initialValue);
		}


		void testLastReaderPluginType()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			QString initial = settings.getLastReaderPluginType();
			QCOMPARE(initial, QString());
			QSignalSpy spy(&settings, &GeneralSettings::fireSettingsChanged);

			QString newValue;
			settings.setLastReaderPluginType(newValue);

			QCOMPARE(settings.getLastReaderPluginType(), newValue);
			settings.save();
			QCOMPARE(spy.count(), 0);
			newValue = QStringLiteral("REMOTE");
			settings.setLastReaderPluginType(newValue);
			QCOMPARE(settings.getLastReaderPluginType(), newValue);
			QCOMPARE(spy.count(), 1);
			settings.save();
		}


		void testStoreFeedbackRequested()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

#if defined(Q_OS_IOS)
			QCOMPARE(settings.isRequestStoreFeedback(), false);
#else
			QCOMPARE(settings.isRequestStoreFeedback(), false);
#endif

			settings.setRequestStoreFeedback(true);
			settings.save();
			QCOMPARE(settings.isRequestStoreFeedback(), true);
			settings.setRequestStoreFeedback(false);
			settings.save();
			QCOMPARE(settings.isRequestStoreFeedback(), false);
		}


};

QTEST_GUILESS_MAIN(test_GeneralSettings)
#include "test_GeneralSettings.moc"
