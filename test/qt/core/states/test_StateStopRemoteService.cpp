/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "states/remote_service/StateStopRemoteService.h"

#include "context/RemoteServiceContext.h"

#include "MockCardConnectionWorker.h"
#include "MockRemoteServer.h"

#include <QtTest/QtTest>


using namespace governikus;

class test_StateStopRemoteService
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
			StateStopRemoteService state(context);
			QSignalSpy spyContinue(&state, &StateStopRemoteService::fireContinue);

			state.run();
			QCOMPARE(spyContinue.count(), 1);
		}


		void test_OnExit()
		{
			const QSharedPointer<RemoteServiceContext> context(new RemoteServiceContext());
			StateStopRemoteService state(context);
			const QString name("name");
			state.setStateName(name);
			state.onExit(nullptr);
			QVERIFY(!context->getRemoteServer()->isRunning());
		}


};

QTEST_GUILESS_MAIN(test_StateStopRemoteService)
#include "test_StateStopRemoteService.moc"
