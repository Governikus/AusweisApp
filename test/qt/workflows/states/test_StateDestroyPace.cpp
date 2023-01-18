/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateDestroyPace.h"

#include "MockCardConnectionWorker.h"
#include "context/ChangePinContext.h"

#include <QtTest>


using namespace governikus;

class MockCardCommand
	: public BaseCardCommand
{
	Q_OBJECT

	public:
		explicit MockCardCommand(const QSharedPointer<MockCardConnectionWorker>& pCardConnectionWorker)
			: BaseCardCommand(pCardConnectionWorker)
		{
		}


		~MockCardCommand() override = default;

		void internalExecute() override
		{
		}


};

class test_StateDestroyPace
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_Run()
		{
			QThread workerThread;
			workerThread.start();

			{
				const QSharedPointer<ChangePinContext> context(new ChangePinContext());
				const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
				worker->moveToThread(&workerThread);
				const QSharedPointer<CardConnection> connection(new CardConnection(worker));
				context->setCardConnection(connection);
				StateDestroyPace state(context);

				state.run();
				QCOMPARE(state.mConnections.size(), 1);
			}

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnDestroyPace()
		{
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QSharedPointer<MockCardCommand> command(new MockCardCommand(worker));
			const QSharedPointer<ChangePinContext> context(new ChangePinContext());
			StateDestroyPace state(context);
			QSignalSpy spy(&state, &StateDestroyPace::fireContinue);

			state.onDestroyPaceDone(command);
			QCOMPARE(spy.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateDestroyPace)
#include "test_StateDestroyPace.moc"
