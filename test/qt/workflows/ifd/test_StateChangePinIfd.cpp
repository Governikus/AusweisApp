/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateChangePinIfd.h"

#include "context/IfdServiceContext.h"

#include "MockCardConnectionWorker.h"
#include "MockIfdServer.h"
#include "TestHookThread.h"

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
			TestHookThread workerThread;

			{
				auto* ifdServer = new MockIfdServer();
				const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(QSharedPointer<IfdServer>(ifdServer)));
				StateChangePinIfd state(context);
				const QString slotHandle = QStringLiteral("slot");
				const QByteArray input("input");
				const QSharedPointer<const IfdModifyPin> message(new IfdModifyPin(slotHandle, input));
				context->setModifyPinMessage(message);

				QSignalSpy spyContinue(&state, &StateChangePinIfd::fireContinue);
				context->setNewPin(QStringLiteral("111111"));
				state.run();
				QCOMPARE(context->getModifyPinMessageResponseApdu().getStatusCode(), StatusCode::UNKNOWN);
				QCOMPARE(spyContinue.count(), 1);

				auto worker = MockCardConnectionWorker::create(&workerThread);
				context->setCardConnection(QSharedPointer<CardConnection>::create(worker));
				state.run();
				QCOMPARE(state.mConnections.size(), 1);
				QTRY_COMPARE(spyContinue.size(), 2);

				context->resetCardConnection();
				QSignalSpy spy(ifdServer, &MockIfdServer::fireDataChannelDestroyed);
				ifdServer->stop();
				QTRY_COMPARE(spy.count(), 1);

				context->resetCardConnection();
			}
		}


		void test_OnChangePinDone()
		{
			const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(QSharedPointer<IfdServer>(new MockIfdServer())));
			StateChangePinIfd state(context);

			const auto& worker = MockCardConnectionWorker::create();
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
