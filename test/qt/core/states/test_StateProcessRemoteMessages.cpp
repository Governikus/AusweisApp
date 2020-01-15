/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "states/remote_service/StateProcessRemoteMessages.h"

#include "context/RemoteServiceContext.h"

#include "MockCardConnectionWorker.h"
#include "MockRemoteServer.h"

#include <QtTest>


using namespace governikus;

class test_StateProcessRemoteMessages
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
			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			StateProcessRemoteMessages state(context);
			state.run();
			QCOMPARE(state.mConnections.size(), 2);
			QCOMPARE(state.mMessageConnections.size(), 4);
		}


		void test_OnMessageHandlerAdded()
		{
			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			StateProcessRemoteMessages state(context);
			const QSharedPointer<ServerMessageHandlerImpl> messageHandler(new ServerMessageHandlerImpl(nullptr));

			state.onMessageHandlerAdded(nullptr);
			QCOMPARE(state.mMessageConnections.size(), 0);

			state.onMessageHandlerAdded(messageHandler);
			QCOMPARE(state.mMessageConnections.size(), 4);
		}


		void test_OnEstablishPaceChannel()
		{
			QThread workerThread;
			workerThread.start();

			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			StateProcessRemoteMessages state(context);
			const QSharedPointer<const IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel());
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			QSignalSpy spy(&state, &StateProcessRemoteMessages::fireEstablishPaceChannel);

			state.onEstablishPaceChannel(message, connection);
			QCOMPARE(context->getEstablishPaceChannelMessage(), message);
			QCOMPARE(context->getCardConnection(), connection);
			QCOMPARE(spy.count(), 1);

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnModifyPin()
		{
			QThread workerThread;
			workerThread.start();

			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			StateProcessRemoteMessages state(context);
			const QSharedPointer<const IfdModifyPin> message(new IfdModifyPin());
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			QSignalSpy spy(&state, &StateProcessRemoteMessages::fireModifyPin);

			state.onModifyPin(message, connection);
			QCOMPARE(context->getModifyPinMessage(), message);
			QCOMPARE(context->getCardConnection(), connection);
			QCOMPARE(spy.count(), 1);

			workerThread.quit();
			workerThread.wait();
		}


};

QTEST_GUILESS_MAIN(test_StateProcessRemoteMessages)
#include "test_StateProcessRemoteMessages.moc"
