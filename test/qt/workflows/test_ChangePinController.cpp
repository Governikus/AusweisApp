/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Test for the Change PIN functionality.
 */

#include "controller/ChangePinController.h"

#include "ReaderManager.h"
#include "context/ChangePinContext.h"
#include "states/AbstractState.h"
#include "states/StateUpdateRetryCounter.h"

#include <QSharedPointer>
#include <QtTest>


Q_IMPORT_PLUGIN(SimulatorReaderManagerPlugIn)


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
			const auto readerManager = Env::getSingleton<ReaderManager>();
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
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->disconnect(this);
			readerManager->shutdown();
		}


		void init()
		{
			mChangePinContext.reset(new ChangePinContext());
			mChangePinContext->setReaderPlugInTypes({ReaderManagerPlugInType::SIMULATOR});
			connect(mChangePinContext.data(), &WorkflowContext::fireStateChanged, this, &test_ChangePinController::onStateChanged);

			mChangePinController.reset(new ChangePinController(mChangePinContext));

			mRetryCounterUpdated = false;
		}


		void cleanup()
		{
			mChangePinContext->disconnect(this);
			mChangePinController.reset();
			mChangePinContext.reset();
		}


		void testSuccess()
		{
			QSignalSpy controllerFinishedSpy(mChangePinController.data(), &ChangePinController::fireComplete);

			mChangePinController->run();
			QVERIFY(controllerFinishedSpy.wait());
			QCOMPARE(mChangePinContext->getStatus().getStatusCode(), GlobalStatus::Code::No_Error);
		}


};

QTEST_GUILESS_MAIN(test_ChangePinController)
#include "test_ChangePinController.moc"
