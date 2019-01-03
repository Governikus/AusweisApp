/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateEstablishPaceChannel.h"

#include "context/ChangePinContext.h"
#include "MockCardConnectionWorker.h"
#include "TestAuthContext.h"
#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

class MockEstablishPaceChannelCommand
	: public EstablishPaceChannelCommand
{
	Q_OBJECT

	public:
		explicit MockEstablishPaceChannelCommand(const QSharedPointer<MockCardConnectionWorker>& pCardConnectionWorker, PacePasswordId pPacePasswordId)
			: EstablishPaceChannelCommand(pCardConnectionWorker, pPacePasswordId, QString(), QByteArray(), QByteArray())
		{
		}


		void setReturnCode(CardReturnCode pCode)
		{
			mReturnCode = pCode;
		}


};

class MockUnblockPinCommand
	: public UnblockPinCommand
{
	Q_OBJECT

	public:
		explicit MockUnblockPinCommand(const QSharedPointer<MockCardConnectionWorker>& pCardConnectionWorker)
			: UnblockPinCommand(pCardConnectionWorker, QString())
		{
		}


		void setReturnCode(CardReturnCode pCode)
		{
			mReturnCode = pCode;
		}


};


class test_StateEstablishPaceChannel
	: public QObject
{
	Q_OBJECT
	QThread mWorkerThread;
	QSharedPointer<AuthContext> mAuthContext;

	private Q_SLOTS:
		void init()
		{
			mWorkerThread.start();
			mAuthContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));
		}


		void cleanup()
		{
			mAuthContext.clear();
			mWorkerThread.quit();
			mWorkerThread.wait();
		}


		void test_OnUserCancelled()
		{
			StateEstablishPaceChannel state(mAuthContext);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&mWorkerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cInfo(CardType::NONE, QSharedPointer<EFCardAccess>(), 3, false, false);
			ReaderInfo rInfo;
			rInfo.setCardInfo(cInfo);
			connection->mReaderInfo = rInfo;
			mAuthContext->setCardConnection(connection);

			QSignalSpy spyAbort(&state, &AbstractState::fireAbort);

			QTest::ignoreMessage(QtInfoMsg, "Cancellation by user");
			state.onUserCancelled();
			QCOMPARE(mAuthContext->getStatus().getStatusCode(), GlobalStatus::Code::Workflow_Cancellation_By_User);
			QCOMPARE(mAuthContext->getLastPaceResult(), CardReturnCode::CANCELLATION_BY_USER);
		}


		void test_OnEstablishConnectionDone_Pin()
		{
			StateEstablishPaceChannel state(mAuthContext);
			state.mPasswordId = PacePasswordId::PACE_PIN;
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QSharedPointer<MockEstablishPaceChannelCommand> command(new MockEstablishPaceChannelCommand(worker, PacePasswordId::PACE_PIN));
			worker->moveToThread(&mWorkerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cInfo(CardType::NONE, QSharedPointer<EFCardAccess>(), 3, false, false);
			ReaderInfo rInfo;
			rInfo.setCardInfo(cInfo);
			connection->mReaderInfo = rInfo;
			mAuthContext->setCardConnection(connection);

			QSignalSpy spyContinue(&state, &StateEstablishPaceChannel::fireContinue);
			QSignalSpy spyPaceChannelEstablished(&state, &StateEstablishPaceChannel::firePaceChannelEstablished);
			QSignalSpy spyAbort(&state, &StateEstablishPaceChannel::fireAbort);

			command->setReturnCode(CardReturnCode::OK);
			state.onEstablishConnectionDone(command);
			QCOMPARE(mAuthContext->getLastPaceResult(), CardReturnCode::OK);
			QCOMPARE(spyPaceChannelEstablished.count(), 1);

			mAuthContext->setStatus(GlobalStatus::Code::No_Error);
			command->setReturnCode(CardReturnCode::CANCELLATION_BY_USER);
			state.onEstablishConnectionDone(command);
			QVERIFY(mAuthContext->getCardConnection());
			QCOMPARE(mAuthContext->getLastPaceResult(), CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(mAuthContext->getStatus().getStatusCode(), GlobalStatus::Code::Card_Cancellation_By_User);
			QCOMPARE(spyAbort.count(), 1);

			command->setReturnCode(CardReturnCode::INVALID_PIN);
			mAuthContext->setCardConnection(connection);
			state.onEstablishConnectionDone(command);
			QCOMPARE(mAuthContext->getLastPaceResult(), CardReturnCode::INVALID_PIN);
			QCOMPARE(spyAbort.count(), 2);

			QCOMPARE(spyContinue.count(), 0);
		}


		void test_OnEstablishConnectionDone_Can()
		{
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&mWorkerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cardInfo(CardType::NONE, QSharedPointer<EFCardAccess>(), 2, false, false);
			const ReaderInfo readerInfo(QString(), ReaderManagerPlugInType::UNKNOWN, cardInfo);
			connection->onReaderInfoChanged(readerInfo);
			const QSharedPointer<AuthContext> context(new AuthContext(nullptr));
			context->setCardConnection(connection);
			StateEstablishPaceChannel state(context);
			state.mPasswordId = PacePasswordId::PACE_CAN;
			QSharedPointer<MockEstablishPaceChannelCommand> command(new MockEstablishPaceChannelCommand(worker, PacePasswordId::PACE_CAN));

			QSignalSpy spyContinue(&state, &StateEstablishPaceChannel::fireContinue);
			QSignalSpy spyAbort(&state, &StateEstablishPaceChannel::fireAbort);

			command->setReturnCode(CardReturnCode::OK);
			state.onEstablishConnectionDone(command);
			QCOMPARE(context->getLastPaceResult(), CardReturnCode::OK);
			QCOMPARE(spyContinue.count(), 1);

			command->setReturnCode(CardReturnCode::CANCELLATION_BY_USER);
			state.onEstablishConnectionDone(command);
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Card_Cancellation_By_User);
			QCOMPARE(context->getLastPaceResult(), CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(spyAbort.count(), 1);

			command->setReturnCode(CardReturnCode::INVALID_CAN);
			state.onEstablishConnectionDone(command);
			QCOMPARE(context->getLastPaceResult(), CardReturnCode::INVALID_CAN);
			QCOMPARE(spyAbort.count(), 2);
		}


		void test_OnEstablishConnectionDone_Puk()
		{
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&mWorkerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cInfo(CardType::UNKNOWN, QSharedPointer<const EFCardAccess>(), 3, false, false);
			const ReaderInfo rInfo(QString(), ReaderManagerPlugInType::UNKNOWN, cInfo);
			connection->onReaderInfoChanged(rInfo);
			const QSharedPointer<ChangePinContext> context(new ChangePinContext());
			context->setCardConnection(connection);
			const QSharedPointer<MockUnblockPinCommand> command(new MockUnblockPinCommand(worker));

			StateEstablishPaceChannel state(context);
			state.mPasswordId = PacePasswordId::PACE_PUK;
			QSignalSpy spyContinue(&state, &StateEstablishPaceChannel::fireContinue);
			QSignalSpy spyPacePukEstablished(&state, &StateEstablishPaceChannel::firePacePukEstablished);
			QSignalSpy spyAbort(&state, &StateEstablishPaceChannel::fireAbort);

			command->setReturnCode(CardReturnCode::OK);
			state.onEstablishConnectionDone(command);
			QCOMPARE(context->getLastPaceResult(), CardReturnCode::OK_PUK);
			QCOMPARE(context->getSuccessMessage(), tr("PIN successfully unblocked"));
			QCOMPARE(spyPacePukEstablished.count(), 1);

			command->setReturnCode(CardReturnCode::CANCELLATION_BY_USER);
			state.onEstablishConnectionDone(command);
			QCOMPARE(context->getLastPaceResult(), CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Card_Cancellation_By_User);
			QCOMPARE(spyAbort.count(), 1);

			command->setReturnCode(CardReturnCode::INVALID_PUK);
			state.onEstablishConnectionDone(command);
			QCOMPARE(context->getLastPaceResult(), CardReturnCode::INVALID_PUK);
			QCOMPARE(spyAbort.count(), 2);

			context->setStatus(GlobalStatus::Code::No_Error);
			command->setReturnCode(CardReturnCode::PUK_INOPERATIVE);
			state.onEstablishConnectionDone(command);
			QCOMPARE(context->getLastPaceResult(), CardReturnCode::PUK_INOPERATIVE);
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Card_Puk_Blocked);
			QCOMPARE(spyAbort.count(), 3);

			context->setStatus(GlobalStatus::Code::No_Error);
			command->setReturnCode(CardReturnCode::UNKNOWN);
			state.onEstablishConnectionDone(command);
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::No_Error);
			QCOMPARE(spyAbort.count(), 4);

			QCOMPARE(spyContinue.count(), 0);
		}


};

QTEST_GUILESS_MAIN(test_StateEstablishPaceChannel)
#include "test_StateEstablishPaceChannel.moc"
