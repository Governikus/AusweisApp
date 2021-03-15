/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateStartRemoteService.h"

#include "states/StateBuilder.h"

#include "MockCardConnectionWorker.h"
#include "MockDataChannel.h"
#include "MockRemoteServer.h"

#include <QtTest>


using namespace governikus;

class test_StateStartRemoteService
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
			const QSharedPointer<StateStartRemoteService> state(StateBuilder::createState<StateStartRemoteService>(context));
			QSignalSpy spyContinue(state.data(), &StateStartRemoteService::fireContinue);

			state->onEntry(nullptr);
			context->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations

			const auto& server = context->getRemoteServer();
			const QSharedPointer<ServerMessageHandler> handler(new ServerMessageHandlerImpl(QSharedPointer<MockDataChannel>::create()));
			Q_EMIT server->fireMessageHandlerAdded(handler);
			Q_EMIT handler->fireClosed();
			QCOMPARE(context->getNewPin(), QString());
			QCOMPARE(context->getPin(), QString());
			QCOMPARE(context->getCan(), QString());
			QCOMPARE(context->getPuk(), QString());
			QCOMPARE(context->getCardConnection(), QSharedPointer<CardConnection>());
			QCOMPARE(context->getLastPaceResult(), CardReturnCode::OK);
			QVERIFY(context->getSlotHandle().isEmpty());
			QCOMPARE(context->getModifyPinMessage(), QSharedPointer<const IfdModifyPin>());
		}


};

QTEST_GUILESS_MAIN(test_StateStartRemoteService)
#include "test_StateStartRemoteService.moc"
