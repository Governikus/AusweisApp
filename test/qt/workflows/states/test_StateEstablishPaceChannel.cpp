/**
 * Copyright (c) 2018-2026 Governikus Service GmbH, Germany
 */

#include "states/StateEstablishPaceChannel.h"

#include "AppSettings.h"
#include "ReaderManager.h"
#include "VolatileSettings.h"

#include "MockCardConnectionWorker.h"
#include "TestAuthContext.h"
#include "TestHookThread.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
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


		void setMockPaceOutput(const EstablishPaceChannelOutput& pPaceOutput)
		{
			mPaceOutput = pPaceOutput;
			setReturnCode(pPaceOutput.getReturnCode());
		}


};


Q_DECLARE_METATYPE(std::optional<FailureCode>)


class test_StateEstablishPaceChannel
	: public QObject
{
	Q_OBJECT
	QScopedPointer<TestHookThread> mWorkerThread;
	QSharedPointer<AuthContext> mAuthContext;
	QSharedPointer<StateEstablishPaceChannel> mState;

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<AppSettings>()->getGeneralSettings().setEnableCanAllowed(true);
			Env::getSingleton<ReaderManager>(); // just init in MainThread because of QObject
			Env::getSingleton<VolatileSettings>(); // just init in MainThread because of QObject
		}


		void init()
		{
			mWorkerThread.reset(new TestHookThread());
			mAuthContext.reset(new TestAuthContext(":/paos/DIDAuthenticateEAC1.xml"_L1));
			mState.reset(StateBuilder::createState<StateEstablishPaceChannel>(mAuthContext));
		}


		void cleanup()
		{
			mState.clear();
			mAuthContext.clear();
			mWorkerThread.reset();
		}


		void cleanupTestCase()
		{
			Env::getSingleton<AppSettings>()->getGeneralSettings().setEnableCanAllowed(false);
		}


		void test_Run_NoConnection()
		{
			mAuthContext->setEstablishPaceChannelType(PacePasswordId::PACE_PIN);
			QSignalSpy spyNoCardConnection(mState.data(), &StateEstablishPaceChannel::fireNoCardConnection);

			QTest::ignoreMessage(QtDebugMsg, "No card connection available");
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

			auto worker = MockCardConnectionWorker::create(mWorkerThread.data());
			const QString password("0000000"_L1);

			mAuthContext->setPin(password);
			mAuthContext->setCardConnection(QSharedPointer<CardConnection>::create(worker));
			mAuthContext->setEstablishPaceChannelType(PacePasswordId::PACE_PIN);

			mAuthContext->setProgress(initialProgress, QString());
			QCOMPARE(mAuthContext->getProgressValue(), initialProgress);
			QCOMPARE(mAuthContext->getProgressMessage(), QString());

			QSignalSpy spy(mState.data(), &StateEstablishPaceChannel::firePaceChannelFailed);
			QTest::ignoreMessage(QtDebugMsg, "Establish connection using PACE_PIN");
			mState->run();
			QCOMPARE(mAuthContext->getEstablishPaceChannelType(), PacePasswordId::PACE_PIN);
			QCOMPARE(mAuthContext->getProgressValue(), initialProgress);
			QCOMPARE(mAuthContext->getProgressMessage(), tr("The secure channel is opened"));
			QTRY_COMPARE(spy.size(), 1);

			mAuthContext->resetCardConnection();
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
			auto worker = MockCardConnectionWorker::create(mWorkerThread.data());
			mAuthContext->setCardConnection(QSharedPointer<CardConnection>::create(worker));

			const CardInfo cInfo(CardType::NONE, FileRef(), QSharedPointer<EFCardAccess>(), 3, false, false);
			ReaderInfo rInfo;
			rInfo.setCardInfo(cInfo);
			Q_EMIT worker->fireReaderInfoChanged(rInfo);

			QSignalSpy spyAbort(mState.data(), &AbstractState::fireAbort);

			QTest::ignoreMessage(QtInfoMsg, "Cancellation by user in \"StateEstablishPaceChannel\"");
			mState->onUserCancelled();
			QCOMPARE(mAuthContext->getStatus().getStatusCode(), GlobalStatus::Code::Workflow_Cancellation_By_User);
			QCOMPARE(mAuthContext->getPaceOutput().getReturnCode(), CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(mAuthContext->getFailureCode(), FailureCode::Reason::User_Cancelled);

			mAuthContext->resetCardConnection();
		}


		void test_OnEstablishConnectionDone_data()
		{
			QTest::addColumn<PacePasswordId>("password");
			QTest::addColumn<int>("retryCounter");
			QTest::addColumn<CardReturnCode>("code");
			QTest::addColumn<PaceResult>("result");
			QTest::addColumn<bool>("authentication");
			QTest::addColumn<std::optional<FailureCode>>("failureCode");

			QTest::newRow("PIN_OK") << PacePasswordId::PACE_PIN << 3 << CardReturnCode::OK << PaceResult::OK_PIN << false << std::optional<FailureCode>();
			QTest::newRow("PIN_OK_AUTH") << PacePasswordId::PACE_PIN << 3 << CardReturnCode::OK << PaceResult::OK_PIN_AUTH << true << std::optional<FailureCode>();
			QTest::newRow("PIN_CANCELLATION_BY_USER") << PacePasswordId::PACE_PIN << 2 << CardReturnCode::CANCELLATION_BY_USER << PaceResult::UNDEFINED << false << std::optional<FailureCode>(FailureCode::Reason::Establish_Pace_Channel_User_Cancelled);
			QTest::newRow("PIN_INVALID_RC3") << PacePasswordId::PACE_PIN << 3 << CardReturnCode::OK << PaceResult::INVALID_PIN_1 << false << std::optional<FailureCode>();
			QTest::newRow("PIN_INVALID_RC2") << PacePasswordId::PACE_PIN << 2 << CardReturnCode::OK << PaceResult::INVALID_PIN_2 << false << std::optional<FailureCode>();
			QTest::newRow("PIN_INVALID_RC1") << PacePasswordId::PACE_PIN << 1 << CardReturnCode::OK << PaceResult::INVALID_PIN_3 << false << std::optional<FailureCode>();
			QTest::newRow("CAN_OK") << PacePasswordId::PACE_CAN << 1 << CardReturnCode::OK << PaceResult::OK_CAN << false << std::optional<FailureCode>();
			QTest::newRow("CAN_OK_AUTH") << PacePasswordId::PACE_CAN << 3 << CardReturnCode::OK << PaceResult::OK_CAN_AUTH << true << std::optional<FailureCode>();
			QTest::newRow("CAN_CANCELLATION_BY_USER") << PacePasswordId::PACE_CAN << 1 << CardReturnCode::CANCELLATION_BY_USER << PaceResult::UNDEFINED << true << std::optional<FailureCode>(FailureCode::Reason::Establish_Pace_Channel_User_Cancelled);
			QTest::newRow("PUK_OK") << PacePasswordId::PACE_PUK << 0 << CardReturnCode::OK << PaceResult::OK_PUK << false << std::optional<FailureCode>();
			QTest::newRow("PUK_INVALID_PIN_RETRY_COUNTER") << PacePasswordId::PACE_PUK << 1 << CardReturnCode::PIN_NOT_BLOCKED << PaceResult::UNDEFINED << false << std::optional<FailureCode>();
			QTest::newRow("MRZ") << PacePasswordId::PACE_MRZ << 3 << CardReturnCode::OK << PaceResult::UNDEFINED << false << std::optional<FailureCode>(FailureCode::Reason::Establish_Pace_Channel_Unknown_Password_Id);
			QTest::newRow("UNKNOWN") << PacePasswordId::UNKNOWN << 3 << CardReturnCode::OK << PaceResult::UNDEFINED << false << std::optional<FailureCode>(FailureCode::Reason::Establish_Pace_Channel_Unknown_Password_Id);
		}


		void test_OnEstablishConnectionDone()
		{
			QFETCH(PacePasswordId, password);
			QFETCH(int, retryCounter);
			QFETCH(CardReturnCode, code);
			QFETCH(PaceResult, result);
			QFETCH(bool, authentication);
			QFETCH(std::optional<FailureCode>, failureCode);

			EstablishPaceChannelOutput output(password, code);
			switch (result)
			{
				case PaceResult::UNDEFINED:
					break;

				case PaceResult::OK_PIN:
				case PaceResult::OK_PIN_AUTH:
				case PaceResult::OK_CAN:
				case PaceResult::OK_CAN_AUTH:
				case PaceResult::OK_PUK:
					break;

				case PaceResult::INVALID_PIN_1:
					output.setErrorCode(EstablishPaceChannelErrorCode::GeneralAuthenticateStep4_RC2);
					break;

				case PaceResult::INVALID_PIN_2:
					output.setErrorCode(EstablishPaceChannelErrorCode::GeneralAuthenticateStep4_RC1);
					break;

				case PaceResult::INVALID_PIN_3:
					output.setErrorCode(EstablishPaceChannelErrorCode::GeneralAuthenticateStep4_RC0);
					break;

				case PaceResult::INVALID_CAN:
				case PaceResult::INVALID_PUK:
					output.setErrorCode(EstablishPaceChannelErrorCode::GeneralAuthenticateStep4);
					break;
			}
			output.setStatusMseSetAt(QByteArray::fromHex("9000"));
			if (password == PacePasswordId::PACE_PIN)
			{
				switch (retryCounter)
				{
					case 0:
						output.setStatusMseSetAt(QByteArray::fromHex("63c0"));
						break;

					case 1:
						output.setStatusMseSetAt(QByteArray::fromHex("63c1"));
						break;

					case 2:
						output.setStatusMseSetAt(QByteArray::fromHex("63c2"));
						break;

					default:
						break;
				}
			}
			if (authentication)
			{
				output.setCarCurr(QByteArray("test"));
			}

			QSignalSpy spyWrongPassword(mState.data(), &StateEstablishPaceChannel::fireWrongPassword);
			QSignalSpy spyThirdPinAttemptFailed(mState.data(), &StateEstablishPaceChannel::fireThirdPinAttemptFailed);
			QSignalSpy spyPaceChannelFailed(mState.data(), &StateEstablishPaceChannel::firePaceChannelFailed);
			QSignalSpy spyPaceCanEstablished(mState.data(), &StateEstablishPaceChannel::firePaceCanEstablished);
			QSignalSpy spyPacePukEstablished(mState.data(), &StateEstablishPaceChannel::firePacePukEstablished);
			QSignalSpy spyAbort(mState.data(), &StateEstablishPaceChannel::fireAbort);
			QSignalSpy spyContinue(mState.data(), &StateEstablishPaceChannel::fireContinue);

			auto worker = MockCardConnectionWorker::create(mWorkerThread.data());
			mAuthContext->setCardConnection(QSharedPointer<CardConnection>::create(worker));
			QSharedPointer<MockEstablishPaceChannelCommand> command(new MockEstablishPaceChannelCommand(worker, password));

			const CardInfo cInfo(CardType::NONE, FileRef(), QSharedPointer<EFCardAccess>(), retryCounter, false, false);
			ReaderInfo rInfo;
			rInfo.setCardInfo(cInfo);
			Q_EMIT worker->fireReaderInfoChanged(rInfo);

			if (authentication)
			{
				*mAuthContext->getAccessRightManager() += AccessRight::CAN_ALLOWED;
			}
			else
			{
				*mAuthContext->getAccessRightManager() -= AccessRight::CAN_ALLOWED;
			}
			command->setMockPaceOutput(output);

			if (output.isOk() && password == PacePasswordId::PACE_PIN)
			{
				QTest::ignoreMessage(QtDebugMsg, "PACE_PIN succeeded. Setting expected retry counter to: 3");
				mState->onEstablishConnectionDone(command);
				QCOMPARE(mAuthContext->getPaceOutput().getReturnCode(), code);
				QCOMPARE(mAuthContext->getPaceOutput().getPaceResult(), result);
				QCOMPARE(mAuthContext->getExpectedRetryCounter(), 3);
				QCOMPARE(spyContinue.count(), 1);
				return;
			}

			if (output.isOk() && password == PacePasswordId::PACE_CAN)
			{
				mState->onEstablishConnectionDone(command);
				QCOMPARE(mAuthContext->getPaceOutput().getReturnCode(), code);
				QCOMPARE(mAuthContext->getPaceOutput().getPaceResult(), result);

				if (authentication)
				{
					QCOMPARE(spyContinue.count(), 1);
					return;
				}

				QCOMPARE(spyPaceCanEstablished.count(), 1);
				return;
			}

			if (output.isOk() && password == PacePasswordId::PACE_PUK)
			{
				QTest::ignoreMessage(QtDebugMsg, "PACE_PUK succeeded");
				mState->onEstablishConnectionDone(command);
				QCOMPARE(mAuthContext->getPaceOutput().getReturnCode(), code);
				QCOMPARE(mAuthContext->getPaceOutput().getPaceResult(), result);
				QCOMPARE(mAuthContext->getExpectedRetryCounter(), -1);
				QCOMPARE(spyWrongPassword.count(), 0);
				QCOMPARE(spyThirdPinAttemptFailed.count(), 0);
				QCOMPARE(spyPacePukEstablished.count(), 1);
				return;
			}

			mState->onEstablishConnectionDone(command);

			if (code == CardReturnCode::CANCELLATION_BY_USER)
			{
				QCOMPARE(mAuthContext->getStatus().getStatusCode(), GlobalStatus::Code::Card_Cancellation_By_User);
			}

			QCOMPARE(mAuthContext->getPaceOutput().getReturnCode(), code);
			QCOMPARE(mAuthContext->getPaceOutput().getPaceResult(), result);
			if (result == PaceResult::INVALID_PIN_1
					|| result == PaceResult::INVALID_PIN_2
					|| result == PaceResult::INVALID_PIN_3)
			{
				QCOMPARE(spyAbort.count(), 0);
				QCOMPARE(spyWrongPassword.count(), result != PaceResult::INVALID_PIN_3 ? 1 : 0);
				QCOMPARE(spyThirdPinAttemptFailed.count(), result == PaceResult::INVALID_PIN_3 ? 1 : 0);
			}
			else if (password == PacePasswordId::PACE_PUK)
			{
				QCOMPARE(spyAbort.count(), 0);
				QCOMPARE(spyWrongPassword.count(), 0);
				QCOMPARE(spyPaceChannelFailed.count(), 1);
				QCOMPARE(spyThirdPinAttemptFailed.count(), 0);
			}
			else
			{
				QCOMPARE(spyAbort.count(), 1);
			}
			QCOMPARE(mAuthContext->getFailureCode(), failureCode);

			command.reset();
			mAuthContext->resetCardConnection();
		}


};

QTEST_GUILESS_MAIN(test_StateEstablishPaceChannel)
#include "test_StateEstablishPaceChannel.moc"
