/*!
 * \brief Unit tests for \ref StateChangePin
 *
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateChangePin.h"

#include "MockCardConnectionWorker.h"

#include <QtTest>

using namespace governikus;

class MockEstablishPaceChannelCommand
	: public BaseCardCommand
{
	Q_OBJECT

	public:
		explicit MockEstablishPaceChannelCommand(const QSharedPointer<MockCardConnectionWorker>& pWorker)
			: BaseCardCommand(pWorker)
		{
		}


		~MockEstablishPaceChannelCommand() override = default;

		void internalExecute() override
		{
		}


		void setReturnCode(CardReturnCode pCode)
		{
			mReturnCode = pCode;
		}


};

class test_StateChangePin
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_Run()
		{
			QThread workerThread;
			workerThread.start();

			{
				const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
				worker->moveToThread(&workerThread);
				const QSharedPointer<CardConnection> connection(new CardConnection(worker));
				const QSharedPointer<ChangePinContext> context(new ChangePinContext());
				context->setCardConnection(connection);
				StateChangePin state(context);

				QTest::ignoreMessage(QtDebugMsg, "Invoke set Eid PIN command");
				state.run();
				QCOMPARE(state.mConnections.size(), 1);
			}

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnSetEidPinDone()
		{
			const QSharedPointer<ChangePinContext> context(new ChangePinContext());
			StateChangePin state(context);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QSharedPointer<MockEstablishPaceChannelCommand> command(new MockEstablishPaceChannelCommand(worker));

			QSignalSpy spyContinue(&state, &StateChangePin::fireContinue);
			QSignalSpy spyAbort(&state, &StateChangePin::fireAbort);
			QSignalSpy spyInvalidPin(&state, &StateChangePin::fireInvalidPin);

			command->setReturnCode(CardReturnCode::OK);
			state.onSetEidPinDone(command);
			QCOMPARE(context->getSuccessMessage(), tr("You have successfully changed your PIN."));
			QCOMPARE(spyContinue.count(), 1);

			command->setReturnCode(CardReturnCode::CANCELLATION_BY_USER);
			state.onSetEidPinDone(command);
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Card_Cancellation_By_User);
			QCOMPARE(spyAbort.count(), 1);

			context->setStatus(GlobalStatus::Code::No_Error);

			command->setReturnCode(CardReturnCode::NEW_PIN_MISMATCH);
			state.onSetEidPinDone(command);
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Card_NewPin_Mismatch);
			QCOMPARE(spyInvalidPin.count(), 1);

			context->setStatus(GlobalStatus::Code::No_Error);

			command->setReturnCode(CardReturnCode::PIN_BLOCKED);
			state.onSetEidPinDone(command);
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::No_Error);
			QCOMPARE(spyAbort.count(), 2);
		}


};

QTEST_GUILESS_MAIN(test_StateChangePin)
#include "test_StateChangePin.moc"
