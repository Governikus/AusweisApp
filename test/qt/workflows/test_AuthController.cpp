/**
 * Copyright (c) 2024-2026 Governikus Service GmbH, Germany
 */

#include "controller/AuthController.h"

#include "AppSettings.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "VolatileSettings.h"
#include "states/StateEditAccessRights.h"
#include "states/StateEnterPacePassword.h"

#include <QtTest>


Q_IMPORT_PLUGIN(SimulatorReaderManagerPlugin)


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_AuthController
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<AuthContext> mAuthContext;

		void onStateChanged(const QString& pNextState)
		{
			if (StateBuilder::isState<StateEditAccessRights>(pNextState))
			{
				QCOMPARE(mAuthContext->getAccessRightManager()->getRequiredAccessRights(), {AccessRight::READ_DG01});
				QCOMPARE(mAuthContext->getAccessRightManager()->getOptionalAccessRights(), {AccessRight::READ_DG02});

				*mAuthContext->getAccessRightManager() -= AccessRight::READ_DG02;
				QVERIFY(!mAuthContext->getAccessRightManager()->getEffectiveAccessRights().contains(AccessRight::READ_DG02));
			}
			else if (StateBuilder::isState<StateEnterPacePassword>(pNextState))
			{
				if (Env::getSingleton<AppSettings>()->getSimulatorSettings().isBasicReader())
				{
					mAuthContext->setPin("123456"_L1);
				}
			}

			mAuthContext->setStateApproved();
		}

	private Q_SLOTS:
		void createWorkflowRequest()
		{
			const QUrl url("https://www.test.de"_L1);
			const QVariant data(42);
			const AuthContext::BrowserHandler handler = [](const QSharedPointer<AuthContext>&){
						return QStringLiteral("HelloWorld");
					};
			const auto& request = AuthController::createWorkflowRequest(url, data, handler);

			const auto& context = request->getContext().objectCast<AuthContext>();
			QVERIFY(context);
			QVERIFY(context->isActivateUi());
			QCOMPARE(context->getActivationUrl(), url);
			QCOMPARE(context->getBrowserHandler()(context), QStringLiteral("HelloWorld"));
					QCOMPARE(request->getData().value<int>(), 42);
			QCOMPARE(context->isSkipMobileRedirect(), false);

			context->setWorkflowFinished(true);
			const auto handling = request->handleBusyWorkflow(request, QSharedPointer<WorkflowRequest>());
			QCOMPARE(handling, WorkflowControl::ENQUEUE);
			QCOMPARE(context->isStateApproved(), true);
			QVERIFY(context->isSkipMobileRedirect());
		}


		void initTestCase()
		{
			ResourceLoader::getInstance().init();
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);
			Env::getSingleton<AppSettings>()->getSimulatorSettings().setEnabled(true);
			Env::getSingleton<ReaderManager>()->init();
		}


		void cleanupTestCase()
		{
			auto* readerManager = Env::getSingleton<ReaderManager>();
			readerManager->disconnect(this);
			readerManager->shutdown();
		}


		void testSuccess_data()
		{
			QTest::addColumn<bool>("basicReader");

			QTest::newRow("basic reader") << true;
			QTest::newRow("comfort reader") << false;
		}


		void testSuccess()
		{
			QScopedPointer<WorkflowController> authController;

			QFETCH(bool, basicReader);
			Env::getSingleton<AppSettings>()->getSimulatorSettings().setBasicReader(basicReader);

			const QUrl activationUrl("http://127.0.0.1:24727/eID-Client?tcTokenURL=https://test.governikus-eid.de/Autent-DemoApplication/api/eid/request?documentType=REQUIRED%26issuingState=ALLOWED"_L1);
			mAuthContext.reset(new AuthContext(true, activationUrl));
			mAuthContext->setReaderPluginTypes({ReaderManagerPluginType::SIMULATOR});
			connect(mAuthContext.data(), &WorkflowContext::fireStateChanged, this, &test_AuthController::onStateChanged);

			authController.reset(new AuthController(mAuthContext));
			QSignalSpy controllerFinishedSpy(authController.data(), &AuthController::fireComplete);

			authController->run();

			QVERIFY(controllerFinishedSpy.wait(std::chrono::seconds{30}));

			QCOMPARE(mAuthContext->getStatus().getStatusCode(), GlobalStatus::Code::No_Error);
			QCOMPARE(mAuthContext->getTcTokenUrl(), QUrl("https://test.governikus-eid.de/Autent-DemoApplication/api/eid/request?documentType=REQUIRED&issuingState=ALLOWED"_L1));
			QVERIFY(!mAuthContext->getTcToken().isNull());
			QCOMPARE(mAuthContext->getTcToken()->getServerAddress(), QUrl("https://testpaos.governikus-eid.de:443/ecardpaos/paosreceiver"_L1));
			QVERIFY(mAuthContext->getRefreshUrl().matches(QUrl("https://test.governikus-eid.de/Autent-DemoApplication/refresh-address"_L1), QUrl::RemoveQuery));

			mAuthContext->disconnect(this);
			authController.reset();
			mAuthContext.reset();
		}


};

QTEST_GUILESS_MAIN(test_AuthController)
#include "test_AuthController.moc"
