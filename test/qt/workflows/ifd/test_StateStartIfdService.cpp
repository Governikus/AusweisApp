/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateStartIfdService.h"

#include "ServerMessageHandlerImpl.h"
#include "states/StateBuilder.h"

#include "MockCardConnectionWorker.h"
#include "MockDataChannel.h"
#include "MockIfdServer.h"

#include <QtTest>


using namespace governikus;

class test_StateStartIfdService
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_Run()
		{
			const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(QSharedPointer<IfdServer>(new MockIfdServer())));
			const QSharedPointer<StateStartIfdService> state(StateBuilder::createState<StateStartIfdService>(context));
			QSignalSpy spyContinue(state.data(), &StateStartIfdService::fireContinue);

			state->onEntry(nullptr);
			context->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations

			const auto& server = context->getIfdServer();
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

QTEST_GUILESS_MAIN(test_StateStartIfdService)
#include "test_StateStartIfdService.moc"
