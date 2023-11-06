/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref GeneralSettings
 */

#include "GeneralSettings.h"

#include "AppSettings.h"
#include "Env.h"
#include "VolatileSettings.h"
#ifdef Q_OS_WIN
	#include "AutoStart.h"
#endif

#include <QCoreApplication>
#include <QFile>
#include <QSignalSpy>
#include <QtTest>

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
		void initTestCase()
		{
			// init ctor of Singleton, otherwise the testcases are not
			// stable. There could be differences between a run of a
			// single testcase and the whole testsuite.
			Env::getSingleton<AppSettings>()->getGeneralSettings();
		}


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

			settings.setAutoCloseWindowAfterAuthentication(initial);
			QCOMPARE(settings.isAutoCloseWindowAfterAuthentication(), initial);
		}


		void testAutoCheck()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			bool initial = settings.isAutoUpdateCheck();

			settings.setAutoUpdateCheck(!initial);
			QCOMPARE(settings.isAutoUpdateCheck(), !initial);

			settings.setAutoUpdateCheck(initial);
			QCOMPARE(settings.isAutoUpdateCheck(), initial);
		}


		void testAutoStart_data()
		{
			QTest::addColumn<bool>("useSdkMode");
			QTest::newRow("SdkModeEnabled") << true;
			QTest::newRow("SdkModeDisabled") << false;
		}


		void testAutoStart()
		{
			QFETCH(bool, useSdkMode);

			SDK_MODE(useSdkMode);
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
			QCOMPARE(settings.isAutoStartAvailable(), !useSdkMode);

			bool initial = settings.isAutoStart();
			if (useSdkMode)
			{
				QTest::ignoreMessage(QtDebugMsg, "AutoStart is not available");
			}
			settings.setAutoStart(!initial);

			if (useSdkMode)
			{
				QCOMPARE(settings.isAutoStart(), initial);
			}
			else
			{
				QCOMPARE(settings.isAutoStart(), !initial);
			}

			if (useSdkMode)
			{
				QTest::ignoreMessage(QtDebugMsg, "AutoStart is not available");
			}
			settings.setAutoStart(initial);
			QCOMPARE(settings.isAutoStart(), initial);
#else
			Q_UNUSED(useSdkMode)
			QCOMPARE(settings.isAutoStartAvailable(), false);
#endif
		}


#ifdef Q_OS_WIN
		void testInstallationPathOfAutoStart_data()
		{
			QTest::addColumn<bool>("useSdkMode");
			QTest::newRow("SdkModeEnabled") << true;
			QTest::newRow("SdkModeDisabled") << false;
		}


		void testInstallationPathOfAutoStart()
		{

			QFETCH(bool, useSdkMode);

			SDK_MODE(useSdkMode);

			// change path to trigger rewrite of AutoStart entry
			const auto& windowsBootUpSettings = AutoStart::getRegistryStore();
			windowsBootUpSettings->setValue(QCoreApplication::applicationName(), QStringLiteral("dummy"));

			QCOMPARE(windowsBootUpSettings->value(QCoreApplication::applicationName(), QString()).toString(), QString("dummy"));

			QVERIFY(!AutoStart::enabled());
			QCOMPARE(windowsBootUpSettings->value(QCoreApplication::applicationName(), QString()).toString(), QString("dummy"));

			QCOMPARE(AutoStart::set(true), !useSdkMode);
			QCOMPARE(AutoStart::enabled(), !useSdkMode);
			const auto& expectedPath = useSdkMode ? QString("dummy") : AutoStart::appPath();
			QCOMPARE(windowsBootUpSettings->value(QCoreApplication::applicationName(), QString()).toString(), expectedPath);
		}


#endif

		void testUseScreenKeyboard()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			bool initial = settings.isUseScreenKeyboard();

			settings.setUseScreenKeyboard(!initial);
			QCOMPARE(settings.isUseScreenKeyboard(), !initial);

			settings.setUseScreenKeyboard(initial);
			QCOMPARE(settings.isUseScreenKeyboard(), initial);
		}


		void testVisualPrivacy()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			bool initial = settings.isVisualPrivacy();

			settings.setVisualPrivacy(!initial);
			QCOMPARE(settings.isVisualPrivacy(), !initial);

			settings.setVisualPrivacy(initial);
			QCOMPARE(settings.isVisualPrivacy(), initial);
		}


		void testShuffleScreenKeyboard()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			bool initial = settings.isShuffleScreenKeyboard();

			settings.setShuffleScreenKeyboard(!initial);
			QCOMPARE(settings.isShuffleScreenKeyboard(), !initial);

			settings.setShuffleScreenKeyboard(initial);
			QCOMPARE(settings.isShuffleScreenKeyboard(), initial);
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
		}


		void testDefaultValues()
		{
			SDK_MODE(false);
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
			QCOMPARE(settings.isAutoUpdateCheck(), true);
#else
			QCOMPARE(settings.isAutoUpdateCheck(), false);
#endif

			if (settings.isAutoStartAvailable())
			{
				QCOMPARE(settings.isAutoStart(), GENERAL_SETTINGS_DEFAULT_AUTOSTART);
			}
			else
			{
				QCOMPARE(settings.isAutoStart(), false);
			}

			QCOMPARE(settings.isAutoCloseWindowAfterAuthentication(), true);
			QCOMPARE(settings.isDeveloperOptions(), false);
			QCOMPARE(settings.isDeveloperMode(), false);
			QCOMPARE(settings.useSelfAuthTestUri(), false);
			QCOMPARE(settings.isUseScreenKeyboard(), false);
			QCOMPARE(settings.isEnableCanAllowed(), false);
			QCOMPARE(settings.isSkipRightsOnCanAllowed(), false);
			QCOMPARE(settings.getStartupModule(), "");
			QCOMPARE(settings.isShowInAppNotifications(), getNotificationsOsDefault());
			QCOMPARE(settings.isRemindUserToClose(), true);
			QCOMPARE(settings.isTransportPinReminder(), true);
			QCOMPARE(settings.getPersistentSettingsVersion(), QString());
			QCOMPARE(settings.isNewAppVersion(), false);
			QCOMPARE(settings.getLastReaderPluginType(), QString());
			QCOMPARE(settings.isUseSystemFont(), false);
			QCOMPARE(settings.getDarkMode(), QString());
		}


		void testStartupModule()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			QString initial = settings.getStartupModule();
			QString newValue = QStringLiteral("TUTORIAL");

			settings.setStartupModule(newValue);
			QCOMPARE(settings.getStartupModule(), newValue);

			settings.setStartupModule(initial);
			QCOMPARE(settings.getStartupModule(), initial);
		}


		void testDeveloperOptions()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			bool initial = settings.isDeveloperOptions();
			bool newValue = !initial;

			settings.setDeveloperOptions(newValue);
			QCOMPARE(settings.isDeveloperOptions(), newValue);

			settings.setDeveloperOptions(initial);
			QCOMPARE(settings.isDeveloperOptions(), initial);
		}


		void testEnableDeveloperMode()
		{
			SDK_MODE(false);
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			bool initial = settings.isDeveloperMode();
			bool newValue = !initial;

			settings.setDeveloperMode(newValue);
			QCOMPARE(settings.isDeveloperMode(), initial);

			settings.setDeveloperMode(initial);
			QCOMPARE(settings.isDeveloperMode(), initial);

			settings.setDeveloperOptions(true);

			settings.setDeveloperMode(newValue);
			QCOMPARE(settings.isDeveloperMode(), newValue);

			settings.setDeveloperMode(initial);
			QCOMPARE(settings.isDeveloperMode(), initial);

			settings.setDeveloperOptions(false);
		}


		void testUseSelfAuthTestUri()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			bool initial = settings.useSelfAuthTestUri();
			bool newValue = !initial;

			settings.setUseSelfauthenticationTestUri(newValue);
			QCOMPARE(settings.useSelfAuthTestUri(), initial);

			settings.setUseSelfauthenticationTestUri(initial);
			QCOMPARE(settings.useSelfAuthTestUri(), initial);

			settings.setDeveloperOptions(true);

			settings.setUseSelfauthenticationTestUri(newValue);
			QCOMPARE(settings.useSelfAuthTestUri(), newValue);

			settings.setUseSelfauthenticationTestUri(initial);
			QCOMPARE(settings.useSelfAuthTestUri(), initial);

			settings.setDeveloperOptions(false);
		}


		void testEnableNotificationsOnDeveloperMode()
		{
			SDK_MODE(false);
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

			settings.setDeveloperMode(false);
			QCOMPARE(settings.isDeveloperMode(), false);
			QCOMPARE(settings.isShowInAppNotifications(), osDefaultNotifications);
			QCOMPARE(spyShowInAppNotifications.count(), 2);
		}


		void testRemindUserToClose()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			bool initial = settings.isRemindUserToClose();
			bool newValue = !initial;

			settings.setRemindUserToClose(newValue);
			QCOMPARE(settings.isRemindUserToClose(), newValue);

			settings.setRemindUserToClose(initial);
			QCOMPARE(settings.isRemindUserToClose(), initial);
		}


		void testPersistentSettingsVersion()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			QCOMPARE(settings.getPersistentSettingsVersion(), QString());

			QCoreApplication::setApplicationVersion(QStringLiteral("X.Y.Z"));
			GeneralSettings generalSettings(settings.mStore);
			QCOMPARE(generalSettings.getPersistentSettingsVersion(), QCoreApplication::applicationVersion());
		}


		void testIsNewAppVersion()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			QCoreApplication::setApplicationVersion(QStringLiteral("X.Y.Z"));
			GeneralSettings generalSettings1(settings.mStore);

			GeneralSettings generalSettings2(settings.mStore);
			QCOMPARE(generalSettings2.isNewAppVersion(), false);

			QCoreApplication::setApplicationVersion(QStringLiteral("Z.Y.X"));
			GeneralSettings generalSettings3(settings.mStore);
			QCOMPARE(generalSettings3.isNewAppVersion(), true);
		}


		void testTransportPinReminder()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			bool initialValue = settings.isTransportPinReminder();
			bool newValue = !initialValue;

			settings.setTransportPinReminder(newValue);
			QCOMPARE(settings.isTransportPinReminder(), newValue);

			settings.setTransportPinReminder(initialValue);
			QCOMPARE(settings.isTransportPinReminder(), initialValue);
		}


		void testLanguage()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

			const QLocale::Language initialValue = settings.getLanguage();
			const QLocale::Language newValue = QLocale::English;

			settings.setLanguage(newValue);
			QCOMPARE(settings.getLanguage(), newValue);

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
			QCOMPARE(spy.count(), 0);
			newValue = QStringLiteral("REMOTE");
			settings.setLastReaderPluginType(newValue);
			QCOMPARE(settings.getLastReaderPluginType(), newValue);
			QCOMPARE(spy.count(), 1);
		}


		void testStoreFeedbackRequested()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			QCOMPARE(settings.isRequestStoreFeedback(), false);
			settings.setRequestStoreFeedback(true);
			QCOMPARE(settings.isRequestStoreFeedback(), true);
			settings.setRequestStoreFeedback(false);
			QCOMPARE(settings.isRequestStoreFeedback(), false);
		}


		void testSmartUpdate()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			QSignalSpy smartSpy(&settings, &GeneralSettings::fireSmartAvailableChanged);

			QCOMPARE(smartSpy.count(), 0);
			QCOMPARE(settings.doSmartUpdate(), true);
			QCOMPARE(settings.isSmartAvailable(), false);

			settings.setSmartAvailable(false);
			QCOMPARE(smartSpy.count(), 0);
			QCOMPARE(settings.doSmartUpdate(), false);
			QCOMPARE(settings.isSmartAvailable(), false);

			settings.setSmartAvailable(true);
			QCOMPARE(smartSpy.count(), 1);
			QCOMPARE(smartSpy.at(0).at(0), true);
			QCOMPARE(settings.doSmartUpdate(), false);
			QCOMPARE(settings.isSmartAvailable(), true);

			settings.setSmartAvailable(true);
			QCOMPARE(smartSpy.count(), 1);
			QCOMPARE(settings.doSmartUpdate(), false);
			QCOMPARE(settings.isSmartAvailable(), true);

			settings.setSmartAvailable(false);
			QCOMPARE(smartSpy.count(), 2);
			QCOMPARE(smartSpy.at(1).at(0), false);
			QCOMPARE(settings.doSmartUpdate(), false);
			QCOMPARE(settings.isSmartAvailable(), false);
		}


		void testUseSystemFont()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			QSignalSpy fontSpy(&settings, &GeneralSettings::fireUseSystemFontChanged);

			QCOMPARE(fontSpy.count(), 0);
			QCOMPARE(settings.isUseSystemFont(), false);

			settings.setUseSystemFont(true);
			QCOMPARE(fontSpy.count(), 1);
			QCOMPARE(settings.isUseSystemFont(), true);

			settings.setUseSystemFont(false);
			QCOMPARE(fontSpy.count(), 2);
			QCOMPARE(settings.isUseSystemFont(), false);
		}


		void testDarkMode()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			QSignalSpy darkSpy(&settings, &GeneralSettings::fireDarkModeChanged);
			QCOMPARE(darkSpy.count(), 0);

			const auto initialMode = settings.getDarkMode();

			const auto systemMode = QStringLiteral("AUTO");
			settings.setDarkMode(systemMode);
			QCOMPARE(darkSpy.count(), 1);
			QCOMPARE(settings.getDarkMode(), systemMode);

			settings.setDarkMode(initialMode);
			QCOMPARE(darkSpy.count(), 2);
			QCOMPARE(settings.getDarkMode(), initialMode);
		}


		void testShowTrayIcon()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
			QCOMPARE(settings.showTrayIcon(), settings.isAutoStart());
			settings.setAutoStart(!settings.isAutoStart());
			QCOMPARE(settings.showTrayIcon(), settings.isAutoStart());
#else
			QCOMPARE(settings.showTrayIcon(), true);
#endif
		}


};

QTEST_GUILESS_MAIN(test_GeneralSettings)
#include "test_GeneralSettings.moc"
