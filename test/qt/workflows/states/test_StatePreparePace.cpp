/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref StatePreparePace
 */

#include "states/StatePreparePace.h"

#include "states/StateBuilder.h"

#include "MockCardConnectionWorker.h"
#include "TestWorkflowContext.h"

#include <QByteArrayList>
#include <QtTest>


using namespace governikus;

class test_StatePreparePace
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StatePreparePace> mState;
	QSharedPointer<TestWorkflowContext> mContext;
	QThread workerThread;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new TestWorkflowContext());
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
			QSignalSpy spyNoCardConnection(mState.data(), &StatePreparePace::fireNoCardConnection);

			QTest::ignoreMessage(QtDebugMsg, "Card connection lost.");
			mContext->setStateApproved();
			QTRY_COMPARE(spyNoCardConnection.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mContext->getEstablishPaceChannelType(), PacePasswordId::UNKNOWN);
			QVERIFY(!mContext->getFailureCode().has_value());
		}


		void test_Run_CanAllowed()
		{
			QSignalSpy spy(mState.data(), &StatePreparePace::fireEnterPacePassword);

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			mContext->setCardConnection(connection);
			mContext->setCanAllowedMode(true);
			QTest::ignoreMessage(QtDebugMsg, "CAN allowed required");
			mContext->setStateApproved();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mContext->getEstablishPaceChannelType(), PacePasswordId::PACE_CAN);
		}


		void test_Run_RetryCounter0_Smart()
		{
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cardInfo(CardType::SMART_EID, QSharedPointer<const EFCardAccess>(), 0);
			const ReaderInfo readerInfo(QString(), ReaderManagerPlugInType::REMOTE_IFD, cardInfo);
			mContext->setCardConnection(connection);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);
			QSignalSpy spyAbort(mState.data(), &StatePreparePace::fireAbort);

			mContext->setStateApproved();
			QTRY_COMPARE(spyAbort.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mContext->getStatus().getStatusCode(), GlobalStatus::Code::Card_Smart_Invalid);
			QCOMPARE(mContext->getFailureCode(), FailureCode::Reason::Prepace_Pace_Smart_Eid_Invalidated);
		}


		void test_Run_RetryCounter0_data()
		{
			QTest::addColumn<ReaderManagerPlugInType>("type");

			const auto& readerTypes = Enum<ReaderManagerPlugInType>::getList();
			for (const auto& type : readerTypes)
			{
				QTest::newRow(getEnumName(type).data()) << type;
			}
		}


		void test_Run_RetryCounter0()
		{
			QFETCH(ReaderManagerPlugInType, type);

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 0);
			const ReaderInfo readerInfo(QString(), type, cardInfo);
			mContext->setCardConnection(connection);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);
			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePace::fireEnterPacePassword);
			QSignalSpy spyEstablishPaceChannel(mState.data(), &StatePreparePace::fireEstablishPaceChannel);

			QTest::ignoreMessage(QtDebugMsg, "PUK required");
			mContext->setStateApproved();
			QTRY_COMPARE(spyEnterPacePassword.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mContext->getEstablishPaceChannelType(), PacePasswordId::PACE_PUK);

			mContext->setStateApproved(false);

			const QString puk("0987654321");
			mContext->setPuk(puk);
			mContext->setStateApproved();
			QTRY_COMPARE(spyEstablishPaceChannel.count(), 1); // clazy:exclude=qstring-allocations
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
			QTest::ignoreMessage(QtDebugMsg, "PACE_CAN done: false");
			mContext->setStateApproved();
			QTRY_COMPARE(spyEnterPacePassword.count(), 1); // clazy:exclude=qstring-allocations

			mContext->setStateApproved(false);

			const QString can("000000");
			mContext->setCan(can);
			QTest::ignoreMessage(QtDebugMsg, "CAN required");
			QTest::ignoreMessage(QtDebugMsg, "PACE_CAN done: false");
			mContext->setStateApproved();
			QTRY_COMPARE(spyEstablishPaceChannel.count(), 1); // clazy:exclude=qstring-allocations
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
			QTest::ignoreMessage(QtDebugMsg, "PACE_PIN done: false");
			mContext->setStateApproved();
			QTRY_COMPARE(spyEnterPacePassword.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyEstablishPaceChannel.count(), 0);
			QCOMPARE(mContext->getEstablishPaceChannelType(), PacePasswordId::PACE_PIN);

			mContext->setStateApproved(false);

			const QString pin("000000");
			mContext->setPin(pin);
			mContext->setStateApproved();
			QCOMPARE(spyEnterPacePassword.count(), 1);
			QTRY_COMPARE(spyEstablishPaceChannel.count(), 1); // clazy:exclude=qstring-allocations
		}


};

QTEST_GUILESS_MAIN(test_StatePreparePace)
#include "test_StatePreparePace.moc"
