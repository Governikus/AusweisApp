/**
 * Copyright (c) 2023-2026 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginQml.h"

#include "AppSettings.h"
#include "ApplicationModel.h"
#include "AuthModel.h"
#include "CertificateDescriptionModel.h"
#include "ChangePinModel.h"
#include "ChatModel.h"
#include "MockIfdServer.h"
#include "NumberModel.h"
#include "RemoteServiceModel.h"
#include "ResourceLoader.h"
#include "SelfAuthModel.h"
#include "SettingsModel.h"
#include "VolatileSettings.h"
#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/IfdServiceContext.h"
#include "context/SelfAuthContext.h"

#include "TestWorkflowController.h"

#include <QSignalSpy>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_METATYPE(QSharedPointer<WorkflowRequest>)

class test_UiPluginQml
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void init()
		{
			qunsetenv("OVERRIDE_PLATFORM");
		}


		void test_getQtVersion()
		{
			UiPluginQml plugin;
			QVERIFY(!plugin.getQtVersion().isEmpty());
		}


		void test_onWorkflowChanged_data()
		{
			QTest::addColumn<QSharedPointer<WorkflowRequest>>("request");

			QTest::newRow("AuthContext") << TestWorkflowController::createWorkflowRequest<AuthContext>();
			QTest::newRow("SelfAuthContext") << TestWorkflowController::createWorkflowRequest<SelfAuthContext>();
			QTest::newRow("ChangePinContext") << TestWorkflowController::createWorkflowRequest<ChangePinContext>();
			QTest::newRow("IfdServiceContext") << TestWorkflowController::createWorkflowRequest<IfdServiceContext>(QSharedPointer<IfdServer>(new MockIfdServer()));
		}


		void test_onWorkflowChanged()
		{
			QFETCH(QSharedPointer<WorkflowRequest>, request);
			const auto& context = request->getContext();

			const bool isAuth = !context.objectCast<AuthContext>().isNull();
			const bool isSelfAuth = !context.objectCast<SelfAuthContext>().isNull();
			const bool isPinChange = !context.objectCast<ChangePinContext>().isNull();
			const bool isIfdService = !context.objectCast<IfdServiceContext>().isNull();

			QCOMPARE(Env::getSingleton<VolatileSettings>()->getDelay(), 0);
			QVERIFY(!Env::getSingleton<ApplicationModel>()->mContext);
			QVERIFY(!Env::getSingleton<NumberModel>()->mContext);
			QVERIFY(!Env::getSingleton<ChangePinModel>()->mContext);
			QVERIFY(!Env::getSingleton<CertificateDescriptionModel>()->mContext);
			QVERIFY(!Env::getSingleton<ChatModel>()->mContext);
			QVERIFY(!Env::getSingleton<AuthModel>()->mContext);
			QVERIFY(!Env::getSingleton<SelfAuthModel>()->mContext);
			QVERIFY(!Env::getSingleton<RemoteServiceModel>()->mContext);

			UiPluginQml plugin;
			plugin.onWorkflowStarted(request);
			QCOMPARE(Env::getSingleton<VolatileSettings>()->getDelay(), 1000);
			QVERIFY(Env::getSingleton<ApplicationModel>()->mContext);
			QVERIFY(Env::getSingleton<NumberModel>()->mContext);
			QCOMPARE(!Env::getSingleton<ChangePinModel>()->mContext.isNull(), isPinChange);
			QCOMPARE(!Env::getSingleton<CertificateDescriptionModel>()->mContext.isNull(), isAuth || isIfdService);
			QCOMPARE(!Env::getSingleton<ChatModel>()->mContext.isNull(), isAuth || isIfdService);
			QCOMPARE(!Env::getSingleton<AuthModel>()->mContext.isNull(), isAuth);
			QCOMPARE(!Env::getSingleton<SelfAuthModel>()->mContext.isNull(), isSelfAuth);
			QCOMPARE(!Env::getSingleton<RemoteServiceModel>()->mContext.isNull(), isIfdService);

			plugin.onWorkflowFinished(request);
			QCOMPARE(Env::getSingleton<VolatileSettings>()->getDelay(), 0);
			QVERIFY(!Env::getSingleton<ApplicationModel>()->mContext);
			QVERIFY(!Env::getSingleton<NumberModel>()->mContext);
			QVERIFY(!Env::getSingleton<ChangePinModel>()->mContext);
			QVERIFY(!Env::getSingleton<CertificateDescriptionModel>()->mContext);
			QVERIFY(!Env::getSingleton<ChatModel>()->mContext);
			QVERIFY(!Env::getSingleton<AuthModel>()->mContext);
			QVERIFY(!Env::getSingleton<SelfAuthModel>()->mContext);
			QVERIFY(!Env::getSingleton<RemoteServiceModel>()->mContext);
		}


		void test_useSystemFontChanged()
		{
			ResourceLoader::getInstance().init();
			UiPluginQml plugin;

			auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			QVERIFY(!generalSettings.isUseSystemFont());
			QTRY_VERIFY(QGuiApplication::font().family().contains(QStringLiteral("roboto"), Qt::CaseInsensitive));

			generalSettings.setUseSystemFont(true);
			//We call this manually, because the signal-connection is tied to an application window being present.
			plugin.onUseSystemFontChanged();
			QTRY_VERIFY(!QGuiApplication::font().family().contains(QStringLiteral("roboto"), Qt::CaseInsensitive));
			ResourceLoader::getInstance().shutdown();
		}


		void test_setFontScaleFactor()
		{
			UiPluginQml plugin;
			QSignalSpy spy(&plugin, &UiPluginQml::fireFontScaleFactorChanged);

			const auto initialValue = plugin.getFontScaleFactor();

			plugin.setFontScaleFactor(initialValue);
			QTRY_COMPARE(spy.count(), 0);

			const auto newValue = 42;
			plugin.setFontScaleFactor(newValue);
			QTRY_COMPARE(spy.count(), 1);
			QCOMPARE(plugin.getFontScaleFactor(), newValue);
		}


		void test_setFontWeightAdjustment()
		{
			UiPluginQml plugin;
			QSignalSpy spy(&plugin, &UiPluginQml::fireFontWeightAdjustmentChanged);

			const auto initialValue = plugin.getFontWeightAdjustment();

			plugin.setFontWeightAdjustment(initialValue);
			QTRY_COMPARE(spy.count(), 0);

			const auto newValue = 42;
			plugin.setFontWeightAdjustment(newValue);
			QTRY_COMPARE(spy.count(), 1);
			QCOMPARE(plugin.getFontWeightAdjustment(), newValue);
		}


		void test_darkModeEnabled_data()
		{
			QTest::addColumn<SettingsModel::ModeOption>("userDarkMode");
			QTest::addColumn<bool>("osDarkMode");
			QTest::addColumn<int>("expectedSpyCount");
			QTest::addColumn<bool>("darkModeEnabled");

			QTest::addRow("AUTO - OS Light Mode") << SettingsModel::ModeOption::AUTO << false << 0 << false;
			QTest::addRow("AUTO - OS Dark Mode") << SettingsModel::ModeOption::AUTO << true << 1 << true;
			QTest::addRow("ON - OS Light Mode") << SettingsModel::ModeOption::ON << false << 0 << true;
			QTest::addRow("ON - OS Dark Mode") << SettingsModel::ModeOption::ON << true << 1 << true;
			QTest::addRow("OFF - OS Light Mode") << SettingsModel::ModeOption::OFF << false << 0 << false;
			QTest::addRow("OFF - OS Dark Mode") << SettingsModel::ModeOption::OFF << true << 1 << false;
		}


		void test_darkModeEnabled()
		{
			QFETCH(SettingsModel::ModeOption, userDarkMode);
			QFETCH(bool, osDarkMode);
			QFETCH(int, expectedSpyCount);
			QFETCH(bool, darkModeEnabled);

			UiPluginQml plugin;
			auto* settingsModel = Env::getSingleton<SettingsModel>();

			QSignalSpy spy(&plugin, &UiPluginQml::fireDarkModeEnabledChanged);

			plugin.setOsDarkMode(osDarkMode);
			settingsModel->setDarkMode(userDarkMode);

			QCOMPARE(plugin.isDarkModeEnabled(), darkModeEnabled);

			QCOMPARE(spy.count(), expectedSpyCount);
		}


		void test_overridePlatform()
		{
			QCOMPARE(UiPluginQml::getOverridePlatform(), QString());
			qputenv("OVERRIDE_PLATFORM", "dummy"_ba);
			QCOMPARE(UiPluginQml::getOverridePlatform(), QStringLiteral("dummy"));
		}


		void test_adjustQmlImportPath()
		{
			const auto& primaryPrefix = QStringLiteral("qrc:/qt/qml");
			QCOMPARE(UiPluginQml::adjustQmlImportPath(nullptr), primaryPrefix);

			QQmlEngine engine;
			QCOMPARE(UiPluginQml::adjustQmlImportPath(&engine), primaryPrefix);
			QVERIFY(engine.importPathList().contains(primaryPrefix));

			qputenv("OVERRIDE_PLATFORM", "platform"_ba);
			QTest::ignoreMessage(QtDebugMsg, "Override platform: \"platform\"");
			QCOMPARE(UiPluginQml::adjustQmlImportPath(&engine), "qrc:/platform"_L1);
			QVERIFY(!engine.importPathList().contains(primaryPrefix));
			QVERIFY(engine.importPathList().contains("qrc:/platform"_L1));
		}


		void test_updatePending()
		{
			UiPluginQml plugin;
			QSignalSpy spy(&plugin, &UiPluginModel::fireIsUpdatePendingChanged);

			QVERIFY(!plugin.isUpdatePending());

			plugin.setUpdatePending(false);
			QVERIFY(!plugin.isUpdatePending());
			QCOMPARE(spy.count(), 0);

			plugin.setUpdatePending(true);
			QVERIFY(plugin.isUpdatePending());
			QCOMPARE(spy.count(), 1);

			plugin.setUpdatePending(false);
			QVERIFY(!plugin.isUpdatePending());
			QCOMPARE(spy.count(), 2);
		}


		void test_a11yButtonShape()
		{
			UiPluginQml plugin;
			QSignalSpy spy(&plugin, &UiPluginModel::fireA11yButtonShapeActiveChanged);

			QVERIFY(!plugin.isA11yButtonShapeActive());

			plugin.setA11yButtonShapeActive(false);
			QVERIFY(!plugin.isA11yButtonShapeActive());
			QCOMPARE(spy.count(), 0);

			plugin.setA11yButtonShapeActive(true);
			QVERIFY(plugin.isA11yButtonShapeActive());
			QCOMPARE(spy.count(), 1);

			plugin.setA11yButtonShapeActive(false);
			QVERIFY(!plugin.isA11yButtonShapeActive());
			QCOMPARE(spy.count(), 2);
		}


		void test_a11yOnOffSwitchLabel()
		{
			UiPluginQml plugin;
			QSignalSpy spy(&plugin, &UiPluginModel::fireA11yOnOffSwitchLabelActiveChanged);

			QVERIFY(!plugin.isA11yOnOffSwitchLabelActive());

			plugin.setA11yOnOffSwitchLabelActive(false);
			QVERIFY(!plugin.isA11yOnOffSwitchLabelActive());
			QCOMPARE(spy.count(), 0);

			plugin.setA11yOnOffSwitchLabelActive(true);
			QVERIFY(plugin.isA11yOnOffSwitchLabelActive());
			QCOMPARE(spy.count(), 1);

			plugin.setA11yOnOffSwitchLabelActive(false);
			QVERIFY(!plugin.isA11yOnOffSwitchLabelActive());
			QCOMPARE(spy.count(), 2);
		}


};

QTEST_MAIN(test_UiPluginQml)
#include "test_UiPluginQml.moc"
