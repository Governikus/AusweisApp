/*!
 * \brief Unit tests for \ref StatePreparePace
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "states/StatePreparePace.h"

#include "states/StateBuilder.h"

#include "MockCardConnectionWorker.h"

#include <QByteArrayList>
#include <QtTest>


using namespace governikus;

class test_StatePreparePace
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StatePreparePace> mState;
	QSharedPointer<WorkflowContext> mContext;
	QThread workerThread;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new WorkflowContext());
			mState.reset(StateBuilder::createState<StatePreparePace>(mContext));
			workerThread.start();
			mState->onEntry(nullptr);
		}


		void cleanup()
		{
			workerThread.quit();
			workerThread.wait();
			mContext.clear();
			mState.clear();
		}


		void test_Run_NoCardConnection()
		{
			QSignalSpy spyAbort(mState.data(), &StatePreparePace::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "Card connection lost.");
			mContext->setStateApproved();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mContext->getEstablishPaceChannelType(), PacePasswordId::UNKNOWN);
		}


		void test_Run_CanAllowed()
		{
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			mContext->setCardConnection(connection);
			mContext->setCanAllowedMode(true);
			QTest::ignoreMessage(QtDebugMsg, "CAN allowed required");
			mContext->setStateApproved();
			QCOMPARE(mContext->getEstablishPaceChannelType(), PacePasswordId::PACE_CAN);
		}


		void test_Run_RetryCounter0()
		{
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 0);
			const ReaderInfo readerInfo(QString(), ReaderManagerPlugInType::UNKNOWN, cardInfo);
			mContext->setCardConnection(connection);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);
			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePace::fireEnterPacePassword);
			QSignalSpy spyEstablishPaceChannel(mState.data(), &StatePreparePace::fireEstablishPaceChannel);

			QTest::ignoreMessage(QtDebugMsg, "PUK required");
			mContext->setStateApproved();
			QCOMPARE(spyEnterPacePassword.count(), 1);
			QCOMPARE(mContext->getEstablishPaceChannelType(), PacePasswordId::PACE_PUK);

			mContext->setStateApproved(false);

			const QString puk("0987654321");
			mContext->setPuk(puk);
			mContext->setStateApproved();
			QCOMPARE(spyEstablishPaceChannel.count(), 1);
		}


		void test_Run_RetryCounter1()
		{
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 1);
			const ReaderInfo readerInfo(QString(), ReaderManagerPlugInType::UNKNOWN, cardInfo);
			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePace::fireEnterPacePassword);
			QSignalSpy spyEstablishPaceChannel(mState.data(), &StatePreparePace::fireEstablishPaceChannel);
			mContext->setCardConnection(connection);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);

			QTest::ignoreMessage(QtDebugMsg, "CAN required");
			QTest::ignoreMessage(QtDebugMsg, "PACE CAN done: false");
			mContext->setStateApproved();

			mContext->setStateApproved(false);

			const QString can("000000");
			mContext->setCan(can);
			QTest::ignoreMessage(QtDebugMsg, "CAN required");
			QTest::ignoreMessage(QtDebugMsg, "PACE CAN done: false");
			mContext->setStateApproved();
			QCOMPARE(spyEstablishPaceChannel.count(), 1);
		}


		void test_Run_RetryCounter3()
		{
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3);
			const ReaderInfo readerInfo(QString(), ReaderManagerPlugInType::UNKNOWN, cardInfo);
			mContext->setCardConnection(connection);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);
			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePace::fireEnterPacePassword);
			QSignalSpy spyEstablishPaceChannel(mState.data(), &StatePreparePace::fireEstablishPaceChannel);

			QTest::ignoreMessage(QtDebugMsg, "PIN allowed");
			QTest::ignoreMessage(QtDebugMsg, "PACE PIN done: false");
			mContext->setStateApproved();
			QCOMPARE(spyEnterPacePassword.count(), 1);
			QCOMPARE(spyEstablishPaceChannel.count(), 0);
			QCOMPARE(mContext->getEstablishPaceChannelType(), PacePasswordId::PACE_PIN);

			mContext->setStateApproved(false);

			const QString pin("000000");
			mContext->setPin(pin);
			mContext->setStateApproved();
			QCOMPARE(spyEnterPacePassword.count(), 1);
			QCOMPARE(spyEstablishPaceChannel.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StatePreparePace)
#include "test_StatePreparePace.moc"
