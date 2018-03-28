/*!
 * \brief Unit tests for \ref RemoteMessages
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "messages/Discovery.h"
#include "messages/GetIfdStatus.h"
#include "messages/IfdConnect.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdEstablishPaceChannelResponse.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmit.h"
#include "messages/IfdTransmitResponse.h"

#include "MockIfdStatus.h"
#include "RemoteMessageChecker.h"
#include "TestFileHelper.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>


using namespace governikus;


Q_DECLARE_METATYPE(QSharedPointer<EFCardAccess> )


class test_RemoteMessages
	: public QObject
{
	Q_OBJECT

	private:
		RemoteMessageChecker mChecker;

	private Q_SLOTS:
		void discovery()
		{
			const QSharedPointer<const Discovery> message(
				new Discovery(QStringLiteral("Sony Xperia Z5 compact"),
				QStringLiteral("0123456789ABCDEF"),
				24728,
				{IfdVersion::Version::v0})
				);

			mChecker.processDiscovery(message);

			const QJsonDocument document = message->toJson();
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 5);
			QCOMPARE(object.value(QLatin1String("IFDName")).toString(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(object.value(QLatin1String("IFDID")).toString(), QStringLiteral("0123456789ABCDEF"));
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("REMOTE_IFD"));
			QCOMPARE(object.value(QLatin1String("port")).toInt(), 24728);
			const QJsonValue apiLevels = object.value(QLatin1String("SupportedAPI"));
			QVERIFY(apiLevels.isArray());
			QCOMPARE(apiLevels.toArray().size(), 1);
			QCOMPARE(apiLevels.toArray().at(0).toString(), QStringLiteral("IFDInterface_WebSocket_v0"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"IFDID\": \"0123456789ABCDEF\",\n"
							   "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
							   "    \"SupportedAPI\": [\n"
							   "        \"IFDInterface_WebSocket_v0\"\n"
							   "    ],\n"
							   "    \"msg\": \"REMOTE_IFD\",\n"
							   "    \"port\": 24728\n"
							   "}\n"));
		}


		void ifdEstablishContext()
		{
			const QSharedPointer<const IfdEstablishContext> message(new IfdEstablishContext(IfdVersion::Version::v0, "MAC-MINI"));

			mChecker.receive(message);

			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());
			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 3);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("IFDEstablishContext"));
			QCOMPARE(object.value(QLatin1String("Protocol")).toString(), QStringLiteral("IFDInterface_WebSocket_v0"));
			QCOMPARE(object.value(QLatin1String("UDName")).toString(), QStringLiteral("MAC-MINI"));

			QByteArray jsonToCompare =
					QByteArray("{\n"
							   "    \"Protocol\": \"IFDInterface_WebSocket_v0\",\n"
							   "    \"UDName\": \"MAC-MINI\",\n"
							   "    \"msg\": \"IFDEstablishContext\"\n}"
							   "\n");

			QCOMPARE(document.toJson(), jsonToCompare);
		}


		void ifdEstablishContextResponse()
		{
			const QSharedPointer<const IfdEstablishContextResponse> message(new IfdEstablishContextResponse(QStringLiteral("IFD Reader"), QStringLiteral("/minorResult")));

			mChecker.receive(message);

			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());
			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 5);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("IFDEstablishContextResponse"));
			QCOMPARE(object.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(object.value(QLatin1String("IFDName")).toString(), QStringLiteral("IFD Reader"));
			QCOMPARE(object.value(QLatin1String("ResultMajor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error"));
			QCOMPARE(object.value(QLatin1String("ResultMinor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"IFDName\": \"IFD Reader\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
							   "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
							   "    \"msg\": \"IFDEstablishContextResponse\"\n"
							   "}\n"));
		}


		void paceCapabilities_data()
		{
			QTest::addColumn<bool>("pace");
			QTest::addColumn<bool>("eid");
			QTest::addColumn<bool>("esign");
			QTest::addColumn<bool>("destroy");

			QTest::newRow("false") << false << false << false << false;
			QTest::newRow("true") << true << true << true << true;
		}


		void paceCapabilities()
		{
			QFETCH(bool, pace);
			QFETCH(bool, eid);
			QFETCH(bool, esign);
			QFETCH(bool, destroy);

			PaceCapabilities capabilities(pace, eid, esign, destroy);

			QCOMPARE(capabilities.getPace(), pace);
			QCOMPARE(capabilities.getEId(), eid);
			QCOMPARE(capabilities.getESign(), esign);
			QCOMPARE(capabilities.getDestroy(), destroy);
		}


		void getIfdStatus()
		{
			const QSharedPointer<const GetIfdStatus> message(new GetIfdStatus("Remote Reader"));

			mChecker.receive(message);

			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());
			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 3);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("IFDGetStatus"));
			QCOMPARE(object.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(object.value(QLatin1String("SlotName")).toString(), QStringLiteral("Remote Reader"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"SlotName\": \"Remote Reader\",\n"
							   "    \"msg\": \"IFDGetStatus\"\n"
							   "}\n"));
		}


		void ifdStatus()
		{
			const QSharedPointer<const IfdStatus> message(new MockIfdStatus(QStringLiteral("NFC Reader"),
					PaceCapabilities(),
					500,
					true,
					false));

			mChecker.receive(message);

			// Json
			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 9);
			QCOMPARE(object.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("IFDStatus"));
			QCOMPARE(object.value(QLatin1String("SlotName")).toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(object.value(QLatin1String("ConnectedReader")).toBool(), true);
			QCOMPARE(object.value(QLatin1String("MaxAPDULength")).toInt(), 500);
			QCOMPARE(object.value(QLatin1String("CardAvailable")).toBool(), false);

			const QJsonValue pinCapVal = object.value(QLatin1String("PINCapabilities"));
			QVERIFY(pinCapVal.isObject());
			const QJsonObject pinCapObject = pinCapVal.toObject();
			QCOMPARE(pinCapObject.size(), 4);
			QCOMPARE(pinCapObject.value(QLatin1String("PACE")).toBool(), false);
			QCOMPARE(pinCapObject.value(QLatin1String("eID")).toBool(), false);
			QCOMPARE(pinCapObject.value(QLatin1String("eSign")).toBool(), false);
			QCOMPARE(pinCapObject.value(QLatin1String("Destroy")).toBool(), false);

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"CardAvailable\": false,\n"
							   "    \"ConnectedReader\": true,\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"EFATR\": null,\n"
							   "    \"EFDIR\": null,\n"
							   "    \"MaxAPDULength\": 500,\n"
							   "    \"PINCapabilities\": {\n"
							   "        \"Destroy\": false,\n"
							   "        \"PACE\": false,\n"
							   "        \"eID\": false,\n"
							   "        \"eSign\": false\n"
							   "    },\n"
							   "    \"SlotName\": \"NFC Reader\",\n"
							   "    \"msg\": \"IFDStatus\"\n"
							   "}\n"));
		}


		void ifdConnect()
		{
			const QSharedPointer<const IfdConnect> message(new IfdConnect(QStringLiteral("NFC Reader")));

			mChecker.receive(message);

			// Json
			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 4);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("IFDConnect"));
			QCOMPARE(object.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(object.value(QLatin1String("SlotName")).toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(object.value(QLatin1String("exclusive")).toBool(), true);

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"SlotName\": \"NFC Reader\",\n"
							   "    \"exclusive\": true,\n"
							   "    \"msg\": \"IFDConnect\"\n"
							   "}\n"));
		}


		void ifdConnectResponseMsg()
		{
			const QSharedPointer<const IfdConnectResponse> message(new IfdConnectResponse(QStringLiteral("NFC Reader"), QStringLiteral("/minorResult")));

			mChecker.receive(message);

			// Json
			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 5);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("IFDConnectResponse"));
			QCOMPARE(object.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(object.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(object.value(QLatin1String("ResultMajor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error"));
			QCOMPARE(object.value(QLatin1String("ResultMinor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
							   "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
							   "    \"SlotHandle\": \"NFC Reader\",\n"
							   "    \"msg\": \"IFDConnectResponse\"\n"
							   "}\n"));
		}


		void ifdDisconnectCmd()
		{
			const QSharedPointer<const IfdDisconnect> message(new IfdDisconnect(QStringLiteral("NFC Reader")));

			mChecker.receive(message);

			// Json
			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 3);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("IFDDisconnect"));
			QCOMPARE(object.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(object.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("NFC Reader"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"SlotHandle\": \"NFC Reader\",\n"
							   "    \"msg\": \"IFDDisconnect\"\n"
							   "}\n"));
		}


		void ifdDisconnectResponseMsg()
		{
			const QSharedPointer<const IfdDisconnectResponse> message(new IfdDisconnectResponse(QStringLiteral("NFC Reader"), QStringLiteral("/minorResult")));

			mChecker.receive(message);

			// Json
			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 5);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("IFDDisconnectResponse"));
			QCOMPARE(object.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(object.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(object.value(QLatin1String("ResultMajor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error"));
			QCOMPARE(object.value(QLatin1String("ResultMinor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
							   "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
							   "    \"SlotHandle\": \"NFC Reader\",\n"
							   "    \"msg\": \"IFDDisconnectResponse\"\n"
							   "}\n"));
		}


		void ifdTransmitCmd()
		{
			const QSharedPointer<const IfdTransmit> message(new IfdTransmit(QStringLiteral("NFC Reader"), QByteArray::fromHex("00A402022F00")));

			mChecker.receive(message);

			// Json
			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 4);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("IFDTransmit"));
			QCOMPARE(object.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(object.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("NFC Reader"));
			QVERIFY(object.value(QLatin1String("CommandAPDUs")).isArray());
			const auto& commandApduValue = object.value(QLatin1String("CommandAPDUs")).toArray().at(0);
			QVERIFY(commandApduValue.isObject());
			const auto& commandApdu = commandApduValue.toObject();
			QCOMPARE(commandApdu.value(QLatin1String("InputAPDU")).toString().toUpper(), QStringLiteral("00A402022F00"));
			QVERIFY(commandApdu.value(QLatin1String("AcceptableStatusCodes")).isNull());

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"CommandAPDUs\": [\n"
							   "        {\n"
							   "            \"AcceptableStatusCodes\": null,\n"
							   "            \"InputAPDU\": \"00a402022f00\"\n"
							   "        }\n"
							   "    ],\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"SlotHandle\": \"NFC Reader\",\n"
							   "    \"msg\": \"IFDTransmit\"\n"
							   "}\n"));
		}


		void ifdTransmitResponseMsg()
		{
			const QSharedPointer<const IfdTransmitResponse> message(new IfdTransmitResponse(QStringLiteral("NFC Reader"),
					QByteArray::fromHex("9000"),
					QStringLiteral("/minorResult")));

			mChecker.receive(message);

			// Json
			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 6);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("IFDTransmitResponse"));
			QCOMPARE(object.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(object.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(object.value(QLatin1String("ResultMajor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error"));
			QCOMPARE(object.value(QLatin1String("ResultMinor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult"));
			QVERIFY(object.value(QLatin1String("ResponseAPDUs")).isArray());
			QCOMPARE(object.value(QLatin1String("ResponseAPDUs")).toArray().at(0).toString().toUpper(), QStringLiteral("9000"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"ResponseAPDUs\": [\n"
							   "        \"9000\"\n"
							   "    ],\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
							   "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
							   "    \"SlotHandle\": \"NFC Reader\",\n"
							   "    \"msg\": \"IFDTransmitResponse\"\n"
							   "}\n"));
		}


		void ifdEstablishPaceChannelMessage()
		{
			const QSharedPointer<const IfdEstablishPaceChannel> message(new IfdEstablishPaceChannel(
					QStringLiteral("My little Reader"),
					QByteArray::fromHex("ABCD1234")));

			mChecker.receive(message);

			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"InputData\": \"abcd1234\",\n"
							   "    \"SlotHandle\": \"My little Reader\",\n"
							   "    \"msg\": \"IFDEstablishPACEChannel\"\n"
							   "}\n"));
		}


		void ifdEstablishPaceChannelMessageResponse()
		{
			const QSharedPointer<const IfdEstablishPaceChannelResponse> message(new IfdEstablishPaceChannelResponse(
					QStringLiteral("My little Reader"),
					QByteArray::fromHex("ABCD1234")));

			mChecker.receive(message);

			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"OutputData\": \"abcd1234\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"My little Reader\",\n"
							   "    \"msg\": \"IFDEstablishPACEChannelResponse\"\n"
							   "}\n"));
		}


		void ifdErrorMsg()
		{
			const QSharedPointer<const IfdError> message(new IfdError(QStringLiteral("NFC Reader"), QStringLiteral("/minorResult")));

			mChecker.receive(message);

			// Json
			const QJsonDocument document = message->toJson(QStringLiteral("TestContext"));
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 5);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("IFDError"));
			QCOMPARE(object.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(object.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(object.value(QLatin1String("ResultMajor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error"));
			QCOMPARE(object.value(QLatin1String("ResultMinor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
							   "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
							   "    \"SlotHandle\": \"NFC Reader\",\n"
							   "    \"msg\": \"IFDError\"\n"
							   "}\n"));
		}


	public:
		test_RemoteMessages()
			: mChecker()
		{
		}


};

QTEST_GUILESS_MAIN(test_RemoteMessages)
#include "test_RemoteMessages.moc"
