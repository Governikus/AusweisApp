/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref StateVerifyRetryCounter
 */

#include "states/StateVerifyRetryCounter.h"

#include "states/StateBuilder.h"

#include "MockCardConnectionWorker.h"
#include "TestWorkflowContext.h"

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
			mContext.reset(new TestWorkflowContext());
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
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_Run_NoConnection()
		{
			QSignalSpy spyNoCardConnection(mState.data(), &StateVerifyRetryCounter::fireNoCardConnection);

			QTest::ignoreMessage(QtDebugMsg, "Card connection lost.");
			mContext->setStateApproved();
			QTRY_COMPARE(spyNoCardConnection.count(), 1); // clazy:exclude=qstring-allocations
			QVERIFY(!mContext->getFailureCode().has_value());
		}


		void test_Run_NoReader()
		{
			const QString password("000000");
			mContext->setPin(password);
			mContext->setCan(password);
			mContext->setPuk(password);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&mWorkerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 2);
			const ReaderInfo readerInfo(QString("test reader"), ReaderManagerPlugInType::UNKNOWN, cardInfo);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);
			mContext->setCardConnection(connection);
			mContext->setReaderName(QString("test reader"));
			QSignalSpy spyContinue(mState.data(), &StateVerifyRetryCounter::fireContinue);

			QTest::ignoreMessage(QtDebugMsg, "Retry counter | actual: 2 / expected: -1");
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mContext->getPin(), password);
			QCOMPARE(mContext->getCan(), password);
			QCOMPARE(mContext->getPuk(), password);
			QVERIFY(mContext->isExpectedReader());
			QCOMPARE(mContext->getExpectedRetryCounter(), 2);
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
			const ReaderInfo readerInfo(QString("test reader"), ReaderManagerPlugInType::UNKNOWN, cardInfo);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);
			mContext->setCardConnection(connection);
			mContext->rememberReader();
			mContext->setReaderName(QString("other reader"));
			QSignalSpy spyContinue(mState.data(), &StateVerifyRetryCounter::fireContinue);

			QTest::ignoreMessage(QtDebugMsg, "Retry counter | actual: 2 / expected: 2");
			QTest::ignoreMessage(QtDebugMsg, "The reader changed or the connected card has an unexpected retry counter. Clearing PACE passwords.");
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mContext->getPin(), QString());
			QCOMPARE(mContext->getCan(), QString());
			QCOMPARE(mContext->getPuk(), QString());
			QVERIFY(!mContext->isExpectedReader());
			QCOMPARE(mContext->getExpectedRetryCounter(), 2);
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
			const ReaderInfo readerInfo(QString("test reader"), ReaderManagerPlugInType::UNKNOWN, cardInfo);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);
			mContext->setCardConnection(connection);
			mContext->rememberReader();
			mContext->setReaderName(QString("test reader"));
			mContext->setExpectedRetryCounter(1);
			QSignalSpy spyContinue(mState.data(), &StateVerifyRetryCounter::fireContinue);

			QTest::ignoreMessage(QtDebugMsg, "Retry counter | actual: 2 / expected: 1");
			QTest::ignoreMessage(QtDebugMsg, "The reader changed or the connected card has an unexpected retry counter. Clearing PACE passwords.");
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mContext->getPin(), QString());
			QCOMPARE(mContext->getCan(), QString());
			QCOMPARE(mContext->getPuk(), QString());
			QVERIFY(mContext->isExpectedReader());
			QCOMPARE(mContext->getExpectedRetryCounter(), 2);
		}


};

QTEST_GUILESS_MAIN(test_StateVerifyRetryCounter)
#include "test_StateVerifyRetryCounter.moc"
