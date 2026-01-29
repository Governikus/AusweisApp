/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateTransmit.h"

#include "MockCardConnectionWorker.h"
#include "TestHookThread.h"

#include <QByteArrayList>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class MockTransmitCommand
	: public TransmitCommand
{
	Q_OBJECT

	public:
		MockTransmitCommand(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker, const QList<InputAPDUInfo>& info, const QString& slot)
			: TransmitCommand(pCardConnectionWorker, info, slot)
		{
		}


		void internalExecute() override;
		~MockTransmitCommand() override = default;

		void setMockReturnCode(CardReturnCode pReturnCode)
		{
			setReturnCode(pReturnCode);
		}


};

void MockTransmitCommand::internalExecute()
{
}


class test_StateTransmit
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_Run()
		{
			TestHookThread workerThread;

			{
				auto worker = MockCardConnectionWorker::create(&workerThread);
				const QSharedPointer<AuthContext> context(new AuthContext());
				context->setCardConnection(QSharedPointer<CardConnection>::create(worker));
				StateTransmit stateTransmit(context);

				const QString slotHandle("slot"_L1);
				const InputAPDUInfo info;
				const QSharedPointer<Transmit> transmit(new Transmit());
				transmit->setSlotHandle(slotHandle);
				transmit->appendInputApduInfo(info);
				QByteArrayList outputApdus;
				const QByteArray output("output");
				outputApdus.insert(0, output);
				const QSharedPointer<TransmitResponse> response(new TransmitResponse());
				response->setOutputApdus(outputApdus);
				context->setTransmit(transmit);
				context->setTransmitResponse(response);

				QSignalSpy spyAbort(&stateTransmit, &StateTransmit::fireAbort);
				QTest::ignoreMessage(QtWarningMsg, "Transmit unsuccessful. Return code: Unknown_Error | \"An unexpected error has occurred during processing.\"");
				stateTransmit.run();
				QCOMPARE(stateTransmit.mConnections.size(), 1);
				QTRY_COMPARE(spyAbort.count(), 1); // clazy:exclude=qstring-allocations

				context->resetCardConnection();
			}
		}


		void test_OnCardCommandDone()
		{
			const QSharedPointer<AuthContext> context(new AuthContext());
			const QSharedPointer<TransmitResponse> response(new TransmitResponse());
			context->setTransmitResponse(response);
			StateTransmit stateTransmit(context);
			const auto& worker = MockCardConnectionWorker::create();
			QList<InputAPDUInfo> vector(5);
			const InputAPDUInfo info(QByteArray("info"));
			vector.insert(0, info);
			const QSharedPointer<MockTransmitCommand> command(new MockTransmitCommand(worker, vector, QString()));
			QSignalSpy spyContinue(&stateTransmit, &StateTransmit::fireContinue);
			QSignalSpy spyAbort(&stateTransmit, &StateTransmit::fireAbort);

			command->setMockReturnCode(CardReturnCode::OK);
			stateTransmit.onCardCommandDone(command);
			QCOMPARE(spyContinue.count(), 1);

			command->setMockReturnCode(CardReturnCode::UNEXPECTED_TRANSMIT_STATUS);
			stateTransmit.onCardCommandDone(command);
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Card_Unexpected_Transmit_Status);
			QCOMPARE(spyContinue.count(), 2);

			command->setMockReturnCode(CardReturnCode::UNKNOWN);
			stateTransmit.onCardCommandDone(command);
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(context->getFailureCode(), FailureCode::Reason::Transmit_Card_Command_Failed);
		}


};

QTEST_GUILESS_MAIN(test_StateTransmit)
#include "test_StateTransmit.moc"
