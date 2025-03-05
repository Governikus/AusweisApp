/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateProcessIfdMessages.h"

#include "ServerMessageHandlerImpl.h"
#include "context/IfdServiceContext.h"

#include "MockCardConnectionWorker.h"
#include "MockDataChannel.h"
#include "MockIfdServer.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_StateProcessIfdMessages
	: public QObject
{
	Q_OBJECT

	QSharedPointer<IfdServer> mIfdServer;

	private Q_SLOTS:
		void initTestCase()
		{
			mIfdServer = QSharedPointer<IfdServer>(new MockIfdServer());
		}


		void test_Run()
		{
			const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(mIfdServer));
			StateProcessIfdMessages state(context);
			state.run();
			QCOMPARE(state.mConnections.size(), 3);
			QCOMPARE(state.mMessageConnections.size(), 7);
		}


		void test_OnMessageHandlerAdded()
		{
			const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(mIfdServer));
			StateProcessIfdMessages state(context);
			const QSharedPointer<ServerMessageHandlerImpl> messageHandler(new ServerMessageHandlerImpl(QSharedPointer<MockDataChannel>::create()));

			state.onMessageHandlerAdded(nullptr);
			QCOMPARE(state.mMessageConnections.size(), 0);

			state.onMessageHandlerAdded(messageHandler);
			QCOMPARE(state.mMessageConnections.size(), 7);
		}


		void test_OnCardConnected()
		{
			const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(mIfdServer));
			StateProcessIfdMessages state(context);
			QVERIFY(!context->getCardInitiallyAppeared());
			state.onCardConnected();
			QVERIFY(context->getCardInitiallyAppeared());
		}


		void test_OnClosed()
		{
			const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(mIfdServer));
			StateProcessIfdMessages state(context);
			context->setCardInitiallyAppeared();
			state.onClosed();
			QVERIFY(!context->getCardInitiallyAppeared());
		}


		void test_OnEstablishPaceChannel()
		{
			QThread workerThread;
			workerThread.start();

			const PacePasswordId pinId(PacePasswordId::PACE_PIN);
			const QByteArray chat = QByteArray::fromHex("7F4C12060904007F00070301020253050000000F0F");
			const QByteArray certDescription = QByteArray::fromHex("30 8202A4"
																   "        06 0A 04007F00070301030103"
																   "        A1 0E 0C0C442D547275737420476D6248"
																   "        A3 3A 0C38476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E"
																   "        A5 820248"
																   "            04 820244 4E616D652C20416E7363687269667420756E6420452D4D61696C2D4164726573736520646573204469656E737465616E626965746572733A0D0A476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E0D0A57696C68656C6D73747261C39F652034332F3433670D0A3130313137204265726C696E0D0A6265726C696E406764762E64650D0A0D0A4765736368C3A46674737A7765636B3A0D0A2D52656769737472696572756E6720756E64204C6F67696E20616D204744562D4D616B6C6572706F7274616C2D0D0A0D0A48696E7765697320617566206469652066C3BC722064656E204469656E737465616E626965746572207A757374C3A46E646967656E205374656C6C656E2C20646965206469652045696E68616C74756E672064657220566F7273636872696674656E207A756D20446174656E73636875747A206B6F6E74726F6C6C696572656E3A0D0A4265726C696E6572204265617566747261677465722066C3BC7220446174656E73636875747A20756E6420496E666F726D6174696F6E7366726569686569740D0A416E20646572205572616E696120342D31300D0A3130373837204265726C696E0D0A3033302F3133382038392D300D0A6D61696C626F7840646174656E73636875747A2D6265726C696E2E64650D0A687474703A2F2F7777772E646174656E73636875747A2D6265726C696E2E64650D0A416E737072656368706172746E65723A2044722E20416C6578616E64657220446978");
			EstablishPaceChannel establishPaceChannel(pinId, chat, certDescription);

			const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(mIfdServer));
			StateProcessIfdMessages state(context);
			const QSharedPointer<const IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel("test"_L1, establishPaceChannel, 6));
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			QSignalSpy spy(&state, &StateProcessIfdMessages::fireEstablishPaceChannel);

			state.onEstablishPaceChannel(message, connection);
			QVERIFY(!context->getSlotHandle().isEmpty());
			QCOMPARE(context->getSlotHandle(), "test"_L1);
			const auto& paceChannel = context->getEstablishPaceChannel();
			QCOMPARE(paceChannel.getPasswordId(), pinId);
			QCOMPARE(paceChannel.getChat(), chat);
			QCOMPARE(paceChannel.getCertificateDescription(), certDescription);
			QCOMPARE(context->getCardConnection(), connection);
			QCOMPARE(spy.count(), 1);

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnModifyPin()
		{
			QThread workerThread;
			workerThread.start();

			const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(mIfdServer));
			StateProcessIfdMessages state(context);
			const QSharedPointer<const IfdModifyPin> message(new IfdModifyPin());
			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection(new CardConnection(worker));
			QSignalSpy spy(&state, &StateProcessIfdMessages::fireModifyPin);

			state.onModifyPin(message, connection);
			QCOMPARE(context->getModifyPinMessage(), message);
			QCOMPARE(context->getCardConnection(), connection);
			QCOMPARE(spy.count(), 1);

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnDisplayTextChanged()
		{
			const QSharedPointer<IfdServiceContext> context(new IfdServiceContext(mIfdServer));
			StateProcessIfdMessages state(context);
			QSignalSpy spy(context.data(), &IfdServiceContext::fireDisplayTextChanged);

			state.onDisplayTextChanged(QStringLiteral("dummy text"));
			QCOMPARE(spy.count(), 1);
			QCOMPARE(context->getDisplayText(), QStringLiteral("dummy text"));
			state.onCardDisconnected();
			QCOMPARE(spy.count(), 2);
			QCOMPARE(context->getDisplayText(), QStringLiteral(""));
		}


};

QTEST_GUILESS_MAIN(test_StateProcessIfdMessages)
#include "test_StateProcessIfdMessages.moc"
