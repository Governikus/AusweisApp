/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "states/StatePreparePace.h"

#include "ReaderManager.h"
#include "states/StateBuilder.h"

#include "MockCardConnectionWorker.h"
#include "TestHookThread.h"
#include "TestWorkflowContext.h"

#include <QByteArrayList>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_StatePreparePace
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StatePreparePace> mState;
	QSharedPointer<TestWorkflowContext> mContext;
	QScopedPointer<TestHookThread> mWorkerThread;

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<ReaderManager>(); // just init in MainThread because of QObject
		}


		void init()
		{
			mWorkerThread.reset(new TestHookThread());
			mContext.reset(new TestWorkflowContext());
			mState.reset(StateBuilder::createState<StatePreparePace>(mContext));
			mState->onEntry(nullptr);
		}


		void cleanup()
		{
			mContext.clear();
			mState.clear();
			mWorkerThread.reset();
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

			const auto& worker = MockCardConnectionWorker::create(mWorkerThread.data());
			mContext->setCardConnection(QSharedPointer<CardConnection>::create(worker));
			mContext->setCanAllowedMode(true);
			QTest::ignoreMessage(QtDebugMsg, "CAN allowed required");
			mContext->setStateApproved();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mContext->getEstablishPaceChannelType(), PacePasswordId::PACE_CAN);
		}


		void test_Run_RetryCounter0_Smart()
		{
			const auto& worker = MockCardConnectionWorker::create(mWorkerThread.data());
			mContext->setCardConnection(QSharedPointer<CardConnection>::create(worker));

			const CardInfo cardInfo(CardType::SMART_EID, FileRef(), QSharedPointer<const EFCardAccess>(), 0);
			const ReaderInfo readerInfo(QString(), ReaderManagerPluginType::REMOTE_IFD, cardInfo);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);

			QSignalSpy spyAbort(mState.data(), &StatePreparePace::fireAbort);
			mContext->setStateApproved();
			QTRY_COMPARE(spyAbort.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mContext->getStatus().getStatusCode(), GlobalStatus::Code::Card_Smart_Invalid);
			QCOMPARE(mContext->getFailureCode(), FailureCode::Reason::Prepace_Pace_Smart_Eid_Invalidated);
		}


		void test_Run_RetryCounter0_data()
		{
			QTest::addColumn<ReaderManagerPluginType>("type");

			const auto& readerTypes = Enum<ReaderManagerPluginType>::getList();
			for (const auto& type : readerTypes)
			{
				QTest::newRow(getEnumName(type).data()) << type;
			}
		}


		void test_Run_RetryCounter0()
		{
			QFETCH(ReaderManagerPluginType, type);

			const auto& worker = MockCardConnectionWorker::create(mWorkerThread.data());
			mContext->setCardConnection(QSharedPointer<CardConnection>::create(worker));

			const CardInfo cardInfo(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 0);
			const ReaderInfo readerInfo(QString(), type, cardInfo);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);

			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePace::fireEnterPacePassword);
			QSignalSpy spyContinue(mState.data(), &StatePreparePace::fireContinue);
			QTest::ignoreMessage(QtDebugMsg, "PUK required");
			mContext->setStateApproved();
			QTRY_COMPARE(spyEnterPacePassword.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mContext->getEstablishPaceChannelType(), PacePasswordId::PACE_PUK);

			mContext->setStateApproved(false);

			const QString puk("0987654321"_L1);
			mContext->setPuk(puk);
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_Run_RetryCounter1()
		{
			const auto& worker = MockCardConnectionWorker::create(mWorkerThread.data());
			mContext->setCardConnection(QSharedPointer<CardConnection>::create(worker));

			const CardInfo cardInfo(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 1);
			const ReaderInfo readerInfo(QString(), ReaderManagerPluginType::UNKNOWN, cardInfo);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);

			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePace::fireEnterPacePassword);
			QSignalSpy spyContinue(mState.data(), &StatePreparePace::fireContinue);
			QTest::ignoreMessage(QtDebugMsg, "CAN required");
			QTest::ignoreMessage(QtDebugMsg, "PACE_CAN done: false");
			mContext->setStateApproved();
			QTRY_COMPARE(spyEnterPacePassword.count(), 1); // clazy:exclude=qstring-allocations

			mContext->setStateApproved(false);

			const QString can("000000"_L1);
			mContext->setCan(can);
			QTest::ignoreMessage(QtDebugMsg, "CAN required");
			QTest::ignoreMessage(QtDebugMsg, "PACE_CAN done: false");
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_Run_RetryCounter3()
		{
			const auto& worker = MockCardConnectionWorker::create(mWorkerThread.data());
			mContext->setCardConnection(QSharedPointer<CardConnection>::create(worker));

			const CardInfo cardInfo(CardType::EID_CARD, FileRef(), QSharedPointer<const EFCardAccess>(), 3);
			const ReaderInfo readerInfo(QString(), ReaderManagerPluginType::UNKNOWN, cardInfo);
			Q_EMIT worker->fireReaderInfoChanged(readerInfo);

			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePace::fireEnterPacePassword);
			QSignalSpy spyContinue(mState.data(), &StatePreparePace::fireContinue);
			QTest::ignoreMessage(QtDebugMsg, "PIN allowed");
			QTest::ignoreMessage(QtDebugMsg, "PACE_PIN done: false");
			mContext->setStateApproved();
			QTRY_COMPARE(spyEnterPacePassword.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyContinue.count(), 0);
			QCOMPARE(mContext->getEstablishPaceChannelType(), PacePasswordId::PACE_PIN);

			mContext->setStateApproved(false);

			const QString pin("000000"_L1);
			mContext->setPin(pin);
			mContext->setStateApproved();
			QCOMPARE(spyEnterPacePassword.count(), 1);
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
		}


};

QTEST_GUILESS_MAIN(test_StatePreparePace)
#include "test_StatePreparePace.moc"
