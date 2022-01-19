/*!
 * \brief Unit tests for \ref StateUpdateRetryCounter
 *
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateUpdateRetryCounter.h"

#include "MockCardConnection.h"
#include "MockCardConnectionWorker.h"

#include <QtTest>

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


		void setCode(CardReturnCode pCode)
		{
			mReturnCode = pCode;
		}


		void internalExecute() override
		{
			mReturnCode = mCardConnectionWorker->updateRetryCounter();
		}


};


class test_StateUpdateRetryCounter
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_RunWithoutCardConnection()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			StateUpdateRetryCounter counter(context);
			QSignalSpy spyContinue(&counter, &StateUpdateRetryCounter::fireContinue);
			QSignalSpy spyAbort(&counter, &StateUpdateRetryCounter::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "No card connection available.");
			counter.run();
			QCOMPARE(spyContinue.count(), 0);
			QCOMPARE(spyAbort.count(), 1);
		}


		void test_OnUpdateRetryCounterDone()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			StateUpdateRetryCounter counter(context);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QSharedPointer<MockCardCommand> command(new MockCardCommand(worker));
			QSignalSpy spyContinue(&counter, &StateUpdateRetryCounter::fireContinue);
			QSignalSpy spyAbort(&counter, &StateUpdateRetryCounter::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "StateUpdateRetryCounter::onUpdateRetryCounterDone()");
			QTest::ignoreMessage(QtCriticalMsg, "An error occurred while communicating with the card reader, cannot determine retry counter, abort state");
			counter.onUpdateRetryCounterDone(command);
			QCOMPARE(spyAbort.count(), 1);

			command->setCode(CardReturnCode::OK);
			context->setCardConnection(QSharedPointer<MockCardConnection>::create());
			QTest::ignoreMessage(QtDebugMsg, "StateUpdateRetryCounter::onUpdateRetryCounterDone()");
			counter.onUpdateRetryCounterDone(command);
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateUpdateRetryCounter)
#include "test_StateUpdRetryCounter.moc"
