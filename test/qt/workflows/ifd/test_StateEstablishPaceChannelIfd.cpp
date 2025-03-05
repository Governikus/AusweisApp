/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateEstablishPaceChannelIfd.h"

#include "context/AuthContext.h"

#include "MockCardConnectionWorker.h"
#include "MockIfdServer.h"
#include "RemoteIfdServer.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class MockEstablishPaceChannelCommand
	: public EstablishPaceChannelCommand
{
	Q_OBJECT

	public:
		explicit MockEstablishPaceChannelCommand(const QSharedPointer<MockCardConnectionWorker>& pCardConnectionWorker, PacePasswordId pPacePasswordId = PacePasswordId::PACE_PIN)
			: EstablishPaceChannelCommand(pCardConnectionWorker, pPacePasswordId, QByteArray("0000"), QByteArray(), QByteArray())
		{
			setPaceReturnCode(CardReturnCode::OK);
		}


		~MockEstablishPaceChannelCommand() override = default;

		void setPaceReturnCode(CardReturnCode pCode)
		{
			mPaceOutput = EstablishPaceChannelOutput(pCode);
			setReturnCode(mPaceOutput.getPaceReturnCode());
		}


};


class test_StateEstablishPaceChannelIfd
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateEstablishPaceChannelIfd> mState;
	QSharedPointer<IfdServiceContext> mContext;
	QSharedPointer<MockCardConnectionWorker> mWorker;
	QThread mThread;

	private Q_SLOTS:
		void init()
		{
			mThread.start();
			mContext.reset(new IfdServiceContext(QSharedPointer<IfdServer>(new RemoteIfdServer())));
			mState.reset(new StateEstablishPaceChannelIfd(mContext));
			mWorker.reset(new MockCardConnectionWorker());
		}


		void cleanup()
		{
			mState.clear();
			mContext.clear();
			mWorker.clear();
			mThread.quit();
			mThread.wait();
		}


		void test_RunNoCardConnection()
		{
			const QSharedPointer<IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel("SlotHandle"_L1, EstablishPaceChannel(), 6));
			mContext->setEstablishPaceChannel(message);
			QSignalSpy spy(mState.data(), &StateEstablishPaceChannelIfd::fireContinue);

			mState->run();
			QCOMPARE(spy.count(), 1);
		}


		void test_Run()
		{
			EstablishPaceChannel establishPaceChannel(PacePasswordId::PACE_PIN);
			const QSharedPointer<IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel("SlotHandle"_L1, establishPaceChannel, 6));
			mContext->setEstablishPaceChannel(message);
			mWorker->moveToThread(&mThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(mWorker));
			mContext->setCardConnection(connection);
			mContext->setPin("0000"_L1);

			mState->run();
			QCOMPARE(mState->mConnections.size(), 1);
		}


		void test_OnReaderInfoChanged()
		{
			ReaderInfo info;
			QSignalSpy spy(mState.data(), &StateEstablishPaceChannelIfd::fireContinue);

			mState->onReaderInfoChanged(info);
			QCOMPARE(mContext->getEstablishPaceChannelOutput().getPaceReturnCode(), CardReturnCode::CARD_NOT_FOUND);
			QCOMPARE(spy.count(), 1);
		}


		void test_OnEstablishConnectionDoneEstablishPaceChannelCommand()
		{
			QSignalSpy spy(mState.data(), &StateEstablishPaceChannelIfd::fireContinue);
			const QSharedPointer<MockEstablishPaceChannelCommand> command(new MockEstablishPaceChannelCommand(mWorker));
			mState->mPasswordId = PacePasswordId::PACE_PIN;

			mContext->setExpectedRetryCounter(2);
			QTest::ignoreMessage(QtDebugMsg, "Correct PACE password. Expected retry counter is now 3");
			mState->onEstablishConnectionDone(command);
			QCOMPARE(mContext->getExpectedRetryCounter(), 3);

			mContext->setExpectedRetryCounter(2);
			command->setPaceReturnCode(CardReturnCode::INVALID_PIN);
			QTest::ignoreMessage(QtDebugMsg, "Wrong PACE password. Decreasing expected retry counter to 1");
			mState->onEstablishConnectionDone(command);
			QCOMPARE(mContext->getExpectedRetryCounter(), 1);

			QCOMPARE(spy.count(), 2);
		}


		void test_OnEstablishConnectionDoneUnblockPinCommand()
		{
			QSignalSpy spy(mState.data(), &StateEstablishPaceChannelIfd::fireContinue);
			const QSharedPointer<MockEstablishPaceChannelCommand> command(new MockEstablishPaceChannelCommand(mWorker, PacePasswordId::PACE_PUK));
			mState->mPasswordId = PacePasswordId::PACE_PUK;

			command->setPaceReturnCode(CardReturnCode::OK);
			QTest::ignoreMessage(QtDebugMsg, "Resetting PACE passwords and setting expected retry counter to -1");
			mState->onEstablishConnectionDone(command);
			QCOMPARE(mContext->getExpectedRetryCounter(), -1);
			QCOMPARE(mContext->getNewPin(), QString());

			command->setPaceReturnCode(CardReturnCode::INVALID_PUK);
			QTest::ignoreMessage(QtDebugMsg, "Resetting PACE passwords and setting expected retry counter to -1");
			mState->onEstablishConnectionDone(command);
			QCOMPARE(mContext->getExpectedRetryCounter(), -1);
			QCOMPARE(mContext->getNewPin(), QString());

			command->setPaceReturnCode(CardReturnCode::UNDEFINED);
			mState->onEstablishConnectionDone(command);
			QCOMPARE(mContext->getEstablishPaceChannelOutput().getPaceReturnCode(), CardReturnCode::UNDEFINED);

			QCOMPARE(spy.count(), 3);
		}


};

QTEST_GUILESS_MAIN(test_StateEstablishPaceChannelIfd)
#include "test_StateEstablishPaceChannelIfd.moc"
