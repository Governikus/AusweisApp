/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateMaintainCardConnection.h"

#include "MockCardConnectionWorker.h"
#include "TestWorkflowContext.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_StateMaintainCardConnection
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateMaintainCardConnection> mState;
	QSharedPointer<WorkflowContext> mContext;
	QThread workerThread;

	private Q_SLOTS:
		void init()
		{
			workerThread.start();
			mContext.reset(new TestWorkflowContext());
			mState.reset(new StateMaintainCardConnection(mContext));
		}


		void cleanup()
		{
			mContext.clear();
			mState.reset();
			workerThread.quit();
			workerThread.wait();
		}


		void test_Run_ResultUnrecoverable_data()
		{
			QTest::addColumn<CardReturnCode>("code");
			QTest::addColumn<GlobalStatus::Code>("status");

			QTest::newRow("cancellation_by_user") << CardReturnCode::CANCELLATION_BY_USER << GlobalStatus::Code::Card_Cancellation_By_User;
			QTest::newRow("puk_inoperative") << CardReturnCode::PUK_INOPERATIVE << GlobalStatus::Code::Card_Puk_Blocked;
			QTest::newRow("input_timeout") << CardReturnCode::INPUT_TIME_OUT << GlobalStatus::Code::Card_Input_TimeOut;
		}


		void test_Run_ResultUnrecoverable()
		{
			QFETCH(CardReturnCode, code);
			QFETCH(GlobalStatus::Code, status);
			QSignalSpy spy(mState.data(), &StateMaintainCardConnection::fireAbort);

			mContext->setLastPaceResult(code);
			QTest::ignoreMessage(QtDebugMsg, "Last PACE result is unrecoverable. Aborting.");
			mState->run();
			QCOMPARE(mContext->getStatus(), status);
			QCOMPARE(spy.count(), 1);
			QCOMPARE(mContext->getFailureCode(), FailureCode::Reason::Maintain_Card_Connection_Pace_Unrecoverable);
		}


		void test_Run_WrongPacePassword_data()
		{
			QTest::addColumn<CardReturnCode>("code");

			QTest::newRow("invalid_can") << CardReturnCode::INVALID_CAN;
			QTest::newRow("invalid_pin") << CardReturnCode::INVALID_PIN;
			QTest::newRow("invalid_pin_2") << CardReturnCode::INVALID_PIN_2;
			QTest::newRow("invalid_pin_3") << CardReturnCode::INVALID_PIN_3;
			QTest::newRow("invalid_puk") << CardReturnCode::INVALID_PUK;
			QTest::newRow("new_pin_mismatch") << CardReturnCode::NEW_PIN_MISMATCH;
			QTest::newRow("new_pin_invalid_length") << CardReturnCode::NEW_PIN_INVALID_LENGTH;
			QTest::newRow("pin_not_blocked") << CardReturnCode::PIN_NOT_BLOCKED;
			QTest::newRow("pin_blocked") << CardReturnCode::PIN_BLOCKED;
		}


		void test_Run_WrongPacePassword()
		{
			QFETCH(CardReturnCode, code);

			QSignalSpy spyNoCardConnection(mState.data(), &StateMaintainCardConnection::fireNoCardConnection);
			QSignalSpy spyUpdateRetryCounter(mState.data(), &StateMaintainCardConnection::fireForceUpdateRetryCounter);

			mContext->setLastPaceResult(code);

			QTest::ignoreMessage(QtDebugMsg, "No card connection available.");
			mState->run();
			QCOMPARE(spyNoCardConnection.count(), 1);

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			mContext->setCardConnection(connection);

			QTest::ignoreMessage(QtDebugMsg, "Trigger retry counter update.");
			mState->run();
			QCOMPARE(spyUpdateRetryCounter.count(), 1);
		}


		void test_Run_ResetCardConnectionAndLastPaceResult_data()
		{
			QTest::addColumn<CardReturnCode>("code");
			QTest::addColumn<bool>("doReset");

			QTest::newRow("card_not_found") << CardReturnCode::CARD_NOT_FOUND << true;
			QTest::newRow("retry_allowed") << CardReturnCode::RESPONSE_EMPTY << true;

			QTest::newRow("undefined") << CardReturnCode::UNDEFINED << false;
			QTest::newRow("unknown") << CardReturnCode::UNKNOWN << false;
			QTest::newRow("command_failed") << CardReturnCode::COMMAND_FAILED << false;
			QTest::newRow("protocol_error") << CardReturnCode::PROTOCOL_ERROR << false;
			QTest::newRow("unexpectd_transmit_status") << CardReturnCode::UNEXPECTED_TRANSMIT_STATUS << false;
		}


		void test_Run_ResetCardConnectionAndLastPaceResult()
		{
			QFETCH(CardReturnCode, code);
			QFETCH(bool, doReset);
			QSignalSpy spyNoCard(mState.data(), &StateMaintainCardConnection::fireNoCardConnection);
			QSignalSpy spyAbort(mState.data(), &StateMaintainCardConnection::fireAbort);

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			mContext->setCardConnection(connection);
			mContext->setLastPaceResult(code);
			if (doReset)
			{
				QTest::ignoreMessage(QtDebugMsg, "No card connection available.");
			}
			else
			{
				QTest::ignoreMessage(QtDebugMsg, QRegularExpression("Last PACE result: .*"_L1));
				QTest::ignoreMessage(QtDebugMsg, "Last PACE result is unrecoverable. Aborting.");
			}
			mState->run();
			QCOMPARE(mContext->getCardConnection(), doReset ? QSharedPointer<CardConnection>() : connection);
			QCOMPARE(mContext->getLastPaceResult(), doReset ? CardReturnCode::OK : code);
			QCOMPARE(spyNoCard.count(), doReset ? 1 : 0);
			QCOMPARE(spyAbort.count(), doReset ? 0 : 1);
			if (!doReset)
			{
				QCOMPARE(mContext->getFailureCode(), FailureCode::Reason::Maintain_Card_Connection_Pace_Unrecoverable);
			}
		}


		void test_Run_Ok()
		{
			QSignalSpy spyNoCardConnection(mState.data(), &StateMaintainCardConnection::fireNoCardConnection);
			QSignalSpy spyContinue(mState.data(), &StateMaintainCardConnection::fireContinue);
			QSignalSpy spyUpdateRetryCounter(mState.data(), &StateMaintainCardConnection::fireForceUpdateRetryCounter);

			mContext->setLastPaceResult(CardReturnCode::OK);
			QTest::ignoreMessage(QtDebugMsg, "No card connection available.");
			mState->run();
			QCOMPARE(spyNoCardConnection.count(), 1);

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			mContext->setCardConnection(connection);

			QTest::ignoreMessage(QtDebugMsg, "Card connection is fine. Proceeding.");
			mState->run();
			QCOMPARE(spyContinue.count(), 1);

			mContext->setLastPaceResult(CardReturnCode::OK_PUK);
			QTest::ignoreMessage(QtDebugMsg, "PIN unblocked! Triggering retry counter update.");
			mState->run();
			QCOMPARE(spyUpdateRetryCounter.count(), 1);
			QCOMPARE(mContext->getLastPaceResult(), CardReturnCode::OK_PUK);
		}


};

QTEST_GUILESS_MAIN(test_StateMaintainCardConnection)
#include "test_StateMaintainCardConnection.moc"
