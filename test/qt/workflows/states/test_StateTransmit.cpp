/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref StateTransmit
 */

#include "states/StateTransmit.h"

#include "MockCardConnectionWorker.h"

#include <QByteArrayList>
#include <QtTest>


using namespace governikus;

class MockTransmitCommand
	: public TransmitCommand
{
	Q_OBJECT

	public:
		MockTransmitCommand(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker, const QVector<InputAPDUInfo>& info, const QString& slot)
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
			QThread workerThread;
			workerThread.start();

			{
				const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
				worker->moveToThread(&workerThread);
				const QSharedPointer<CardConnection> connection(new CardConnection(worker));
				const QSharedPointer<AuthContext> context(new AuthContext(nullptr));
				context->setCardConnection(connection);
				StateTransmit stateTransmit(context);

				const QString slotHandle("slot");
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

				stateTransmit.run();
				QCOMPARE(stateTransmit.mConnections.size(), 1);
			}

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnCardCommandDone()
		{
			const QSharedPointer<AuthContext> context(new AuthContext(nullptr));
			const QSharedPointer<TransmitResponse> response(new TransmitResponse());
			context->setTransmitResponse(response);
			StateTransmit stateTransmit(context);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			QVector<InputAPDUInfo> vector(5);
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
