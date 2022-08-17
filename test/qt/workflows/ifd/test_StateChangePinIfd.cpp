/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateChangePinIfd.h"

#include "context/IfdServiceContext.h"

#include "MockCardConnectionWorker.h"
#include "MockIfdServer.h"

#include <QtTest>


using namespace governikus;

class MockSetEidPinCommand
	: public SetEidPinCommand
{
	Q_OBJECT

	public:
		MockSetEidPinCommand(const QSharedPointer<MockCardConnectionWorker>& pWorker,
				const QByteArray& pNewPin)
			: SetEidPinCommand(pWorker, pNewPin, '0')
		{
			mResponseApdu = ResponseApdu(StatusCode::SUCCESS);
		}


		~MockSetEidPinCommand() override = default;

		void internalExecute() override
		{
		}


};


class test_StateChangePinIfd
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_Run()
		{
			QThread workerThread;
			workerThread.start();

			{
				const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(QSharedPointer<IfdServer>(new MockIfdServer())));
				StateChangePinIfd state(context);
				const QString slotHandle = QStringLiteral("slot");
				const QByteArray input("input");
				const QSharedPointer<const IfdModifyPin> message(new IfdModifyPin(slotHandle, input));
				context->setModifyPinMessage(message);

				QSignalSpy spyContinue(&state, &StateChangePinIfd::fireContinue);
				state.run();
				QCOMPARE(context->getModifyPinMessageResponseApdu().getStatusCode(), StatusCode::UNKNOWN);
				QCOMPARE(spyContinue.count(), 1);

				const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
				worker->moveToThread(&workerThread);
				const QSharedPointer<CardConnection> connection(new CardConnection(worker));
				context->setCardConnection(connection);
				state.run();
				QCOMPARE(state.mConnections.size(), 1);
			}

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnChangePinDone()
		{
			const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(QSharedPointer<IfdServer>(new MockIfdServer())));
			StateChangePinIfd state(context);

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QByteArray pin("000000");
			const QSharedPointer<MockSetEidPinCommand> setEidCommand(new MockSetEidPinCommand(worker, pin));
			QSignalSpy spyContinue(&state, &StateChangePinIfd::fireContinue);

			state.onChangePinDone(setEidCommand);
			QCOMPARE(context->getModifyPinMessageResponseApdu().getStatusCode(), StatusCode::SUCCESS);
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateChangePinIfd)
#include "test_StateChangePinIfd.moc"
