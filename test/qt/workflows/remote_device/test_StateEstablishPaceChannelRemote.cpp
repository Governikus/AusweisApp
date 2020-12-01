/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateEstablishPaceChannelRemote.h"

#include "context/AuthContext.h"
#include "EstablishPaceChannelParser.h"

#include "MockCardConnectionWorker.h"
#include "MockRemoteServer.h"

#include <QtTest>


using namespace governikus;

class MockEstablishPaceChannelCommand
	: public EstablishPaceChannelCommand
{
	Q_OBJECT

	public:
		explicit MockEstablishPaceChannelCommand(const QSharedPointer<MockCardConnectionWorker>& pCardConnectionWorker)
			: EstablishPaceChannelCommand(pCardConnectionWorker, PacePasswordId::PACE_PIN, QByteArray("0000"), QByteArray(), QByteArray())
		{
			EstablishPaceChannelOutput output;
			output.setPaceReturnCode(CardReturnCode::OK);
			mPaceOutput = output;
		}


		~MockEstablishPaceChannelCommand() override = default;

		void setPaceReturnCode(CardReturnCode pCode)
		{
			EstablishPaceChannelOutput output;
			output.setPaceReturnCode(pCode);
			mPaceOutput = output;
		}


};

class MockUnblockPinCommand
	: public UnblockPinCommand
{
	Q_OBJECT

	public:
		explicit MockUnblockPinCommand(const QSharedPointer<MockCardConnectionWorker>& pCardConnectionWorker)
			: UnblockPinCommand(pCardConnectionWorker, QByteArray("0000"))
		{
		}


		~MockUnblockPinCommand() override = default;

		void setReturnCode(CardReturnCode pCode)
		{
			mReturnCode = pCode;
		}


};

class test_StateEstablishPaceChannelRemote
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateEstablishPaceChannelRemote> mState;
	QSharedPointer<RemoteServiceContext> mContext;
	QSharedPointer<MockCardConnectionWorker> mWorker;
	QThread mThread;

	private Q_SLOTS:
		void init()
		{
			mThread.start();
			mContext.reset(new RemoteServiceContext());
			mState.reset(new StateEstablishPaceChannelRemote(mContext));
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
			const QSharedPointer<IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel("NFC Reader", "abc"));
			mContext->setEstablishPaceChannelMessage(message);
			QSignalSpy spy(mState.data(), &StateEstablishPaceChannelRemote::fireContinue);

			mState->run();
			QCOMPARE(spy.count(), 1);
		}


		void test_Run()
		{
			const QSharedPointer<IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel("NFC Reader", "input"));
			mContext->setEstablishPaceChannelMessage(message);
			mWorker->moveToThread(&mThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(mWorker));
			mContext->setCardConnection(connection);
			mContext->setPin("0000");

			mState->run();
			QCOMPARE(mState->mConnections.size(), 1);
		}


		void test_OnReaderInfoChanged()
		{
			ReaderInfo info;
			QSignalSpy spy(mState.data(), &StateEstablishPaceChannelRemote::fireContinue);

			mState->onReaderInfoChanged(info);
			QCOMPARE(mContext->getEstablishPaceChannelOutput().getPaceReturnCode(), CardReturnCode::CARD_NOT_FOUND);
			QCOMPARE(spy.count(), 1);
		}


		void test_OnEstablishConnectionDoneEstablishPaceChannelCommand()
		{
			QSignalSpy spy(mState.data(), &StateEstablishPaceChannelRemote::fireContinue);
			const QSharedPointer<MockEstablishPaceChannelCommand> command(new MockEstablishPaceChannelCommand(mWorker));
			mContext->setExpectedRetryCounter(2);

			mState->mPasswordId = PacePasswordId::PACE_PIN;
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
			QSignalSpy spy(mState.data(), &StateEstablishPaceChannelRemote::fireContinue);
			const QSharedPointer<MockUnblockPinCommand> command(new MockUnblockPinCommand(mWorker));

			command->setReturnCode(CardReturnCode::OK);
			QTest::ignoreMessage(QtDebugMsg, "Resetting PACE passwords and setting expected retry counter to -1");
			mState->onEstablishConnectionDone(command);
			QCOMPARE(mContext->getExpectedRetryCounter(), -1);
			QCOMPARE(mContext->getNewPin(), QString());

			command->setReturnCode(CardReturnCode::INVALID_PUK);
			QTest::ignoreMessage(QtDebugMsg, "Resetting PACE passwords and setting expected retry counter to -1");
			mState->onEstablishConnectionDone(command);
			QCOMPARE(mContext->getExpectedRetryCounter(), -1);
			QCOMPARE(mContext->getNewPin(), QString());

			command->setReturnCode(CardReturnCode::UNDEFINED);
			mState->onEstablishConnectionDone(command);
			QCOMPARE(mContext->getEstablishPaceChannelOutput().getPaceReturnCode(), CardReturnCode::UNDEFINED);

			QCOMPARE(spy.count(), 3);
		}


};

QTEST_GUILESS_MAIN(test_StateEstablishPaceChannelRemote)
#include "test_StateEstablishPaceChannelRemote.moc"
