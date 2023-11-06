/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for the class \ref IfdServiceContext.
 */

#include "MockIfdServer.h"
#include "context/IfdServiceContext.h"

#include <QSharedPointer>
#include <QSignalSpy>
#include <QtTest>


using namespace governikus;


Q_DECLARE_METATYPE(EstablishPaceChannel)


class test_IfdServiceContext
	: public QObject
{
	Q_OBJECT

	static EstablishPaceChannel createDataToParse(const PacePasswordId& pinId)
	{
		const QByteArray chat = QByteArray::fromHex("7F4C12060904007F00070301020253050000000F0F");
		const QByteArray certDescription = QByteArray::fromHex("30 8202A4"
															   "        06 0A 04007F00070301030103"
															   "        A1 0E 0C0C442D547275737420476D6248"
															   "        A3 3A 0C38476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E"
															   "        A5 820248"
															   "            04 820244 4E616D652C20416E7363687269667420756E6420452D4D61696C2D4164726573736520646573204469656E737465616E626965746572733A0D0A476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E0D0A57696C68656C6D73747261C39F652034332F3433670D0A3130313137204265726C696E0D0A6265726C696E406764762E64650D0A0D0A4765736368C3A46674737A7765636B3A0D0A2D52656769737472696572756E6720756E64204C6F67696E20616D204744562D4D616B6C6572706F7274616C2D0D0A0D0A48696E7765697320617566206469652066C3BC722064656E204469656E737465616E626965746572207A757374C3A46E646967656E205374656C6C656E2C20646965206469652045696E68616C74756E672064657220566F7273636872696674656E207A756D20446174656E73636875747A206B6F6E74726F6C6C696572656E3A0D0A4265726C696E6572204265617566747261677465722066C3BC7220446174656E73636875747A20756E6420496E666F726D6174696F6E7366726569686569740D0A416E20646572205572616E696120342D31300D0A3130373837204265726C696E0D0A3033302F3133382038392D300D0A6D61696C626F7840646174656E73636875747A2D6265726C696E2E64650D0A687474703A2F2F7777772E646174656E73636875747A2D6265726C696E2E64650D0A416E737072656368706172746E65723A2044722E20416C6578616E64657220446978");

		return EstablishPaceChannel(pinId, chat, certDescription);
	}


	QSharedPointer<IfdServer> mIfdServer;

	private Q_SLOTS:
		void initTestCase()
		{
			mIfdServer = QSharedPointer<IfdServer>(new MockIfdServer());
		}


		void test_NewPin()
		{
			IfdServiceContext context(mIfdServer);
			const QString pin1 = QStringLiteral("567890");
			const QString pin2 = QStringLiteral("654321");

			context.setNewPin(pin1);
			QCOMPARE(context.getNewPin(), pin1);

			context.setNewPin(pin2);
			QCOMPARE(context.getNewPin(), pin2);
		}


		void test_ModifyPinMessage()
		{
			IfdServiceContext context(mIfdServer);
			const QSharedPointer<const IfdModifyPin> message(new IfdModifyPin(QString("message"), QByteArray("message")));
			const QSharedPointer<const IfdModifyPin> emptyMessage(new IfdModifyPin(QString(), QByteArray()));

			context.setModifyPinMessage(message);
			QCOMPARE(context.getModifyPinMessage(), message);

			context.setModifyPinMessage(emptyMessage);
			QCOMPARE(context.getModifyPinMessage(), emptyMessage);
		}


		void test_OnResetMessageHandler()
		{
			IfdServiceContext context(mIfdServer);
			context.reset();
			QCOMPARE(context.getDisplayText(), QString());
			QCOMPARE(context.getCardConnection(), QSharedPointer<CardConnection>());
			QCOMPARE(context.getCan(), QString());
			QCOMPARE(context.getPin(), QString());
			QCOMPARE(context.getPuk(), QString());
			QCOMPARE(context.getNewPin(), QString());
			QCOMPARE(context.getLastPaceResult(), CardReturnCode::OK);
			QVERIFY(context.getSlotHandle().isEmpty());
			QCOMPARE(context.getModifyPinMessage(), QSharedPointer<const IfdModifyPin>());
		}


		void test_EstablishPaceChannelMessage()
		{
			IfdServiceContext context(mIfdServer);
			const QSharedPointer<const IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel(QString("SlotHandle"), EstablishPaceChannel(), 6));

			context.setEstablishPaceChannel(message);
			QVERIFY(!context.getSlotHandle().isEmpty());

			context.setEstablishPaceChannel(nullptr);
			QVERIFY(context.getSlotHandle().isEmpty());
		}


		void test_IsPinChangeWorkflow_data()
		{
			QTest::addColumn<EstablishPaceChannel>("inputData");
			QTest::addColumn<bool>("isPinChangeWorkflow");

			QTest::newRow("default") << EstablishPaceChannel() << false;

			QTest::newRow("pin - no cert") << EstablishPaceChannel(PacePasswordId::PACE_PIN) << true;
			QTest::newRow("can - no cert") << EstablishPaceChannel(PacePasswordId::PACE_CAN) << false;
			QTest::newRow("puk - no cert") << EstablishPaceChannel(PacePasswordId::PACE_PUK) << false;
			QTest::newRow("unknown - no cert") << EstablishPaceChannel(PacePasswordId::UNKNOWN) << false;

			QTest::newRow("pin - cert") << createDataToParse(PacePasswordId::PACE_PIN) << false;
			QTest::newRow("can - cert") << createDataToParse(PacePasswordId::PACE_CAN) << false;
			QTest::newRow("puk - cert") << createDataToParse(PacePasswordId::PACE_PUK) << false;
			QTest::newRow("unknown - cert") << createDataToParse(PacePasswordId::UNKNOWN) << false;
		}


		void test_IsPinChangeWorkflow()
		{
			QFETCH(EstablishPaceChannel, inputData);
			QFETCH(bool, isPinChangeWorkflow);

			IfdServiceContext context(mIfdServer);
			QSharedPointer<const IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel(QString(), inputData, 6));
			context.setEstablishPaceChannel(message);

			QCOMPARE(context.isPinChangeWorkflow(), isPinChangeWorkflow);
		}


		void test_IsPinAuthentication_data()
		{
			QTest::addColumn<EstablishPaceChannel>("inputData");
			QTest::addColumn<bool>("isPinAuthentication");

			QTest::newRow("default") << EstablishPaceChannel() << false;

			QTest::newRow("pin - no cert") << EstablishPaceChannel(PacePasswordId::PACE_PIN) << false;
			QTest::newRow("can - no cert") << EstablishPaceChannel(PacePasswordId::PACE_CAN) << false;
			QTest::newRow("puk - no cert") << EstablishPaceChannel(PacePasswordId::PACE_PUK) << false;
			QTest::newRow("unknown - no cert") << EstablishPaceChannel(PacePasswordId::UNKNOWN) << false;

			QTest::newRow("pin - cert") << createDataToParse(PacePasswordId::PACE_PIN) << true;
			QTest::newRow("can - cert") << createDataToParse(PacePasswordId::PACE_CAN) << false;
			QTest::newRow("puk - cert") << createDataToParse(PacePasswordId::PACE_PUK) << false;
			QTest::newRow("unknown - cert") << createDataToParse(PacePasswordId::UNKNOWN) << false;
		}


		void test_IsPinAuthentication()
		{
			QFETCH(EstablishPaceChannel, inputData);
			QFETCH(bool, isPinAuthentication);

			IfdServiceContext context(mIfdServer);
			QSharedPointer<const IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel(QString(), inputData, 6));
			context.setEstablishPaceChannel(message);

			QCOMPARE(context.isPinAuthentication(), isPinAuthentication);
		}


		void test_PinLength_data()
		{
			QTest::addColumn<EstablishPaceChannel>("inputData");
			QTest::addColumn<int>("pinLength");
			QTest::addColumn<bool>("requestTransportPin");

			QTest::newRow("default - 0") << EstablishPaceChannel() << 0 << false;
			QTest::newRow("default - 5") << EstablishPaceChannel() << 5 << true;
			QTest::newRow("default - 6") << EstablishPaceChannel() << 6 << false;

			QTest::newRow("pin - no cert - 0") << EstablishPaceChannel(PacePasswordId::PACE_PIN) << 0 << false;
			QTest::newRow("pin - no cert - 5") << EstablishPaceChannel(PacePasswordId::PACE_PIN) << 5 << true;
			QTest::newRow("pin - no cert - 6") << EstablishPaceChannel(PacePasswordId::PACE_PIN) << 6 << false;

			QTest::newRow("can - no cert - 0") << EstablishPaceChannel(PacePasswordId::PACE_CAN) << 0 << false;
			QTest::newRow("can - no cert - 5") << EstablishPaceChannel(PacePasswordId::PACE_CAN) << 5 << true;
			QTest::newRow("can - no cert - 6") << EstablishPaceChannel(PacePasswordId::PACE_CAN) << 6 << false;

			QTest::newRow("puk - no cert - 0") << EstablishPaceChannel(PacePasswordId::PACE_PUK) << 0 << false;
			QTest::newRow("puk - no cert - 5") << EstablishPaceChannel(PacePasswordId::PACE_PUK) << 5 << true;
			QTest::newRow("puk - no cert - 6") << EstablishPaceChannel(PacePasswordId::PACE_PUK) << 6 << false;

			QTest::newRow("unknown - no cert - 0") << EstablishPaceChannel(PacePasswordId::UNKNOWN) << 0 << false;
			QTest::newRow("unknown - no cert - 5") << EstablishPaceChannel(PacePasswordId::UNKNOWN) << 5 << true;
			QTest::newRow("unknown - no cert - 6") << EstablishPaceChannel(PacePasswordId::UNKNOWN) << 6 << false;

			QTest::newRow("pin - cert - 0") << createDataToParse(PacePasswordId::PACE_PIN) << 0 << false;
			QTest::newRow("pin - cert - 5") << createDataToParse(PacePasswordId::PACE_PIN) << 5 << true;
			QTest::newRow("pin - cert - 6") << createDataToParse(PacePasswordId::PACE_PIN) << 6 << false;

			QTest::newRow("can - cert - 0") << createDataToParse(PacePasswordId::PACE_CAN) << 0 << false;
			QTest::newRow("can - cert - 5") << createDataToParse(PacePasswordId::PACE_CAN) << 5 << true;
			QTest::newRow("can - cert - 6") << createDataToParse(PacePasswordId::PACE_CAN) << 6 << false;

			QTest::newRow("puk - cert - 0") << createDataToParse(PacePasswordId::PACE_PUK) << 0 << false;
			QTest::newRow("puk - cert - 5") << createDataToParse(PacePasswordId::PACE_PUK) << 5 << true;
			QTest::newRow("puk - cert - 6") << createDataToParse(PacePasswordId::PACE_PUK) << 6 << false;

			QTest::newRow("unknown - cert - 0") << createDataToParse(PacePasswordId::UNKNOWN) << 0 << false;
			QTest::newRow("unknown - cert - 5") << createDataToParse(PacePasswordId::UNKNOWN) << 5 << true;
			QTest::newRow("unknown - cert - 6") << createDataToParse(PacePasswordId::UNKNOWN) << 6 << false;
		}


		void test_PinLength()
		{
			QFETCH(EstablishPaceChannel, inputData);
			QFETCH(int, pinLength);
			QFETCH(bool, requestTransportPin);

			IfdServiceContext context(mIfdServer);
			QSharedPointer<const IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel(QString(), inputData, pinLength));
			context.setEstablishPaceChannel(message);

			QCOMPARE(context.isRequestTransportPin(), requestTransportPin);
		}


};

QTEST_GUILESS_MAIN(test_IfdServiceContext)
#include "test_IfdServiceContext.moc"
