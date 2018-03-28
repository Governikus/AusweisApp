/*!
 * \brief Unit tests for \ref GeneralSettings
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include <QCoreApplication>
#include <QFile>
#include <QSignalSpy>
#include <QtTest>

#include "GeneralSettings.h"


using namespace governikus;

class test_GeneralSettings
	: public QObject
{
	Q_OBJECT
	QScopedPointer<GeneralSettings> mSettings;

	private Q_SLOTS:
		void init()
		{
			AbstractSettings::mTestDir.clear();
			mSettings.reset(new GeneralSettings());
		}


		void testAutoCloseWindowAfterAuthentication()
		{
			bool initial = mSettings->isAutoCloseWindowAfterAuthentication();
			bool newValue = !initial;

			mSettings->setAutoCloseWindowAfterAuthentication(newValue);
			QCOMPARE(mSettings->isAutoCloseWindowAfterAuthentication(), newValue);
			mSettings->save();

			mSettings->setAutoCloseWindowAfterAuthentication(initial);
			QCOMPARE(mSettings->isAutoCloseWindowAfterAuthentication(), initial);
			mSettings->save();
		}


		void testAutoCheck()
		{
			bool initial = mSettings->isAutoUpdateCheck();

			mSettings->setAutoUpdateCheck(!initial);
			QCOMPARE(mSettings->isAutoUpdateCheck(), !initial);
			mSettings->save();

			mSettings->setAutoUpdateCheck(initial);
			QCOMPARE(mSettings->isAutoUpdateCheck(), initial);
			mSettings->save();

		}


		void testAutoStart()
		{
			#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
			bool initial = mSettings->isAutoStart();

			mSettings->setAutoStart(!initial);
			QCOMPARE(mSettings->isAutoStart(), !initial);
			mSettings->save();


			mSettings->setAutoStart(initial);
			QCOMPARE(mSettings->isAutoStart(), initial);
			mSettings->save();

			#else
			QSKIP("Autostart currently only on windows and mac os");
			#endif
		}


		void testUseScreenKeyboard()
		{
			bool initial = mSettings->isUseScreenKeyboard();

			mSettings->setUseScreenKeyboard(!initial);
			QCOMPARE(mSettings->isUseScreenKeyboard(), !initial);
			mSettings->save();

			mSettings->setUseScreenKeyboard(initial);
			QCOMPARE(mSettings->isUseScreenKeyboard(), initial);
			mSettings->save();

		}


		void testDefaultValues()
		{
			QCOMPARE(mSettings->isAutoCloseWindowAfterAuthentication(), true);
			QCOMPARE(mSettings->isAutoStart(), GENERAL_SETTINGS_DEFAULT_AUTOSTART);
			QCOMPARE(mSettings->isAutoUpdateCheck(), true);
			QCOMPARE(mSettings->isUseScreenKeyboard(), false);
			QCOMPARE(mSettings->isShowSetupAssistant(), true);
			QCOMPARE(mSettings->isRemindUserToClose(), true);
			QCOMPARE(mSettings->isTransportPinReminder(), true);
			QCOMPARE(mSettings->getPersistentSettingsVersion(), QString());
			QCOMPARE(mSettings->isDeveloperMode(), false);
			QCOMPARE(mSettings->useSelfAuthTestUri(), false);
			QCOMPARE(mSettings->getLastReaderPluginType(), QString());
		}


		void testRemindStartupWizard()
		{
			bool initial = mSettings->isShowSetupAssistant();
			bool newValue = !initial;

			mSettings->setShowSetupAssistant(newValue);
			QCOMPARE(mSettings->isShowSetupAssistant(), newValue);
			mSettings->save();

			mSettings->setShowSetupAssistant(initial);
			QCOMPARE(mSettings->isShowSetupAssistant(), initial);
			mSettings->save();
		}


		void testRemindUserToClose()
		{
			bool initial = mSettings->isRemindUserToClose();
			bool newValue = !initial;

			mSettings->setRemindUserToClose(newValue);
			QCOMPARE(mSettings->isRemindUserToClose(), newValue);
			mSettings->save();

			mSettings->setRemindUserToClose(initial);
			QCOMPARE(mSettings->isRemindUserToClose(), initial);
			mSettings->save();
		}


		void testPersistentSettingsVersion()
		{
			QCoreApplication::setApplicationVersion(QStringLiteral("X.Y.Z"));
			// the persistent settings version contains the application version number
			// last saving the settings --> in this test the settings have
			// never been saved, so the empty string is expected
			QCOMPARE(mSettings->getPersistentSettingsVersion(), QString());

			mSettings->save();
			QCOMPARE(mSettings->getPersistentSettingsVersion(), QCoreApplication::applicationVersion());
		}


		void testTransportPinReminder()
		{
			bool initialValue = mSettings->isTransportPinReminder();
			bool newValue = !initialValue;

			mSettings->save();
			mSettings->setTransportPinReminder(newValue);
			QCOMPARE(mSettings->isTransportPinReminder(), newValue);
			mSettings->save();

			mSettings->setTransportPinReminder(initialValue);
			QCOMPARE(mSettings->isTransportPinReminder(), initialValue);
			mSettings->save();
		}


		void testLanguage()
		{
			const QLocale::Language initialValue = mSettings->getLanguage();
			const QLocale::Language newValue = QLocale::English;

			mSettings->save();
			mSettings->setLanguage(newValue);
			QCOMPARE(mSettings->getLanguage(), newValue);
			mSettings->save();

			mSettings->setLanguage(initialValue);
			QCOMPARE(mSettings->getLanguage(), initialValue);
		}


		void testLastReaderPluginType()
		{
			QString initial = mSettings->getLastReaderPluginType();
			QCOMPARE(initial, QString());
			QSignalSpy spy(mSettings.data(), &GeneralSettings::fireSettingsChanged);

			QString newValue;
			mSettings->setLastReaderPluginType(newValue);

			QCOMPARE(mSettings->getLastReaderPluginType(), newValue);
			mSettings->save();
			QCOMPARE(spy.count(), 0);
			newValue = QStringLiteral("REMOTE");
			mSettings->setLastReaderPluginType(newValue);
			QCOMPARE(mSettings->getLastReaderPluginType(), newValue);
			QCOMPARE(spy.count(), 1);
			mSettings->save();
		}


};

QTEST_GUILESS_MAIN(test_GeneralSettings)
#include "test_GeneralSettings.moc"
