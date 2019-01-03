/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "states/remote_service/StateChangePinRemote.h"

#include "context/RemoteServiceContext.h"

#include "MockCardConnectionWorker.h"
#include "MockRemoteServer.h"

#include <QtTest/QtTest>


using namespace governikus;

class test_StateChangePinRemote
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::setCreator<RemoteServer*>(std::function<RemoteServer*()>([&] {
						return new MockRemoteServer();
					}));
		}


		void test_Run()
		{
			QThread workerThread;
			workerThread.start();

			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			const QString pin = QStringLiteral("103050");
			context->setPin(pin);
			const QString slotHandle = QStringLiteral("slot");
			const QByteArray input("input");
			const QSharedPointer<const IfdModifyPin> message(new IfdModifyPin(slotHandle, input));
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			context->setCardConnection(connection);
			StateChangePinRemote state(context);
			context->setModifyPinMessage(message);

			state.run();
			QCOMPARE(state.mConnections.size(), 1);
			QCOMPARE(context->getPin(), QString("103050"));

			workerThread.quit();
			workerThread.wait();
		}


};

QTEST_GUILESS_MAIN(test_StateChangePinRemote)
#include "test_StateChangePinRemote.moc"
