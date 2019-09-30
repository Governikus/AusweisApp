/*!
 * \brief Unit tests for \ref StateVerifyRetryCounter
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateVerifyRetryCounter.h"

#include "states/StateBuilder.h"

#include "MockCardConnectionWorker.h"

#include <QByteArrayList>
#include <QtTest>


using namespace governikus;

class test_StateVerifyRetryCounter
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateVerifyRetryCounter> mState;
	QSharedPointer<WorkflowContext> mContext;
	QThread mWorkerThread;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new WorkflowContext());
			mState.reset(StateBuilder::createState<StateVerifyRetryCounter>(mContext));
			mState->onEntry(nullptr);
			mWorkerThread.start();
		}


		void cleanup()
		{
			mWorkerThread.quit();
			mWorkerThread.wait();
			mState.clear();
			mContext.clear();
		}


		void test_Run()
		{
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&mWorkerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3);
			const ReaderInfo readerInfo(QString(), ReaderManagerPlugInType::UNKNOWN, cardInfo);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);
			mContext->setCardConnection(connection);
			mContext->setExpectedRetryCounter(3);
			QSignalSpy spyContinue(mState.data(), &StateVerifyRetryCounter::fireContinue);

			QTest::ignoreMessage(QtDebugMsg, "Retry counter | actual: 3 / expected: 3");
			mContext->setStateApproved();
			QCOMPARE(spyContinue.count(), 1);
		}


		void test_Run_NoConnection()
		{
			QSignalSpy spyAbort(mState.data(), &StateVerifyRetryCounter::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "Card connection lost.");
			mContext->setStateApproved();
			QCOMPARE(spyAbort.count(), 1);
		}


		void test_Run_NotExpectedReader()
		{
			const QString password("000000");
			mContext->setPin(password);
			mContext->setCan(password);
			mContext->setPuk(password);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&mWorkerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 2);
			const ReaderInfo readerInfo(QString(), ReaderManagerPlugInType::UNKNOWN, cardInfo);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);
			mContext->setCardConnection(connection);
			mContext->setExpectedRetryCounter(2);
			mContext->setReaderName(QString("test reader"));
			QSignalSpy spyContinue(mState.data(), &StateVerifyRetryCounter::fireContinue);

			QTest::ignoreMessage(QtDebugMsg, "Retry counter | actual: 2 / expected: 2");
			QTest::ignoreMessage(QtDebugMsg, "The reader changed or the connected card has an unexpected retry counter. Clearing PACE passwords.");
			mContext->setStateApproved();
			QCOMPARE(mContext->getPin(), QString());
			QCOMPARE(mContext->getCan(), QString());
			QCOMPARE(mContext->getPuk(), QString());
			QVERIFY(mContext->isExpectedReader());
			QCOMPARE(mContext->getExpectedRetryCounter(), 2);
			QCOMPARE(spyContinue.count(), 1);
		}


		void test_Run_ActualNotEqualExpectedRetryCounter()
		{
			const QString password("000000");
			mContext->setPin(password);
			mContext->setCan(password);
			mContext->setPuk(password);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&mWorkerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 2);
			const ReaderInfo readerInfo(QString(), ReaderManagerPlugInType::UNKNOWN, cardInfo);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);
			mContext->setCardConnection(connection);
			QSignalSpy spyContinue(mState.data(), &StateVerifyRetryCounter::fireContinue);

			QTest::ignoreMessage(QtDebugMsg, "Retry counter | actual: 2 / expected: -1");
			QTest::ignoreMessage(QtDebugMsg, "The reader changed or the connected card has an unexpected retry counter. Clearing PACE passwords.");
			mContext->setStateApproved();
			QCOMPARE(mContext->getPin(), QString());
			QCOMPARE(mContext->getCan(), QString());
			QCOMPARE(mContext->getPuk(), QString());
			QVERIFY(mContext->isExpectedReader());
			QCOMPARE(mContext->getExpectedRetryCounter(), 2);
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateVerifyRetryCounter)
#include "test_StateVerifyRetryCounter.moc"
