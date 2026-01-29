/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateDestroyPace.h"

#include "context/ChangePinContext.h"

#include "MockCardConnectionWorker.h"
#include "TestHookThread.h"

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
			TestHookThread workerThread;

			{
				const QSharedPointer<ChangePinContext> context(new ChangePinContext());
				auto worker = MockCardConnectionWorker::create(&workerThread);
				context->setCardConnection(QSharedPointer<CardConnection>::create(worker));

				StateDestroyPace state(context);
				QSignalSpy spy(&state, &StateDestroyPace::fireContinue);
				state.run();
				QCOMPARE(state.mConnections.size(), 1);
				QTRY_COMPARE(spy.size(), 1);

				context->resetCardConnection();
			}
		}


		void test_OnDestroyPace()
		{
			const auto& worker = MockCardConnectionWorker::create();
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
