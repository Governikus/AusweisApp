/*!
 * \brief Unit tests for \ref GeneralSettings
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include <QCoreApplication>
#include <QFile>
#include <QtTest>

#include "GeneralSettings.h"


using namespace governikus;

class test_GeneralSettings
	: public QObject
{
	Q_OBJECT
	QScopedPointer<GeneralSettings> settings;

	private Q_SLOTS:
		void init()
		{
			AbstractSettings::mTestDir.clear();
			settings.reset(new GeneralSettings());
		}


		void testEquals()
		{
			GeneralSettings otherSettings;
			otherSettings.load();

			QVERIFY(*settings == otherSettings);

			settings->setAutoCloseWindowAfterAuthentication(!settings->isAutoCloseWindowAfterAuthentication());
			QVERIFY(*settings != otherSettings);
			otherSettings.setAutoCloseWindowAfterAuthentication(settings->isAutoCloseWindowAfterAuthentication());
			QVERIFY(*settings == otherSettings);

			settings->setAutoUpdateCheck(false);
			QVERIFY(*settings != otherSettings);
			otherSettings.setAutoUpdateCheck(false);

			settings->setAutoStart(!GENERAL_SETTINGS_DEFAULT_AUTOSTART);
			QVERIFY(*settings != otherSettings);
			otherSettings.setAutoStart(!GENERAL_SETTINGS_DEFAULT_AUTOSTART);

			settings->setUseScreenKeyboard(true);
			QVERIFY(*settings != otherSettings);
			otherSettings.setUseScreenKeyboard(true);

			settings->setShowSetupAssistant(!settings->isShowSetupAssistant());
			QVERIFY(*settings != otherSettings);
			otherSettings.setShowSetupAssistant(settings->isShowSetupAssistant());
			QVERIFY(*settings == otherSettings);

			settings->setRemindUserToClose(!settings->isRemindUserToClose());
			QVERIFY(*settings != otherSettings);
			otherSettings.setRemindUserToClose(settings->isRemindUserToClose());
			QVERIFY(*settings == otherSettings);
		}


		void testAutoCloseWindowAfterAuthentication()
		{
			bool initial = settings->isAutoCloseWindowAfterAuthentication();
			bool newValue = !initial;

			settings->setAutoCloseWindowAfterAuthentication(newValue);
			QCOMPARE(settings->isAutoCloseWindowAfterAuthentication(), newValue);
			QVERIFY(settings->isUnsaved());
			settings->save();
			QVERIFY(!settings->isUnsaved());

			settings->setAutoCloseWindowAfterAuthentication(initial);
			QCOMPARE(settings->isAutoCloseWindowAfterAuthentication(), initial);
			settings->save();
		}


		void testAutoCheck()
		{
			bool initial = settings->isAutoUpdateCheck();

			settings->setAutoUpdateCheck(!initial);
			QCOMPARE(settings->isAutoUpdateCheck(), !initial);
			QVERIFY(settings->isUnsaved());
			settings->save();
			QVERIFY(!settings->isUnsaved());


			settings->setAutoUpdateCheck(initial);
			QCOMPARE(settings->isAutoUpdateCheck(), initial);
			settings->save();

		}


		void testAutoStart()
		{
			#if defined(Q_OS_WIN32) || defined(Q_OS_OSX)
			bool initial = settings->isAutoStart();

			settings->setAutoStart(!initial);
			QCOMPARE(settings->isAutoStart(), !initial);
			QVERIFY(settings->isUnsaved());
			settings->save();
			QVERIFY(!settings->isUnsaved());


			settings->setAutoStart(initial);
			QCOMPARE(settings->isAutoStart(), initial);
			settings->save();

			#else
			QSKIP("Autostart currently only on windows and mac os");
			#endif
		}


		void testUseScreenKeyboard()
		{
			bool initial = settings->isUseScreenKeyboard();

			settings->setUseScreenKeyboard(!initial);
			QCOMPARE(settings->isUseScreenKeyboard(), !initial);
			QVERIFY(settings->isUnsaved());
			settings->save();
			QVERIFY(!settings->isUnsaved());

			settings->setUseScreenKeyboard(initial);
			QCOMPARE(settings->isUseScreenKeyboard(), initial);
			settings->save();

		}


		void testDefaultValues()
		{
			GeneralSettings defaultSettings;
			QCOMPARE(defaultSettings.isAutoCloseWindowAfterAuthentication(), true);
			QCOMPARE(defaultSettings.isAutoStart(), GENERAL_SETTINGS_DEFAULT_AUTOSTART);
			QCOMPARE(defaultSettings.isAutoUpdateCheck(), true);
			QCOMPARE(defaultSettings.isUseScreenKeyboard(), false);
			QCOMPARE(defaultSettings.isShowSetupAssistant(), true);
			QCOMPARE(defaultSettings.isRemindUserToClose(), true);
			QCOMPARE(defaultSettings.isTransportPinReminder(), true);
			QCOMPARE(defaultSettings.getPersistentSettingsVersion(), QString());
			QCOMPARE(defaultSettings.isDeveloperMode(), false);
			QCOMPARE(defaultSettings.useSelfauthenticationTestUri(), false);
		}


		void testRemindStartupWizard()
		{
			bool initial = settings->isShowSetupAssistant();
			bool newValue = !initial;

			settings->setShowSetupAssistant(newValue);
			QCOMPARE(settings->isShowSetupAssistant(), newValue);
			QVERIFY(settings->isUnsaved());
			settings->save();
			QVERIFY(!settings->isUnsaved());

			settings->setShowSetupAssistant(initial);
			QCOMPARE(settings->isShowSetupAssistant(), initial);
			QVERIFY(settings->isUnsaved());
			settings->save();
		}


		void testRemindUserToClose()
		{
			bool initial = settings->isRemindUserToClose();
			bool newValue = !initial;

			settings->setRemindUserToClose(newValue);
			QCOMPARE(settings->isRemindUserToClose(), newValue);
			QVERIFY(settings->isUnsaved());
			settings->save();
			QVERIFY(!settings->isUnsaved());

			settings->setRemindUserToClose(initial);
			QCOMPARE(settings->isRemindUserToClose(), initial);
			QVERIFY(settings->isUnsaved());
			settings->save();
		}


		void testPersistentSettingsVersion()
		{
			QCoreApplication::setApplicationVersion(QStringLiteral("X.Y.Z"));
			// the persistent settings version contains the application version number
			// last saving the settings --> in this test the settings have
			// never been saved, so the empty string is expected
			QCOMPARE(settings->getPersistentSettingsVersion(), QString());

			settings->save();
			QCOMPARE(settings->getPersistentSettingsVersion(), QCoreApplication::applicationVersion());
		}


		void testTransportPinReminder()
		{
			bool initialValue = settings->isTransportPinReminder();
			bool newValue = !initialValue;

			QVERIFY(!settings->isUnsaved());
			settings->setTransportPinReminder(newValue);
			QCOMPARE(settings->isTransportPinReminder(), newValue);
			QVERIFY(settings->isUnsaved());
			settings->save();

			QVERIFY(!settings->isUnsaved());
			settings->setTransportPinReminder(initialValue);
			QCOMPARE(settings->isTransportPinReminder(), initialValue);
			QVERIFY(settings->isUnsaved());
			settings->save();
		}


};

QTEST_GUILESS_MAIN(test_GeneralSettings)
#include "test_GeneralSettings.moc"
