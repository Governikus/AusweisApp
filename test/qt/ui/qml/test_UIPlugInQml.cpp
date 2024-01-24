/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInQml.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/IfdServiceContext.h"
#include "context/SelfAuthContext.h"
#if __has_include("context/PersonalizationContext.h")
	#include "PersonalizationModel.h"
	#include "context/PersonalizationContext.h"
#endif
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
#include "VolatileSettings.h"

#include "TestWorkflowController.h"

#include <QSignalSpy>
#include <QtTest>


using namespace governikus;

Q_DECLARE_METATYPE(QSharedPointer<WorkflowRequest>)

class test_UIPlugInQml
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_onWorkflowChanged_data()
		{
			QTest::addColumn<QSharedPointer<WorkflowRequest>>("request");

			QTest::newRow("AuthContext") << TestWorkflowController::createWorkflowRequest<AuthContext>();
			QTest::newRow("SelfAuthContext") << TestWorkflowController::createWorkflowRequest<SelfAuthContext>();
			QTest::newRow("ChangePinContext") << TestWorkflowController::createWorkflowRequest<ChangePinContext>();
			QTest::newRow("IfdServiceContext") << TestWorkflowController::createWorkflowRequest<IfdServiceContext>(QSharedPointer<IfdServer>(new MockIfdServer()));
#if __has_include("context/PersonalizationContext.h")
			QTest::newRow("PersonalizationContext") << TestWorkflowController::createWorkflowRequest<PersonalizationContext>(QString());
#endif
		}


		void test_onWorkflowChanged()
		{
			QFETCH(QSharedPointer<WorkflowRequest>, request);
			const auto& context = request->getContext();

			const bool isAuth = !context.objectCast<AuthContext>().isNull();
			const bool isSelfAuth = !context.objectCast<SelfAuthContext>().isNull();
			const bool isPinChange = !context.objectCast<ChangePinContext>().isNull();
			const bool isIfdService = !context.objectCast<IfdServiceContext>().isNull();
#if __has_include("context/PersonalizationContext.h")
			const bool isPersonalization = !context.objectCast<PersonalizationContext>().isNull();
#else
			const bool isPersonalization = false;
#endif

			QCOMPARE(Env::getSingleton<VolatileSettings>()->getDelay(), 0);
			QVERIFY(!Env::getSingleton<ApplicationModel>()->mContext);
			QVERIFY(!Env::getSingleton<NumberModel>()->mContext);
			QVERIFY(!Env::getSingleton<ChangePinModel>()->mContext);
#if __has_include("context/PersonalizationContext.h")
			QVERIFY(!Env::getSingleton<PersonalizationModel>()->mContext);
#endif
			QVERIFY(!Env::getSingleton<CertificateDescriptionModel>()->mContext);
			QVERIFY(!Env::getSingleton<ChatModel>()->mContext);
			QVERIFY(!Env::getSingleton<AuthModel>()->mContext);
			QVERIFY(!Env::getSingleton<SelfAuthModel>()->mContext);
			QVERIFY(!Env::getSingleton<RemoteServiceModel>()->mContext);

			UIPlugInQml plugin;
			plugin.onWorkflowStarted(request);
			QCOMPARE(Env::getSingleton<VolatileSettings>()->getDelay(), 1000);
			QVERIFY(Env::getSingleton<ApplicationModel>()->mContext);
			QVERIFY(Env::getSingleton<NumberModel>()->mContext);
			QCOMPARE(!Env::getSingleton<ChangePinModel>()->mContext.isNull(), isPinChange);
#if __has_include("context/PersonalizationContext.h")
			QCOMPARE(!Env::getSingleton<PersonalizationModel>()->mContext.isNull(), isPersonalization);
#endif
			QCOMPARE(!Env::getSingleton<CertificateDescriptionModel>()->mContext.isNull(), isAuth || isIfdService || isPersonalization);
			QCOMPARE(!Env::getSingleton<ChatModel>()->mContext.isNull(), isAuth || isIfdService || isPersonalization);
			QCOMPARE(!Env::getSingleton<AuthModel>()->mContext.isNull(), isAuth && !isPersonalization);
			QCOMPARE(!Env::getSingleton<SelfAuthModel>()->mContext.isNull(), isSelfAuth);
			QCOMPARE(!Env::getSingleton<RemoteServiceModel>()->mContext.isNull(), isIfdService);

			plugin.onWorkflowFinished(request);
			QCOMPARE(Env::getSingleton<VolatileSettings>()->getDelay(), 0);
			QVERIFY(!Env::getSingleton<ApplicationModel>()->mContext);
			QVERIFY(!Env::getSingleton<NumberModel>()->mContext);
			QVERIFY(!Env::getSingleton<ChangePinModel>()->mContext);
#if __has_include("context/PersonalizationContext.h")
			QVERIFY(!Env::getSingleton<PersonalizationModel>()->mContext);
#endif
			QVERIFY(!Env::getSingleton<CertificateDescriptionModel>()->mContext);
			QVERIFY(!Env::getSingleton<ChatModel>()->mContext);
			QVERIFY(!Env::getSingleton<AuthModel>()->mContext);
			QVERIFY(!Env::getSingleton<SelfAuthModel>()->mContext);
			QVERIFY(!Env::getSingleton<RemoteServiceModel>()->mContext);
		}


		void test_useSystemFontChanged()
		{
			ResourceLoader::getInstance().init();
			UIPlugInQml plugin;

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
			UIPlugInQml plugin;
			QSignalSpy spy(&plugin, &UIPlugInQml::fireFontScaleFactorChanged);

			const auto initialValue = plugin.getFontScaleFactor();

			plugin.setFontScaleFactor(initialValue);
			QTRY_COMPARE(spy.count(), 0);

			const auto newValue = 42;
			plugin.setFontScaleFactor(newValue);
			QTRY_COMPARE(spy.count(), 1);
			QCOMPARE(plugin.getFontScaleFactor(), newValue);
		}


		void test_setDarkMode()
		{
			UIPlugInQml plugin;
			QSignalSpy spy(&plugin, &UIPlugInQml::fireOsDarkModeChanged);

			const auto initialValue = plugin.isOsDarkModeEnabled();

			plugin.setOsDarkMode(initialValue);
			QTRY_COMPARE(spy.count(), 0);

			const auto newValue = !initialValue;
			plugin.setOsDarkMode(newValue);
			QTRY_COMPARE(spy.count(), 1);
			QCOMPARE(plugin.isOsDarkModeEnabled(), newValue);
		}


};

QTEST_MAIN(test_UIPlugInQml)
#include "test_UIPlugInQml.moc"
