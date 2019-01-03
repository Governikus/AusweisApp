/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "states/remote_service/StateStartRemoteService.h"

#include "MockCardConnectionWorker.h"
#include "MockRemoteServer.h"

#include <QtTest/QtTest>


using namespace governikus;

class test_StateStartRemoteService
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
			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			StateStartRemoteService state(context);
			QSignalSpy spyContinue(&state, &StateStartRemoteService::fireContinue);

			state.run();
			QCOMPARE(spyContinue.count(), 1);
			state.onMessageHandlerAdded(nullptr);
		}


};

QTEST_GUILESS_MAIN(test_StateStartRemoteService)
#include "test_StateStartRemoteService.moc"
