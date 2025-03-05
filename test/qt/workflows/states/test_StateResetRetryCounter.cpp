/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateResetRetryCounter.h"

#include "FailureCode.h"
#include "MockCardConnection.h"
#include "MockCardConnectionWorker.h"
#include "TestWorkflowContext.h"

#include <QSharedPointer>
#include <QThread>
#include <QtTest>

#include <optional>

using namespace governikus;


class MockCardCommand
	: public BaseCardCommand
{
	Q_OBJECT

	public:
		explicit MockCardCommand(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
			: BaseCardCommand(pCardConnectionWorker)
		{
		}


		void setMockReturnCode(CardReturnCode pReturnCode)
		{
			setReturnCode(pReturnCode);
		}


		void internalExecute() override
		{
			setReturnCode(getCardConnectionWorker()->updateRetryCounter());
		}


};

class test_StateResetRetryCounter
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_RunNoCardConnection()
		{
			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			StateResetRetryCounter state(context);

			QSignalSpy spyContinue(&state, &StateResetRetryCounter::fireContinue);
			QSignalSpy spyAbort(&state, &StateResetRetryCounter::fireAbort);
			QSignalSpy spyNoCardConnection(&state, &StateResetRetryCounter::fireNoCardConnection);

			QTest::ignoreMessage(QtDebugMsg, "No card connection available.");
			state.run();
			QCOMPARE(spyContinue.count(), 0);
			QCOMPARE(spyAbort.count(), 0);
			QCOMPARE(spyNoCardConnection.count(), 1);
			QVERIFY(!context->getFailureCode().has_value());
		}


		void test_ResetRetryCounterDone_data()
		{
			QTest::addColumn<CardReturnCode>("returnCode");
			QTest::addColumn<std::optional<FailureCode>>("failureCode");

			QTest::addRow("OK") << CardReturnCode::OK << std::optional<FailureCode>();
			QTest::addRow("PUK inoperative") << CardReturnCode::PUK_INOPERATIVE << std::optional<FailureCode>(FailureCode::Reason::Establish_Pace_Channel_Puk_Inoperative);
			QTest::addRow("Error case") << CardReturnCode::INVALID_PUK << std::optional<FailureCode>();
		}


		void test_ResetRetryCounterDone()
		{
			QFETCH(CardReturnCode, returnCode);
			QFETCH(std::optional<FailureCode>, failureCode);

			const QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			StateResetRetryCounter state(context);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QSharedPointer<MockCardCommand> command(new MockCardCommand(worker));
			QSignalSpy spyContinue(&state, &StateResetRetryCounter::fireContinue);
			QSignalSpy spyAbort(&state, &StateResetRetryCounter::fireAbort);
			QSignalSpy spyNoCardConnection(&state, &StateResetRetryCounter::fireNoCardConnection);
			command->setMockReturnCode(returnCode);
			context->setCardConnection(QSharedPointer<MockCardConnection>::create());
			state.onResetRetryCounterDone(command);
			if (returnCode == CardReturnCode::OK || returnCode == CardReturnCode::PUK_INOPERATIVE)
			{
				QCOMPARE(spyNoCardConnection.count(), 0);
				QCOMPARE(spyContinue.count(), failureCode.has_value() ? 0 : 1);
				QCOMPARE(spyAbort.count(), failureCode.has_value() ? 1 : 0);
			}
			else
			{
				QCOMPARE(spyNoCardConnection.count(), 1);
				QCOMPARE(spyContinue.count(), 0);
				QCOMPARE(spyAbort.count(), 0);
			}
			QCOMPARE(context->getFailureCode(), failureCode);
		}


};

QTEST_GUILESS_MAIN(test_StateResetRetryCounter)
#include "test_StateResetRetryCounter.moc"
