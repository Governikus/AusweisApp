/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateEstablishPaceChannel.h"

#include "AppSettings.h"
#include "MockCardConnectionWorker.h"
#include "TestAuthContext.h"
#include "TestFileHelper.h"
#include "context/ChangePinContext.h"

#include <QtTest>

using namespace governikus;

class MockEstablishPaceChannelCommand
	: public EstablishPaceChannelCommand
{
	Q_OBJECT

	public:
		explicit MockEstablishPaceChannelCommand(const QSharedPointer<MockCardConnectionWorker>& pCardConnectionWorker, PacePasswordId pPacePasswordId)
			: EstablishPaceChannelCommand(pCardConnectionWorker, pPacePasswordId, QByteArray(), QByteArray(), QByteArray())
		{
		}


		void setMockReturnCode(CardReturnCode pReturnCode)
		{
			setReturnCode(pReturnCode);
		}


};

Q_DECLARE_METATYPE(std::optional<FailureCode>)

class test_StateEstablishPaceChannel
	: public QObject
{
	Q_OBJECT
	QThread mWorkerThread;
	QSharedPointer<AuthContext> mAuthContext;
	QSharedPointer<StateEstablishPaceChannel> mState;

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<AppSettings>()->getGeneralSettings().setEnableCanAllowed(true);
		}


		void cleanupTestCase()
		{
			Env::getSingleton<AppSettings>()->getGeneralSettings().setEnableCanAllowed(false);
		}


		void init()
		{
			mWorkerThread.start();
			mAuthContext.reset(new TestAuthContext(":/paos/DIDAuthenticateEAC1.xml"));
			mState.reset(StateBuilder::createState<StateEstablishPaceChannel>(mAuthContext));
		}


		void cleanup()
		{
			mState.clear();
			mAuthContext.clear();
			mWorkerThread.quit();
			mWorkerThread.wait();
		}


		void test_Run_NoConnection()
		{
			mAuthContext->setEstablishPaceChannelType(PacePasswordId::PACE_PIN);
			QSignalSpy spyNoCardConnection(mState.data(), &StateEstablishPaceChannel::fireNoCardConnection);

			QTest::ignoreMessage(QtDebugMsg, "No card connection available.");
			mState->run();
			QCOMPARE(spyNoCardConnection.count(), 1);
			QVERIFY(!mAuthContext->getFailureCode().has_value());
		}


		void test_Run_data()
		{
			QTest::addColumn<int>("initialProgress");

			QTest::newRow("0") << 0;
			QTest::newRow("42") << 42;
			QTest::newRow("50") << 42;
			QTest::newRow("90") << 90;
			QTest::newRow("100") << 100;
		}


		void test_Run()
		{
			QFETCH(int, initialProgress);

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&mWorkerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const QString password("0000000");

			mAuthContext->setPin(password);
			mAuthContext->setCardConnection(connection);
			mAuthContext->setEstablishPaceChannelType(PacePasswordId::PACE_PIN);

			mAuthContext->setProgress(initialProgress, QString());
			QCOMPARE(mAuthContext->getProgressValue(), initialProgress);
			QCOMPARE(mAuthContext->getProgressMessage(), QString());

			QTest::ignoreMessage(QtDebugMsg, "Establish connection using PACE_PIN");
			mState->run();
			QCOMPARE(mAuthContext->getEstablishPaceChannelType(), PacePasswordId::PACE_PIN);
			QCOMPARE(mState->mPasswordId, PacePasswordId::PACE_PIN);
			QCOMPARE(mAuthContext->getProgressValue(), initialProgress);
			QCOMPARE(mAuthContext->getProgressMessage(), tr("The secure channel is opened"));
		}


		void test_OnContextError()
		{
			mAuthContext->setStatus(GlobalStatus::Code::Card_Cancellation_By_User);
			mAuthContext->setFailureCode(FailureCode::Reason::User_Cancelled);
			QSignalSpy spyPaceChannelFailed(mState.data(), &StateEstablishPaceChannel::firePaceChannelFailed);

			mState->run();

			QCOMPARE(spyPaceChannelFailed.count(), 1);
		}


		void test_OnKillWorkflow()
		{
			QSignalSpy spyAbort(mState.data(), &AbstractState::fireAbort);
			QSignalSpy spyPaceChannelFailed(mState.data(), &StateEstablishPaceChannel::firePaceChannelFailed);
			mState->onEntry(nullptr);
			QCOMPARE(spyAbort.count(), 0);
			QCOMPARE(spyPaceChannelFailed.count(), 0);

			mAuthContext->killWorkflow();

			QTRY_COMPARE(spyAbort.count(), 1); // clazy:exclude=qstring-allocations
			QTRY_COMPARE(spyPaceChannelFailed.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_OnUserCancelled()
		{
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&mWorkerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cInfo(CardType::NONE, QSharedPointer<EFCardAccess>(), 3, false, false);
			ReaderInfo rInfo;
			rInfo.setCardInfo(cInfo);
			Q_EMIT worker->fireReaderInfoChanged(rInfo);
			mAuthContext->setCardConnection(connection);

			QSignalSpy spyAbort(mState.data(), &AbstractState::fireAbort);

			QTest::ignoreMessage(QtInfoMsg, "Cancellation by user");
			mState->onUserCancelled();
			QCOMPARE(mAuthContext->getStatus().getStatusCode(), GlobalStatus::Code::Workflow_Cancellation_By_User);
			QCOMPARE(mAuthContext->getLastPaceResult(), CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::User_Cancelled);
		}


		void test_OnEstablishConnectionDone_data()
		{
			QTest::addColumn<PacePasswordId>("password");
			QTest::addColumn<int>("retryCounter");
			QTest::addColumn<CardReturnCode>("code");
			QTest::addColumn<CardReturnCode>("result");
			QTest::addColumn<bool>("canAllowed");
			QTest::addColumn<std::optional<FailureCode>>("failureCode");

			QTest::newRow("PIN_OK") << PacePasswordId::PACE_PIN << 3 << CardReturnCode::OK << CardReturnCode::OK << false << std::optional<FailureCode>();
			QTest::newRow("PIN_PUK_INOPERATIVE") << PacePasswordId::PACE_PIN << 1 << CardReturnCode::PUK_INOPERATIVE << CardReturnCode::PUK_INOPERATIVE << false << std::optional<FailureCode>(FailureCode::Reason::Establish_Pace_Channel_Puk_Inoperative);
			QTest::newRow("PIN_CANCELLATION_BY_USER") << PacePasswordId::PACE_PIN << 2 << CardReturnCode::CANCELLATION_BY_USER << CardReturnCode::CANCELLATION_BY_USER << false << std::optional<FailureCode>(FailureCode::Reason::Establish_Pace_Channel_User_Cancelled);
			QTest::newRow("PIN_INVALID_PIN_RETRY_COUNTER_3") << PacePasswordId::PACE_PIN << 3 << CardReturnCode::INVALID_PIN << CardReturnCode::INVALID_PIN << false << std::optional<FailureCode>();
			QTest::newRow("PIN_INVALID_PIN_RETRY_COUNTER_2") << PacePasswordId::PACE_PIN << 2 << CardReturnCode::INVALID_PIN << CardReturnCode::INVALID_PIN_2 << false << std::optional<FailureCode>();
			QTest::newRow("PIN_INVALID_PIN_RETRY_COUNTER_1") << PacePasswordId::PACE_PIN << 1 << CardReturnCode::INVALID_PIN << CardReturnCode::INVALID_PIN_3 << false << std::optional<FailureCode>();
			QTest::newRow("CAN_OK_CAN_ALLOWED") << PacePasswordId::PACE_CAN << 3 << CardReturnCode::OK << CardReturnCode::OK << true << std::optional<FailureCode>();
			QTest::newRow("CAN_OK") << PacePasswordId::PACE_CAN << 2 << CardReturnCode::OK << CardReturnCode::OK << false << std::optional<FailureCode>();
			QTest::newRow("CAN_CANCELLATION_BY_USER") << PacePasswordId::PACE_CAN << 2 << CardReturnCode::CANCELLATION_BY_USER << CardReturnCode::CANCELLATION_BY_USER << true << std::optional<FailureCode>(FailureCode::Reason::Establish_Pace_Channel_User_Cancelled);
			QTest::newRow("PUK_OK") << PacePasswordId::PACE_PUK << 0 << CardReturnCode::OK << CardReturnCode::OK_PUK << false << std::optional<FailureCode>();
			QTest::newRow("PUK_INVALID_PIN_RETRY_COUNTER_1") << PacePasswordId::PACE_PUK << 0 << CardReturnCode::INVALID_PIN << CardReturnCode::INVALID_PIN << false << std::optional<FailureCode>();
		}


		void test_OnEstablishConnectionDone()
		{
			QFETCH(PacePasswordId, password);
			QFETCH(int, retryCounter);
			QFETCH(CardReturnCode, code);
			QFETCH(CardReturnCode, result);
			QFETCH(bool, canAllowed);
			QFETCH(std::optional<FailureCode>, failureCode);

			QSignalSpy spyPaceChannelEstablished(mState.data(), &StateEstablishPaceChannel::firePaceChannelEstablished);
			QSignalSpy spyPaceChannelInoperative(mState.data(), &StateEstablishPaceChannel::firePaceChannelInoperative);
			QSignalSpy spyAbort(mState.data(), &StateEstablishPaceChannel::fireAbort);
			QSignalSpy spyContinue(mState.data(), &StateEstablishPaceChannel::fireContinue);

			mState->mPasswordId = password;
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QSharedPointer<MockEstablishPaceChannelCommand> command(new MockEstablishPaceChannelCommand(worker, PacePasswordId::PACE_PIN));
			worker->moveToThread(&mWorkerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			const CardInfo cInfo(CardType::NONE, QSharedPointer<EFCardAccess>(), retryCounter, false, false);
			ReaderInfo rInfo;
			rInfo.setCardInfo(cInfo);
			Q_EMIT worker->fireReaderInfoChanged(rInfo);
			mAuthContext->setCardConnection(connection);
			if (canAllowed)
			{
				*mAuthContext->getAccessRightManager() += AccessRight::CAN_ALLOWED;
			}
			else
			{
				*mAuthContext->getAccessRightManager() -= AccessRight::CAN_ALLOWED;
			}
			command->setMockReturnCode(code);

			if (code == CardReturnCode::OK && password == PacePasswordId::PACE_PIN)
			{
				QTest::ignoreMessage(QtDebugMsg, "PACE_PIN succeeded. Setting expected retry counter to: 3");
				mState->onEstablishConnectionDone(command);
				QCOMPARE(mAuthContext->getLastPaceResult(), result);
				QCOMPARE(mAuthContext->getExpectedRetryCounter(), 3);
				QCOMPARE(spyPaceChannelEstablished.count(), 1);
				return;
			}

			if (code == CardReturnCode::OK && password == PacePasswordId::PACE_CAN)
			{
				if (!canAllowed)
				{
					mState->onEstablishConnectionDone(command);
					QCOMPARE(spyContinue.count(), 1);
					return;
				}
				mState->onEstablishConnectionDone(command);
				QCOMPARE(mAuthContext->getLastPaceResult(), result);
				QCOMPARE(spyPaceChannelEstablished.count(), 1);
				return;
			}

			if (code == CardReturnCode::OK && password == PacePasswordId::PACE_PUK)
			{
				QTest::ignoreMessage(QtDebugMsg, "PACE_PUK succeeded. Resetting PACE passwords and setting expected retry counter to: -1");
				mState->onEstablishConnectionDone(command);
				QCOMPARE(mAuthContext->getLastPaceResult(), result);
				QCOMPARE(mAuthContext->getExpectedRetryCounter(), -1);
				QCOMPARE(spyPaceChannelInoperative.count(), 1);
				return;
			}

			mState->onEstablishConnectionDone(command);

			if (code == CardReturnCode::PUK_INOPERATIVE)
			{
				QCOMPARE(mAuthContext->getStatus().getStatusCode(), GlobalStatus::Code::Card_Puk_Blocked);
			}

			if (code == CardReturnCode::CANCELLATION_BY_USER)
			{
				QCOMPARE(mAuthContext->getStatus().getStatusCode(), GlobalStatus::Code::Card_Cancellation_By_User);
			}

			QCOMPARE(mAuthContext->getLastPaceResult(), result);
			if (password == PacePasswordId::PACE_PUK
					|| mAuthContext->getLastPaceResult() == CardReturnCode::INVALID_PIN
					|| mAuthContext->getLastPaceResult() == CardReturnCode::INVALID_PIN_2
					|| mAuthContext->getLastPaceResult() == CardReturnCode::INVALID_PIN_3)
			{
				QCOMPARE(spyAbort.count(), 0);
				QCOMPARE(spyPaceChannelInoperative.count(), 1);

			}
			else
			{
				QCOMPARE(spyAbort.count(), 1);
			}
			QCOMPARE(mAuthContext->getFailureCode(), failureCode);
		}


};

QTEST_GUILESS_MAIN(test_StateEstablishPaceChannel)
#include "test_StateEstablishPaceChannel.moc"
