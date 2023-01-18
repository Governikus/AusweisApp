/*!
 * \brief Test for the Change PIN functionality.
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
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


		void onCardInfoChanged(const ReaderInfo& pInfo)
		{
			if (pInfo.isInsertable())
			{
				Env::getSingleton<ReaderManager>()->insert(pInfo);
			}
		}


		void onCardInserted(const ReaderInfo& pInfo)
		{
			if (pInfo.hasEid())
			{
				Q_EMIT fireEidCardInserted();
			}
		}


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

			if (AbstractState::isState<StateUpdateRetryCounter>(pNextState))
			{
				mRetryCounterUpdated = true;
			}

			mChangePinContext->setStateApproved();
		}

	private Q_SLOTS:
		void initTestCase()
		{
			QSignalSpy eidCardDetected(this, &test_ChangePinController::fireEidCardInserted);

			const auto readerManager = Env::getSingleton<ReaderManager>();
			connect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &test_ChangePinController::onCardInfoChanged);
			connect(readerManager, &ReaderManager::fireCardInserted, this, &test_ChangePinController::onCardInserted);
			readerManager->init();
		}


		void cleanupTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->shutdown();
			disconnect(readerManager, &ReaderManager::fireCardInserted, this, &test_ChangePinController::onCardInserted);
			disconnect(readerManager, &ReaderManager::fireReaderPropertiesUpdated, this, &test_ChangePinController::onCardInfoChanged);
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
			disconnect(mChangePinContext.data(), &WorkflowContext::fireStateChanged, this, &test_ChangePinController::onStateChanged);

			mChangePinController.reset();
			mChangePinContext.reset();
		}


		void testSuccess()
		{
			qDebug() << "START: testSuccess";

			QSignalSpy controllerFinishedSpy(mChangePinController.data(), &ChangePinController::fireComplete);

			mChangePinController->run();

			QVERIFY(controllerFinishedSpy.wait());

			QCOMPARE(mChangePinContext->getStatus().getStatusCode(), GlobalStatus::Code::No_Error);
		}

	Q_SIGNALS:
		void fireEidCardInserted();
};

QTEST_GUILESS_MAIN(test_ChangePinController)
#include "test_ChangePinController.moc"
