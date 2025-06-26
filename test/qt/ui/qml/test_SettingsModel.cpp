/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */


#include "SettingsModel.h"

#include "AppSettings.h"

#include <QSignalSpy>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_SettingsModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void init()
		{
			Env::getSingleton<SettingsModel>()->setPreferredTechnology(ReaderManagerPluginType::UNKNOWN);
		}


		void testAutoRedirect()
		{
			auto* model = Env::getSingleton<SettingsModel>();
			QSignalSpy spy(model, &SettingsModel::fireAutoRedirectAfterAuthenticationChanged);

			QCOMPARE(spy.count(), 0);
			QCOMPARE(model->isAutoRedirectAfterAuthentication(), true);

			model->setAutoRedirectAfterAuthentication(false);
			QCOMPARE(spy.count(), 1);
			QCOMPARE(model->isAutoRedirectAfterAuthentication(), false);

			model->setAutoRedirectAfterAuthentication(false);
			QCOMPARE(spy.count(), 1);
			QCOMPARE(model->isAutoRedirectAfterAuthentication(), false);

			model->setAutoRedirectAfterAuthentication(true);
			QCOMPARE(spy.count(), 2);
			QCOMPARE(model->isAutoRedirectAfterAuthentication(), true);

			model->setAutoRedirectAfterAuthentication(true);
			QCOMPARE(spy.count(), 2);
			QCOMPARE(model->isAutoRedirectAfterAuthentication(), true);
		}


		void testAnimations()
		{
			const auto* model = Env::getSingleton<SettingsModel>();
			QSignalSpy animationsSpy(model, &SettingsModel::fireUseAnimationsChanged);

			QCOMPARE(animationsSpy.count(), 0);
			QCOMPARE(model->isUseAnimations(), true);

			model->setUseAnimations(false);
			QCOMPARE(animationsSpy.count(), 1);
			QCOMPARE(model->isUseAnimations(), false);

			model->setUseAnimations(false);
			QCOMPARE(animationsSpy.count(), 1);
			QCOMPARE(model->isUseAnimations(), false);

			model->setUseAnimations(true);
			QCOMPARE(animationsSpy.count(), 2);
			QCOMPARE(model->isUseAnimations(), true);

			model->setUseAnimations(true);
			QCOMPARE(animationsSpy.count(), 2);
			QCOMPARE(model->isUseAnimations(), true);
		}


		void testModeOption()
		{
			const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			auto* model = Env::getSingleton<SettingsModel>();
			QCOMPARE(model->getDarkMode(), SettingsModel::ModeOption::AUTO);

			model->setDarkMode(SettingsModel::ModeOption::OFF);
			QCOMPARE(settings.getDarkMode(), "OFF"_L1);
			QCOMPARE(model->getDarkMode(), SettingsModel::ModeOption::OFF);

			model->setDarkMode(SettingsModel::ModeOption::AUTO);
			QCOMPARE(settings.getDarkMode(), "AUTO"_L1);
			QCOMPARE(model->getDarkMode(), SettingsModel::ModeOption::AUTO);

			model->setDarkMode(SettingsModel::ModeOption::ON);
			QCOMPARE(settings.getDarkMode(), "ON"_L1);
			QCOMPARE(model->getDarkMode(), SettingsModel::ModeOption::ON);
		}


		void testTechnology()
		{
			const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			auto* model = Env::getSingleton<SettingsModel>();
			QSignalSpy technologySpy(model, &SettingsModel::firePreferredTechnologyChanged);
			QCOMPARE(model->getPreferredTechnology(), ReaderManagerPluginType::REMOTE_IFD);

			model->setPreferredTechnology(ReaderManagerPluginType::NFC);
			QCOMPARE(technologySpy.count(), 1);
			QCOMPARE(settings.getPreferredTechnology(), "NFC"_L1);
			QCOMPARE(model->getPreferredTechnology(), ReaderManagerPluginType::NFC);

			model->setPreferredTechnology(ReaderManagerPluginType::SIMULATOR);
			QCOMPARE(technologySpy.count(), 2);
			QCOMPARE(settings.getPreferredTechnology(), "SIMULATOR"_L1);
			QCOMPARE(model->getPreferredTechnology(), ReaderManagerPluginType::REMOTE_IFD);

			model->setSimulatorEnabled(true);
			QCOMPARE(technologySpy.count(), 3);
			QCOMPARE(settings.getPreferredTechnology(), "SIMULATOR"_L1);
			QCOMPARE(model->getPreferredTechnology(), ReaderManagerPluginType::SIMULATOR);

			model->setPreferredTechnology(ReaderManagerPluginType::NFC);
			QCOMPARE(technologySpy.count(), 4);
			QCOMPARE(settings.getPreferredTechnology(), "NFC"_L1);
			QCOMPARE(model->getPreferredTechnology(), ReaderManagerPluginType::NFC);
		}


		void testStartupModule()
		{
			auto* model = Env::getSingleton<SettingsModel>();
			QSignalSpy startupModuleSpy(model, &SettingsModel::fireStartupModuleChanged);

			QCOMPARE(model->getShowOnboarding(), true);
			QCOMPARE(model->getStartupModule(), UiModule::ONBOARDING);

			model->setStartupModule(UiModule::DEFAULT);
			QCOMPARE(startupModuleSpy.count(), 1);
			QCOMPARE(model->getStartupModule(), UiModule::ONBOARDING);

			model->setShowOnboarding(false);
			QCOMPARE(model->getShowOnboarding(), false);
			QCOMPARE(startupModuleSpy.count(), 2);
			QCOMPARE(model->getStartupModule(), UiModule::DEFAULT);

			model->setShowOnboarding(false);
			QCOMPARE(startupModuleSpy.count(), 2);
		}


		void test_resetHideableDialogs()
		{
			auto* settingsModel = Env::getSingleton<SettingsModel>();
			settingsModel->setTransportPinReminder(false);
			settingsModel->setRemindUserToClose(false);
			auto remindRedirectSpy = QSignalSpy(settingsModel, &SettingsModel::fireRemindUserOfAutoRedirectChanged);
			settingsModel->setRemindUserOfAutoRedirect(false);
			QCOMPARE(remindRedirectSpy.count(), 1);
			settingsModel->hideFutureStoreFeedbackDialogs();
			settingsModel->setStartupModule(UiModule::REMOTE_SERVICE);

			QVERIFY(!settingsModel->isTransportPinReminder());
			QVERIFY(!settingsModel->isRemindUserToClose());
			QVERIFY(!settingsModel->isRemindUserOfAutoRedirect());
			QVERIFY(!settingsModel->requestStoreFeedback());
			QCOMPARE(settingsModel->getStartupModule(), UiModule::REMOTE_SERVICE);

			settingsModel->resetHideableDialogs();
			QVERIFY(settingsModel->isTransportPinReminder());
			QVERIFY(settingsModel->isRemindUserToClose());
			QVERIFY(settingsModel->isRemindUserOfAutoRedirect());
			QVERIFY(remindRedirectSpy.count() > 1);
			QVERIFY(settingsModel->requestStoreFeedback());
			QCOMPARE(settingsModel->getStartupModule(), UiModule::ONBOARDING);
		}


		void test_showBetaTesting()
		{
			const bool isBetaVersion = VersionNumber::getApplicationVersion().isDeveloperVersion();

			auto* settingsModel = Env::getSingleton<SettingsModel>();
			QCOMPARE(settingsModel->getShowBetaTesting(), isBetaVersion);

			settingsModel->setShowBetaTesting(false);
			QCOMPARE(settingsModel->getShowBetaTesting(), false);

			settingsModel->setShowBetaTesting(true);
			QCOMPARE(settingsModel->getShowBetaTesting(), isBetaVersion);
		}


		void testAppendTransportPin()
		{
			const auto* const env = "APPEND_TRANSPORT_PIN";
			QVERIFY(!qEnvironmentVariableIsSet(env));

			auto* model = Env::getSingleton<SettingsModel>();
			model->setAppendTransportPin(QString());
			QVERIFY(!qEnvironmentVariableIsSet(env));
			QCOMPARE(model->getAppendTransportPin(), ""_L1);

			model->setAppendTransportPin(""_L1);
			QVERIFY(!qEnvironmentVariableIsSet(env));
			QCOMPARE(model->getAppendTransportPin(), ""_L1);

			model->setAppendTransportPin("1"_L1);
			QCOMPARE(qEnvironmentVariable(env), "1"_L1);
			QCOMPARE(model->getAppendTransportPin(), "1"_L1);

			model->setAppendTransportPin(QString());
			QVERIFY(!qEnvironmentVariableIsSet(env));
			QCOMPARE(model->getAppendTransportPin(), ""_L1);
		}


		void test_startupModuleIsOnboarding_data()
		{
			QTest::addColumn<bool>("showOnboarding");
			QTest::addColumn<bool>("onboardingShown");
			QTest::addColumn<bool>("startupModuleIsOnboarding");

			QTest::addRow("1") << true << false << true;
			QTest::addRow("2") << true << true << false;
			QTest::addRow("3") << false << false << false;
			QTest::addRow("4") << false << true << false;
		}


		void test_startupModuleIsOnboarding()
		{
			QFETCH(bool, showOnboarding);
			QFETCH(bool, onboardingShown);
			QFETCH(bool, startupModuleIsOnboarding);

			auto* model = Env::getSingleton<SettingsModel>();

			model->setShowOnboarding(showOnboarding);
			model->setOnboardingShown(onboardingShown);
			QCOMPARE(model->getStartupModule() == UiModule::ONBOARDING, startupModuleIsOnboarding);
		}


		void test_manualUpdateCheck()
		{
			auto* settingsModel = Env::getSingleton<SettingsModel>();
			QSignalSpy spy(settingsModel, &SettingsModel::fireAppUpdateDataChanged);
			Q_EMIT Env::getSingleton<AppUpdateDataModel>()->fireAppUpdateDataChanged();

			QCOMPARE(spy.count(), 1);
			QList<QVariant> arguments = spy.takeFirst();
			QVERIFY(arguments.at(0).toBool() == false);
			spy.clear();

			settingsModel->updateAppcast();
			Q_EMIT Env::getSingleton<AppUpdateDataModel>()->fireAppUpdateDataChanged();
			QCOMPARE(spy.count(), 1);
			arguments = spy.takeFirst();
			QVERIFY(arguments.at(0).toBool() == true);
		}


		void test_isAutoRedirectAfterAuthentication()
		{
			auto* settings = Env::getSingleton<SettingsModel>();

			QSignalSpy spy(settings, &SettingsModel::fireRemindUserOfAutoRedirectChanged);
			bool initial = settings->isRemindUserOfAutoRedirect();
			bool newValue = !initial;

			settings->setRemindUserOfAutoRedirect(newValue);
			QCOMPARE(spy.count(), 1);
			QCOMPARE(settings->isRemindUserOfAutoRedirect(), newValue);

			settings->setRemindUserOfAutoRedirect(initial);
			QCOMPARE(spy.count(), 2);
			QCOMPARE(settings->isRemindUserOfAutoRedirect(), initial);
		}


		void test_onNfcStateChanged_data()
		{
			QTest::addColumn<ApplicationModel::NfcState>("nfcState");
			QTest::addColumn<ReaderManagerPluginType>("initialPreferredTechnology");
			QTest::addColumn<ReaderManagerPluginType>("finalPreferredTechnology");

			QTest::addRow("NFC ready and selected") << ApplicationModel::NfcState::READY << ReaderManagerPluginType::NFC << ReaderManagerPluginType::NFC;
			QTest::addRow("NFC inactive and selected") << ApplicationModel::NfcState::INACTIVE << ReaderManagerPluginType::NFC << ReaderManagerPluginType::NFC;
			QTest::addRow("NFC disabled and selected") << ApplicationModel::NfcState::DISABLED << ReaderManagerPluginType::NFC << ReaderManagerPluginType::NFC;
			QTest::addRow("NFC unavailable and selected") << ApplicationModel::NfcState::UNAVAILABLE << ReaderManagerPluginType::NFC << ReaderManagerPluginType::REMOTE_IFD;
			QTest::addRow("NFC unavailable and simulator selected") << ApplicationModel::NfcState::UNAVAILABLE << ReaderManagerPluginType::SIMULATOR << ReaderManagerPluginType::SIMULATOR;
			QTest::addRow("NFC unavailable and remote ifd selected") << ApplicationModel::NfcState::UNAVAILABLE << ReaderManagerPluginType::REMOTE_IFD << ReaderManagerPluginType::REMOTE_IFD;
		}


		void test_onNfcStateChanged()
		{
			QFETCH(ApplicationModel::NfcState, nfcState);
			QFETCH(ReaderManagerPluginType, initialPreferredTechnology);
			QFETCH(ReaderManagerPluginType, finalPreferredTechnology);

			auto* settings = Env::getSingleton<SettingsModel>();
			settings->setPreferredTechnology(initialPreferredTechnology);
			QSignalSpy spy(settings, &SettingsModel::firePreferredTechnologyChanged);

			settings->onNfcStateChanged(nfcState);
			QCOMPARE(settings->getPreferredTechnology(), finalPreferredTechnology);
			QCOMPARE(spy.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_SettingsModel)
#include "test_SettingsModel.moc"
