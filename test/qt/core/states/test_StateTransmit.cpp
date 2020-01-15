/*!
 * \brief Unit tests for \ref StateTransmit
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
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


		virtual void internalExecute() override;
		virtual ~MockTransmitCommand() override = default;

		void setReturnCode(CardReturnCode code)
		{
			mReturnCode = code;
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
			context->addTransmit(transmit);
			context->addTransmitResponse(response);

			stateTransmit.run();
			QCOMPARE(stateTransmit.mConnections.size(), 1);

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnCardCommandDone()
		{
			const QSharedPointer<AuthContext> context(new AuthContext(nullptr));
			const QSharedPointer<TransmitResponse> response1(new TransmitResponse());
			const QSharedPointer<TransmitResponse> response2(new TransmitResponse());
			const QSharedPointer<TransmitResponse> response3(new TransmitResponse());
			context->addTransmitResponse(response1);
			context->addTransmitResponse(response2);
			context->addTransmitResponse(response3);
			StateTransmit stateTransmit(context);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			QVector<InputAPDUInfo> vector(5);
			const InputAPDUInfo info(QByteArray("info"));
			vector.insert(0, info);
			const QSharedPointer<MockTransmitCommand> command(new MockTransmitCommand(worker, vector, QString()));
			QSignalSpy spyContinue(&stateTransmit, &StateTransmit::fireContinue);
			QSignalSpy spyAbort(&stateTransmit, &StateTransmit::fireAbort);

			command->setReturnCode(CardReturnCode::OK);
			stateTransmit.onCardCommandDone(command);
			QCOMPARE(spyContinue.count(), 1);

			command->setReturnCode(CardReturnCode::UNEXPECTED_TRANSMIT_STATUS);
			stateTransmit.onCardCommandDone(command);
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Card_Unexpected_Transmit_Status);
			QCOMPARE(spyContinue.count(), 2);

			command->setReturnCode(CardReturnCode::UNKNOWN);
			stateTransmit.onCardCommandDone(command);
			QCOMPARE(spyAbort.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateTransmit)
#include "test_StateTransmit.moc"
