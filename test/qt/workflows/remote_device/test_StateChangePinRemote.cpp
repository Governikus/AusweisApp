/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateChangePinRemote.h"

#include "context/RemoteServiceContext.h"

#include "MockCardConnectionWorker.h"
#include "MockRemoteServer.h"

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


class test_StateChangePinRemote
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::setCreator<RemoteServer*>(std::function<RemoteServer* ()>([&] {
					return new MockRemoteServer();
				}));
		}


		void test_Run()
		{
			QThread workerThread;
			workerThread.start();

			{
				const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
				StateChangePinRemote state(context);
				const QString slotHandle = QStringLiteral("slot");
				const QByteArray input("input");
				const QSharedPointer<const IfdModifyPin> message(new IfdModifyPin(slotHandle, input));
				context->setModifyPinMessage(message);

				QSignalSpy spyContinue(&state, &StateChangePinRemote::fireContinue);
				state.run();
				QCOMPARE(context->getModifyPinMessageResponseApdu().getReturnCode(), StatusCode::EMPTY);
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
			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			StateChangePinRemote state(context);

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			const QByteArray pin("000000");
			const QSharedPointer<MockSetEidPinCommand> setEidCommand(new MockSetEidPinCommand(worker, pin));
			QSignalSpy spyContinue(&state, &StateChangePinRemote::fireContinue);

			state.onChangePinDone(setEidCommand);
			QCOMPARE(context->getModifyPinMessageResponseApdu().getReturnCode(), StatusCode::SUCCESS);
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateChangePinRemote)
#include "test_StateChangePinRemote.moc"
