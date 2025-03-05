/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "controller/ChangePinController.h"

#include "AppSettings.h"
#include "ReaderManager.h"
#include "VolatileSettings.h"
#include "context/ChangePinContext.h"
#include "states/StateUpdateRetryCounter.h"

#include <QSharedPointer>
#include <QtTest>


Q_IMPORT_PLUGIN(SimulatorReaderManagerPlugin)


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_ChangePinController
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<ChangePinContext> mChangePinContext;
		QScopedPointer<WorkflowController> mChangePinController;
		bool mRetryCounterUpdated = false;


		void onStateChanged(const QString& pNextState)
		{
			if (mRetryCounterUpdated)
			{
				mRetryCounterUpdated = false;
				if (mChangePinContext->getLastPaceResult() != CardReturnCode::OK)
				{
					QCOMPARE(mChangePinContext->getLastPaceResult(), CardReturnCode::INVALID_PIN);
					Q_EMIT mChangePinContext->fireCancelWorkflow();
					return;
				}
			}

			if (StateBuilder::isState<StateUpdateRetryCounter>(pNextState))
			{
				mRetryCounterUpdated = true;
			}

			mChangePinContext->setStateApproved();
		}

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(false);
			Env::getSingleton<AppSettings>()->getSimulatorSettings().setEnabled(true);

			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, [] (const ReaderInfo& pInfo){
						if (pInfo.isInsertable())
						{
							Env::getSingleton<ReaderManager>()->insert(pInfo);
						}
					});

			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
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
			QFETCH(bool, basicReader);
			Env::getSingleton<AppSettings>()->getSimulatorSettings().setBasicReader(basicReader);

			mChangePinContext.reset(new ChangePinContext());
			if (basicReader)
			{
				mChangePinContext->setPin("123456"_L1);
				mChangePinContext->setNewPin("123456"_L1);
			}
			mChangePinContext->setReaderPluginTypes({ReaderManagerPluginType::SIMULATOR});
			connect(mChangePinContext.data(), &WorkflowContext::fireStateChanged, this, &test_ChangePinController::onStateChanged);
			mChangePinController.reset(new ChangePinController(mChangePinContext));
			mRetryCounterUpdated = false;

			QSignalSpy controllerFinishedSpy(mChangePinController.data(), &ChangePinController::fireComplete);
			mChangePinController->run();
			QVERIFY(controllerFinishedSpy.wait());
			QCOMPARE(mChangePinContext->getStatus().getStatusCode(), GlobalStatus::Code::No_Error);

			mChangePinContext->disconnect(this);
			mChangePinController.reset();
			mChangePinContext.reset();
		}


		void createWorkflowRequest_data()
		{
			QTest::addColumn<bool>("requestTransportPin");
			QTest::addColumn<bool>("activateUi");

			QTest::newRow("PIN 5") << true << false;
			QTest::newRow("PIN 5 + ACTIVATE") << true << true;
			QTest::newRow("PIN 6") << false << false;
			QTest::newRow("PIN 6 + ACTIVATE") << false << true;
		}


		void createWorkflowRequest()
		{
			QFETCH(bool, requestTransportPin);
			QFETCH(bool, activateUi);

			const auto& request = ChangePinController::createWorkflowRequest(requestTransportPin, activateUi);

			const auto& context = request->getContext();
			QCOMPARE(context->isRequestTransportPin(), requestTransportPin);
			QCOMPARE(context->isActivateUi(), activateUi);
		}


};

QTEST_GUILESS_MAIN(test_ChangePinController)
#include "test_ChangePinController.moc"
