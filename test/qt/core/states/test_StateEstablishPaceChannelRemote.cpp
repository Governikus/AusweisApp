/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "states/remote_service/StateEstablishPaceChannelRemote.h"

#include "context/AuthContext.h"
#include "EstablishPaceChannelParser.h"

#include "MockCardConnectionWorker.h"
#include "MockRemoteServer.h"

#include <QtTest/QtTest>


using namespace governikus;

class test_StateEstablishPaceChannelRemote
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
			QThread connectionThread;
			connectionThread.start();

			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			StateEstablishPaceChannelRemote state(context);
			const QString slotHandle = QStringLiteral("slothandle");
			const QByteArray input("abc");
			const QSharedPointer<IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel(slotHandle, input));
			context->setEstablishPaceChannelMessage(message);
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&connectionThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			context->setCardConnection(connection);
			context->setPin(QString("1234"));

			state.run();
			QCOMPARE(context->getPin(), QString("1234"));
			QCOMPARE(state.mConnections.size(), 1);

			connectionThread.quit();
			connectionThread.wait();
		}


};

QTEST_GUILESS_MAIN(test_StateEstablishPaceChannelRemote)
#include "test_StateEstablishPaceChannelRemote.moc"
