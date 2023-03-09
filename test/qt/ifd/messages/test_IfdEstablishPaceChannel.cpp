/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdEstablishPaceChannel.h"

#include "LogHandler.h"
#include "TestFileHelper.h"

#include <QtTest>


using namespace governikus;


Q_DECLARE_METATYPE(IfdVersion::Version)


class test_IfdEstablishPaceChannel
	: public QObject
{
	Q_OBJECT

	const QByteArray mChatHex = "7F4C12060904007F00070301020253050000000F0F";
	const QByteArray mCertHex = "30 8202A4"
								"06 0A 04007F00070301030103"
								"A1 0E 0C0C442D547275737420476D6248"
								"A3 3A 0C38476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E"
								"A5 820248"
								"04 820244 4E616D652C20416E7363687269667420756E6420452D4D61696C2D4164726573736520646573204469656E737465616E626965746572733A0D0A476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E0D0A57696C68656C6D73747261C39F652034332F3433670D0A3130313137204265726C696E0D0A6265726C696E406764762E64650D0A0D0A4765736368C3A46674737A7765636B3A0D0A2D52656769737472696572756E6720756E64204C6F67696E20616D204744562D4D616B6C6572706F7274616C2D0D0A0D0A48696E7765697320617566206469652066C3BC722064656E204469656E737465616E626965746572207A757374C3A46E646967656E205374656C6C656E2C20646965206469652045696E68616C74756E672064657220566F7273636872696674656E207A756D20446174656E73636875747A206B6F6E74726F6C6C696572656E3A0D0A4265726C696E6572204265617566747261677465722066C3BC7220446174656E73636875747A20756E6420496E666F726D6174696F6E7366726569686569740D0A416E20646572205572616E696120342D31300D0A3130373837204265726C696E0D0A3033302F3133382038392D300D0A6D61696C626F7840646174656E73636875747A2D6265726C696E2E64650D0A687474703A2F2F7777772E646174656E73636875747A2D6265726C696E2E64650D0A416E737072656368706172746E65723A2044722E20416C6578616E64657220446978";

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void invalidJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			IfdEstablishPaceChannel msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 6);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"msg\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ContextHandle\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"SlotHandle\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"InputData\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDEstablishPACEChannel")));
		}


		void values()
		{
			EstablishPaceChannel establishPaceChannel;
			const IfdEstablishPaceChannel ifdEstablishPaceChannel("SlotHandle", establishPaceChannel, 6);

			QVERIFY(!ifdEstablishPaceChannel.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannel.getType(), IfdMessageType::IFDEstablishPACEChannel);
			QCOMPARE(ifdEstablishPaceChannel.getContextHandle(), QString());
			QCOMPARE(ifdEstablishPaceChannel.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdEstablishPaceChannel.getInputData(), establishPaceChannel);
			QCOMPARE(ifdEstablishPaceChannel.getExpectedPinLength(), 6);
		}


		void toJson_data()
		{
			QTest::addColumn<IfdVersion::Version>("version");
			QTest::addColumn<QByteArray>("inputApdu");

			QTest::newRow("Unknown") << IfdVersion::Version::Unknown << QByteArray("ff9a0402073005a10302010300");
			QTest::newRow("v0") << IfdVersion::Version::v0 << QByteArray("ff9a0402073005a10302010300");
			QTest::newRow("v2") << IfdVersion::Version::v2 << QByteArray("0300000000");
		}


		void toJson()
		{
			QFETCH(IfdVersion::Version, version);
			QFETCH(QByteArray, inputApdu);

			const IfdEstablishPaceChannel ifdEstablishPaceChannel(QStringLiteral("SlotHandle"), EstablishPaceChannel(PacePasswordId::PACE_PIN), 6);

			const QByteArray& byteArray = ifdEstablishPaceChannel.toByteArray(version, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "[LENGTH]"
							   "    \"InputData\": \"[DATA]\",\n"
							   "[LENGTH_OLD]"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDEstablishPACEChannel\"\n"
							   "}\n")
					.replace("[DATA]", inputApdu)
					.replace("[LENGTH]", version >= IfdVersion::Version::v2 ? QByteArray("    \"ExpectedPINLength\": 6,\n") : QByteArray())
					.replace("[LENGTH_OLD]", version >= IfdVersion::Version::v2 ? QByteArray("    \"PreferredPinLength\": 6,\n") : QByteArray()));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), version >= IfdVersion::Version::v2 ? 6 : 4);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDEstablishPACEChannel"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			QCOMPARE(obj.value(QLatin1String("InputData")).toString(), QString::fromLatin1(inputApdu));
			QCOMPARE(obj.value(QLatin1String("ExpectedPINLength")).toInt(), version >= IfdVersion::Version::v2 ? 6 : 0);
		}


		void fromJson_data()
		{
			QTest::addColumn<QByteArray>("inputData");
			QTest::addColumn<bool>("incomplete");

			QByteArray ccidHex("FF9A04020002CE308202CAA103020103A3170415 CHAT A48202A8 CERT 01 00");
			ccidHex.replace(" CHAT ", mChatHex);
			ccidHex.replace(" CERT ", mCertHex);
			QTest::newRow("CCID") << ccidHex << !IfdVersion(IfdVersion::Version::v0).isSupported();

			QByteArray pcscHex("0315 CHAT 00A802 CERT ");
			pcscHex.replace(" CHAT ", mChatHex);
			pcscHex.replace(" CERT ", mCertHex);
			QTest::newRow("PCSC") << pcscHex << false;

			QTest::newRow("empty") << QByteArray() << true;
			QTest::newRow("shortGarbage") << QByteArray("1137") << true;
			QTest::newRow("longGarbage") << QByteArray("113711371137") << true;
		}


		void fromJson()
		{
			QFETCH(QByteArray, inputData);
			QFETCH(bool, incomplete);

			const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
									"ContextHandle": "TestContext",
									"InputData": "[DATA]",
									"ExpectedPINLength": 6,
									"SlotHandle": "SlotHandle",
									"msg": "IFDEstablishPACEChannel"
								 })");
			message.replace("[DATA]", inputData);

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannel ifdEstablishPaceChannel(obj);
			QCOMPARE(ifdEstablishPaceChannel.isIncomplete(), incomplete);
			QCOMPARE(ifdEstablishPaceChannel.getType(), IfdMessageType::IFDEstablishPACEChannel);
			QCOMPARE(ifdEstablishPaceChannel.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishPaceChannel.getSlotHandle(), QStringLiteral("SlotHandle"));
			if (incomplete)
			{
				QCOMPARE(ifdEstablishPaceChannel.getInputData(), EstablishPaceChannel());
			}
			else
			{
				QCOMPARE(ifdEstablishPaceChannel.getInputData(), EstablishPaceChannel(PacePasswordId::PACE_PIN, QByteArray::fromHex(mChatHex), QByteArray::fromHex(mCertHex)));
			}
			QCOMPARE(ifdEstablishPaceChannel.getExpectedPinLength(), 6);

			QCOMPARE(logSpy.count(), incomplete ? v0Supported ? 3 : 2 : 0);
			if (incomplete)
			{
				if (v0Supported)
				{
					QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("(v0) The value of InputData should be as defined in TR-03119 section D.3")));
					QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("(v2) The value of InputData should be as defined in PC/SC Part 10 AMD1 section 2.6.16")));
				}
				else
				{
					QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of InputData should be as defined in PC/SC Part 10 AMD1 section 2.6.16")));
				}
			}
		}


		void msgField_data()
		{
			QTest::addColumn<IfdMessageType>("type");

			const auto& msgTypes = Enum<IfdMessageType>::getList();
			for (const auto& type : msgTypes)
			{
				QTest::newRow(getEnumName(type).data()) << type;
			}
		}


		void msgField()
		{
			QFETCH(IfdMessageType, type);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
									"ContextHandle": "TestContext",
									"InputData": "0100000000",
									"ExpectedPINLength": 6,
									"SlotHandle": "SlotHandle",
									"msg": "%1"
							   })");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdEstablishPaceChannel ifdEstablishPaceChannel(obj);

			if (type == IfdMessageType::IFDEstablishPACEChannel)
			{
				QVERIFY(!ifdEstablishPaceChannel.isIncomplete());
				QCOMPARE(ifdEstablishPaceChannel.getType(), IfdMessageType::IFDEstablishPACEChannel);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdEstablishPaceChannel.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannel.getType(), type);

			if (type == IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"UNDEFINED\"")));
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDEstablishPACEChannel")));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDEstablishPACEChannel")));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message(R"({
										"ContextHandle": "TestContext",
										"InputData": 1,
										"ExpectedPINLength": "Hello World!",
										"SlotHandle": 2,
										"msg": "IFDEstablishPACEChannel"
									 })");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannel ifdEstablishPaceChannel(obj);
			QVERIFY(ifdEstablishPaceChannel.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannel.getType(), IfdMessageType::IFDEstablishPACEChannel);
			QCOMPARE(ifdEstablishPaceChannel.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishPaceChannel.getSlotHandle(), QString());
			QCOMPARE(ifdEstablishPaceChannel.getInputData(), EstablishPaceChannel());
			QCOMPARE(ifdEstablishPaceChannel.getExpectedPinLength(), 0);

			QCOMPARE(logSpy.count(), 3);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"SlotHandle\" should be of type \"string\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"InputData\" should be of type \"string\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of \"ExpectedPINLength\" should be of type \"number\"")));
		}


		void wrongInputData()
		{
			const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
									"ContextHandle": "TestContext",
									"InputData": "Hello World!",
									"ExpectedPINLength": 6,
									"SlotHandle": "SlotHandle",
									"msg": "IFDEstablishPACEChannel"
							   })");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannel ifdEstablishPaceChannel(obj);
			QVERIFY(ifdEstablishPaceChannel.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannel.getType(), IfdMessageType::IFDEstablishPACEChannel);
			QCOMPARE(ifdEstablishPaceChannel.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishPaceChannel.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdEstablishPaceChannel.getInputData(), EstablishPaceChannel());

			QCOMPARE(logSpy.count(), v0Supported ? 3 : 2);
			if (v0Supported)
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("(v0) The value of InputData should be as defined in TR-03119 section D.3")));
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("(v2) The value of InputData should be as defined in PC/SC Part 10 AMD1 section 2.6.16")));
			}
			else
			{
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of InputData should be as defined in PC/SC Part 10 AMD1 section 2.6.16")));
			}
		}


		void expectedPinLength_data()
		{
			QTest::addColumn<QByteArray>("json");
			QTest::addColumn<int>("expectedPinLength");

			QTest::newRow("empty") << QByteArray() << 0;
			QTest::newRow("0") << QByteArray(R"("ExpectedPINLength": 0,)") << 0;
			QTest::newRow("5") << QByteArray(R"("ExpectedPINLength": 5,)") << 5;
			QTest::newRow("6") << QByteArray(R"("ExpectedPINLength": 6,)") << 6;
		}


		void expectedPinLength()
		{
			QFETCH(QByteArray, json);
			QFETCH(int, expectedPinLength);

			QByteArray message(R"({
									"ContextHandle": "TestContext",
									"InputData": "0300000000",
									[JSON]
									"SlotHandle": "SlotHandle",
									"msg": "IFDEstablishPACEChannel"
								 })");
			message.replace("[JSON]", json);

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannel ifdEstablishPaceChannel(obj);
			QVERIFY(!ifdEstablishPaceChannel.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannel.getType(), IfdMessageType::IFDEstablishPACEChannel);
			QCOMPARE(ifdEstablishPaceChannel.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishPaceChannel.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdEstablishPaceChannel.getInputData(), EstablishPaceChannel(PacePasswordId::PACE_PIN));
			QCOMPARE(ifdEstablishPaceChannel.getExpectedPinLength(), expectedPinLength);
		}


		void preferredPinLength_data()
		{
			QTest::addColumn<QByteArray>("json");
			QTest::addColumn<int>("preferredPinLength");

			QTest::newRow("0") << QByteArray(R"("PreferredPinLength": 0,)") << 0;
			QTest::newRow("5") << QByteArray(R"("PreferredPinLength": 5,)") << 5;
			QTest::newRow("6") << QByteArray(R"("PreferredPinLength": 6,)") << 6;
			QTest::newRow("both1") << QByteArray(R"("PreferredPinLength": 5, "ExpectedPINLength": 6,)") << 6;
			QTest::newRow("both2") << QByteArray(R"("PreferredPinLength": 6, "ExpectedPINLength": 5,)") << 5;
			QTest::newRow("both3") << QByteArray(R"("ExpectedPINLength": 5, "PreferredPinLength": 6,)") << 5;
			QTest::newRow("both4") << QByteArray(R"("ExpectedPINLength": 6, "PreferredPinLength": 5,)") << 6;
		}


		void preferredPinLength()
		{
			QFETCH(QByteArray, json);
			QFETCH(int, preferredPinLength);

			QByteArray message(R"({
									"ContextHandle": "TestContext",
									"InputData": "0300000000",
									[JSON]
									"SlotHandle": "SlotHandle",
									"msg": "IFDEstablishPACEChannel"
								 })");
			message.replace("[JSON]", json);

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannel ifdEstablishPaceChannel(obj);
			QVERIFY(!ifdEstablishPaceChannel.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannel.getType(), IfdMessageType::IFDEstablishPACEChannel);
			QCOMPARE(ifdEstablishPaceChannel.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishPaceChannel.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdEstablishPaceChannel.getInputData(), EstablishPaceChannel(PacePasswordId::PACE_PIN));
			QCOMPARE(ifdEstablishPaceChannel.getExpectedPinLength(), preferredPinLength);
		}


};

QTEST_GUILESS_MAIN(test_IfdEstablishPaceChannel)
#include "test_IfdEstablishPaceChannel.moc"
