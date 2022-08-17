/*!
 * \brief Test for the Change PIN functionality.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "controller/ChangePinController.h"

#include "PersoSimController.h"
#include "ReaderManager.h"
#include "context/ChangePinContext.h"
#include "states/AbstractState.h"
#include "states/StateUpdateRetryCounter.h"

#include <QSharedPointer>
#include <QtTest>


#ifdef PERSOSIM_TESTS_ENABLED
Q_IMPORT_PLUGIN(PcscReaderManagerPlugIn)
#endif


using namespace governikus;


class test_ChangePinController
	: public QObject
{
	Q_OBJECT

	private:
		QScopedPointer<PersoSimController> mPersoSim;
		QSharedPointer<ChangePinContext> mChangePinContext;
		QScopedPointer<WorkflowController> mChangePinController;
		bool mRetryCounterUpdated = false;


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
			mPersoSim.reset(new PersoSimController());
			if (!mPersoSim->isEnabled())
			{
				QSKIP("PersoSim tests not enabled");
			}

			QVERIFY(mPersoSim->init());

			QSignalSpy eidCardDetected(this, &test_ChangePinController::fireEidCardInserted);

			const auto readerManager = Env::getSingleton<ReaderManager>();
			connect(readerManager, &ReaderManager::fireCardInserted, this, &test_ChangePinController::onCardInserted);
			readerManager->init();

			QVERIFY(eidCardDetected.wait(20000));
		}


		void cleanupTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->shutdown();
			disconnect(readerManager, &ReaderManager::fireCardInserted, this, &test_ChangePinController::onCardInserted);

			QVERIFY(mPersoSim->shutdown());
			mPersoSim.reset();
		}


		void init()
		{
			mChangePinContext.reset(new ChangePinContext());
			mChangePinContext->setReaderPlugInTypes({ReaderManagerPlugInType::PCSC});
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

			mChangePinContext->setPin("123456");
			mChangePinContext->setNewPin("123456");

			QSignalSpy controllerFinishedSpy(mChangePinController.data(), &ChangePinController::fireComplete);

			mChangePinController->run();

			QVERIFY(controllerFinishedSpy.wait());

			QCOMPARE(mChangePinContext->getStatus().getStatusCode(), GlobalStatus::Code::No_Error);
		}


		void testWrongPin()
		{
			mChangePinContext->setPin("111111");
			mChangePinContext->setNewPin("111111");

			QSignalSpy controllerFinishedSpy(mChangePinController.data(), &ChangePinController::fireComplete);

			mChangePinController->run();

			QVERIFY(controllerFinishedSpy.wait());

			QCOMPARE(mChangePinContext->getStatus().getStatusCode(), GlobalStatus::Code::Workflow_Cancellation_By_User);
		}

	Q_SIGNALS:
		void fireEidCardInserted();
};

QTEST_GUILESS_MAIN(test_ChangePinController)
#include "test_ChangePinController.moc"
