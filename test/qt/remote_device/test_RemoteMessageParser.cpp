/*!
 * \brief Unit tests for \ref RemoteMessageParser
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "messages/RemoteMessageParser.h"

#include "messages/IfdConnectResponse.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmitResponse.h"
#include "messages/RemoteMessage.h"
#include "RemoteMessageChecker.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>


using namespace governikus;


class test_RemoteMessageParser
	: public QObject
{
	Q_OBJECT

	private:
		const RemoteMessageParser mParser;

		RemoteMessageChecker mChecker;

		void parseAndVerify(const char* pJsonByteData, bool isValid = true)
		{
			const QByteArray jsonData(pJsonByteData);
			const QSharedPointer<const RemoteMessage> message = mParser.parse(jsonData);

			QVERIFY(!message.isNull());
			mChecker.receive(message);
			QCOMPARE(message->isValid(), isValid);
		}


	private Q_SLOTS:
		void invalidJsonInput()
		{
			const QByteArray jsonData("bla bla");
			const QSharedPointer<const RemoteMessage> message = mParser.parse(jsonData);

			QVERIFY(message.isNull());
		}


		void discovery()
		{
			const QByteArray jsonData("{\n"
									  "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
									  "    \"IFDID\": \"0123456789ABCDEF\",\n"
									  "    \"msg\": \"REMOTE_IFD\",\n"
									  "    \"port\": 24728,\n"
									  "    \"SupportedAPI\": [\"IFDInterface_WebSocket_v0\", \"IFDInterface_WebSocket_v2\"]\n"
									  "}");

			QJsonParseError error;
			const QJsonDocument& doc = QJsonDocument::fromJson(jsonData, &error);
			QVERIFY(error.error == QJsonParseError::NoError);

			const QSharedPointer<const Discovery> message = mParser.parseDiscovery(doc);

			QVERIFY(!message.isNull());
			mChecker.processDiscovery(message);
		}


		void discovery_with_missing_msg_field()
		{
			const QByteArray jsonData("{\n"
									  "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
									  "    \"IFDID\": \"0123456789ABCDEF\",\n"
									  "    \"port\": 24728,\n"
									  "    \"SupportedAPI\": [\"IFDInterface_WebSocket_v0\", \"IFDInterface_WebSocket_v2\"]\n"
									  "}");

			QJsonParseError error;
			const QJsonDocument& doc = QJsonDocument::fromJson(jsonData, &error);
			QVERIFY(error.error == QJsonParseError::NoError);

			const QSharedPointer<const Discovery> message = mParser.parseDiscovery(doc);

			QVERIFY(message.isNull());
		}


		void discovery_with_invalid_msg_field()
		{
			const QByteArray jsonData("{\n"
									  "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
									  "    \"IFDID\": \"0123456789ABCDEF\",\n"
									  "    \"msg\": \"REMOTE\",\n"
									  "    \"port\": 24728,\n"
									  "    \"SupportedAPI\": [\"IFDInterface_WebSocket_v0\", \"IFDInterface_WebSocket_v2\"]\n"
									  "}");

			QJsonParseError error;
			const QJsonDocument& doc = QJsonDocument::fromJson(jsonData, &error);
			QVERIFY(error.error == QJsonParseError::NoError);

			const QSharedPointer<const Discovery> message = mParser.parseDiscovery(doc);

			QVERIFY(message.isNull());
		}


		void ifdEstablishContext()
		{
			parseAndVerify("{\n"
						   "    \"msg\": \"IFDEstablishContext\",\n"
						   "    \"Protocol\": \"IFDInterface_WebSocket_v0\",\n"
						   "	\"UDName\": \"MAC-MINI\"\n"
						   "}");
		}


		void ifdEstablishContextResponse()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"contextHandle\",\n"
						   "    \"IFDName\": \"IFD Remote Server\",\n"
						   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
						   "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
						   "    \"msg\": \"IFDEstablishContextResponse\"\n"
						   "}");
		}


		void getIfdStatus()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"contextHandle\",\n"
						   "    \"msg\": \"IFDGetStatus\",\n"
						   "    \"SlotName\": \"Remote Reader\"\n"
						   "}");
		}


		void ifdStatus()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"contextHandle\",\n"
						   "    \"msg\": \"IFDStatus\",\n"
						   "    \"ConnectedReader\": true,\n"
						   "    \"IFDName\": \"Remote Reader\",\n"
						   "    \"SlotName\": \"NFC Reader\",\n"
						   "    \"PINCapabilities\": {\n"
						   "        \"Destroy\": false,\n"
						   "        \"PACE\": false,\n"
						   "        \"eID\": false,\n"
						   "        \"eSign\": false\n"
						   "    },\n"
						   "    \"MaxAPDULength\": 500,\n"
						   "    \"CardAvailable\": false\n"
						   "}");
		}


		void ifdConnect()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"contextHandle\",\n"
						   "    \"exclusive\": true,\n"
						   "    \"msg\": \"IFDConnect\",\n"
						   "    \"SlotName\": \"NFC Reader\"\n"
						   "}");
		}


		void ifdConnectWrongContextHandleType()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": true,\n"
						   "    \"exclusive\": true,\n"
						   "    \"msg\": \"IFDConnect\",\n"
						   "    \"SlotName\": \"NFC Reader\"\n"
						   "}", false);
		}


		void ifdConnectMissingContextHandle()
		{
			parseAndVerify("{\n"
						   "    \"exclusive\": true,\n"
						   "    \"msg\": \"IFDConnect\",\n"
						   "    \"SlotName\": \"NFC Reader\"\n"
						   "}", false);
		}


		void ifdConnectWrongExclusvieType()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"contextHandle\",\n"
						   "    \"exclusive\": \"test\",\n"
						   "    \"msg\": \"IFDConnect\",\n"
						   "    \"SlotName\": \"NFC Reader\"\n"
						   "}", false);
		}


		void ifdConnectMissingExclusvie()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"contextHandle\",\n"
						   "    \"msg\": \"IFDConnect\",\n"
						   "    \"SlotName\": \"NFC Reader\"\n"
						   "}", false);
		}


		void ifdConnectResponse()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"contextHandle\",\n"
						   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
						   "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
						   "    \"msg\": \"IFDConnectResponse\",\n"
						   "    \"SlotHandle\": \"NFC Reader\"\n"
						   "}\n");
		}


		void ifdConnectResponseWrongContextHandleType()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": true,\n"
						   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
						   "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
						   "    \"msg\": \"IFDConnectResponse\",\n"
						   "    \"SlotHandle\": \"NFC Reader\"\n"
						   "}\n", false);
		}


		void ifdConnectResponseMissingContextHandle()
		{
			parseAndVerify("{\n"
						   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
						   "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
						   "    \"msg\": \"IFDConnectResponse\",\n"
						   "    \"SlotHandle\": \"NFC Reader\"\n"
						   "}\n", false);
		}


		void ifdConnectResponseMsg_nullError()
		{
			QByteArray jsonData("{\n"
								"    \"ContextHandle\": \"contextHandle\",\n"
								"    \"ResultMajor\": null,\n"
								"    \"ResultMinor\": \"Error message\",\n"
								"    \"msg\": \"IFDConnectResponse\",\n"
								"    \"SlotHandle\": \"NFC Reader\"\n"
								"}\n");

			const QSharedPointer<const RemoteMessage> message = mParser.parse(jsonData);

			QVERIFY(!message.isNull());
			QCOMPARE(message->getType(), RemoteCardMessageType::IFDConnectResponse);
			const auto& msg = message.staticCast<const IfdConnectResponse>();
			QCOMPARE(msg->resultHasError(), true);
			QCOMPARE(msg->getResultMinor(), QStringLiteral("Error message"));
			QCOMPARE(msg->getSlotHandle(), QStringLiteral("NFC Reader"));
			QVERIFY(!msg->isValid());
		}


		void ifdConnectResponseMsg_noError()
		{
			QByteArray jsonData("{\n"
								"    \"ContextHandle\": \"contextHandle\",\n"
								"    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
								"    \"ResultMinor\": null,\n"
								"    \"msg\": \"IFDConnectResponse\",\n"
								"    \"SlotHandle\": \"NFC Reader\"\n"
								"}\n");

			const QSharedPointer<const RemoteMessage> message = mParser.parse(jsonData);

			QVERIFY(!message.isNull());
			QCOMPARE(message->getType(), RemoteCardMessageType::IFDConnectResponse);
			const auto& msg = message.staticCast<const IfdConnectResponse>();
			QCOMPARE(msg->resultHasError(), false);
			QCOMPARE(msg->getResultMinor(), QString());
			QCOMPARE(msg->getSlotHandle(), QStringLiteral("NFC Reader"));
			QVERIFY(msg->isValid());
		}


		void ifdDisconnect()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"contextHandle\",\n"
						   "    \"SlotHandle\": \"NFC Reader\",\n"
						   "    \"msg\": \"IFDDisconnect\",\n"
						   "    \"readerName\": \"NFC Reader\"\n"
						   "}");
		}


		void ifdDisconnectResponse()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"contextHandle\",\n"
						   "    \"SlotHandle\": \"NFC Reader\",\n"
						   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
						   "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
						   "    \"msg\": \"IFDDisconnectResponse\"\n"
						   "}\n");
		}


		void ifdDisconnectResponse_nullError()
		{
			QByteArray jsonData("{\n"
								"    \"ContextHandle\": \"contextHandle\",\n"
								"    \"SlotHandle\": \"NFC Reader\",\n"
								"    \"ResultMajor\": null,\n"
								"    \"ResultMinor\": \"Error message\",\n"
								"    \"msg\": \"IFDDisconnectResponse\"\n"
								"}\n");

			const QSharedPointer<const RemoteMessage> message = mParser.parse(jsonData);

			QVERIFY(!message.isNull());
			QCOMPARE(message->getType(), RemoteCardMessageType::IFDDisconnectResponse);
			const auto& msg = message.staticCast<const IfdDisconnectResponse>();
			QCOMPARE(msg->resultHasError(), true);
			QCOMPARE(msg->getResultMinor(), QStringLiteral("Error message"));
			QCOMPARE(msg->getSlotHandle(), QStringLiteral("NFC Reader"));
			QVERIFY(!msg->isValid());
		}


		void ifdDisconnectResponse_noError()
		{
			QByteArray jsonData("{\n"
								"    \"ContextHandle\": \"contextHandle\",\n"
								"    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
								"    \"ResultMinor\": null,\n"
								"    \"SlotHandle\": \"NFC Reader\",\n"
								"    \"msg\": \"IFDDisconnectResponse\"\n"
								"}\n");

			const QSharedPointer<const RemoteMessage> message = mParser.parse(jsonData);

			QVERIFY(!message.isNull());
			QCOMPARE(message->getType(), RemoteCardMessageType::IFDDisconnectResponse);
			const auto& msg = message.staticCast<const IfdDisconnectResponse>();
			QCOMPARE(msg->resultHasError(), false);
			QCOMPARE(msg->getResultMinor(), QString());
			QCOMPARE(msg->getSlotHandle(), QStringLiteral("NFC Reader"));
		}


		void ifdTransmit()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"contextHandle\",\n"
						   "    \"msg\": \"IFDTransmit\",\n"
						   "    \"SlotHandle\": \"NFC Reader\",\n"
						   "    \"CommandAPDUs\": [\n"
						   "        {\n"
						   "            \"InputAPDU\": \"00A402022F00\",\n"
						   "            \"AcceptableStatusCodes\": null\n"
						   "        }\n"
						   "    ]\n"
						   "}");
		}


		void ifdTransmitWrongContextHandleType()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": true,\n"
						   "    \"msg\": \"IFDTransmit\",\n"
						   "    \"SlotHandle\": \"NFC Reader\",\n"
						   "    \"CommandAPDUs\": [\n"
						   "        {\n"
						   "            \"InputAPDU\": \"00A402022F00\",\n"
						   "            \"AcceptableStatusCodes\": null\n"
						   "        }\n"
						   "    ]\n"
						   "}", false);
		}


		void ifdTransmitMissingContextHandle()
		{
			parseAndVerify("{\n"
						   "    \"msg\": \"IFDTransmit\",\n"
						   "    \"SlotHandle\": \"NFC Reader\",\n"
						   "    \"CommandAPDUs\": [\n"
						   "        {\n"
						   "            \"InputAPDU\": \"00A402022F00\",\n"
						   "            \"AcceptableStatusCodes\": null\n"
						   "        }\n"
						   "    ]\n"
						   "}", false);
		}


		void transmitResponseMsg()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"contextHandle\",\n"
						   "    \"SlotHandle\": \"NFC Reader\",\n"
						   "    \"ResponseAPDUs\": [\n"
						   "		\"9000\"\n"
						   "    ],\n"
						   "    \"msg\": \"IFDTransmitResponse\",\n"
						   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
						   "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\"\n"
						   "}");
		}


		void transmitResponseMsg_nullError()
		{
			QByteArray jsonData("{\n"
								"    \"ContextHandle\": \"contextHandle\",\n"
								"    \"SlotHandle\": \"NFC Reader\",\n"
								"    \"ResponseAPDUs\": [\n"
								"		\"9000\"\n"
								"    ],\n"
								"    \"ResultMajor\": null,\n"
								"    \"ResultMinor\": \"Error message\",\n"
								"    \"msg\": \"IFDTransmitResponse\"\n"
								"}\n");

			const QSharedPointer<const RemoteMessage> message = mParser.parse(jsonData);

			QVERIFY(!message.isNull());
			QCOMPARE(message->getType(), RemoteCardMessageType::IFDTransmitResponse);
			const auto& msg = message.staticCast<const IfdTransmitResponse>();
			QCOMPARE(msg->resultHasError(), true);
			QCOMPARE(msg->getResultMinor(), QStringLiteral("Error message"));
			QCOMPARE(msg->getSlotHandle(), QStringLiteral("NFC Reader"));
			QCOMPARE(msg->getResponseApdu(), QByteArray::fromHex("9000"));
			QVERIFY(!msg->isValid());
		}


		void transmitResponseMsg_noError()
		{
			QByteArray jsonData("{\n"
								"    \"ContextHandle\": \"contextHandle\",\n"
								"    \"SlotHandle\": \"NFC Reader\",\n"
								"    \"ResponseAPDUs\": [\n"
								"		\"9000\"\n"
								"    ],\n"
								"    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
								"    \"ResultMinor\": null,\n"
								"    \"msg\": \"IFDTransmitResponse\"\n"
								"}\n");

			const QSharedPointer<const RemoteMessage> message = mParser.parse(jsonData);

			QVERIFY(!message.isNull());
			QVERIFY(message->isValid());
			QCOMPARE(message->getType(), RemoteCardMessageType::IFDTransmitResponse);
			const auto& msg = message.staticCast<const IfdTransmitResponse>();
			QCOMPARE(msg->resultHasError(), false);
			QCOMPARE(msg->getResultMinor(), QString());
			QCOMPARE(msg->getSlotHandle(), QStringLiteral("NFC Reader"));
			QCOMPARE(msg->getResponseApdu(), QByteArray::fromHex("9000"));
			QVERIFY(msg->isValid());
		}


		void transmitResponseMsg_noResponseApdu()
		{
			QByteArray jsonData("{\n"
								"    \"ContextHandle\": \"contextHandle\",\n"
								"    \"SlotHandle\": \"NFC Reader\",\n"
								"    \"ResultMajor\": \"/resultmajor#error\",\n"
								"    \"ResultMinor\": \"Error message\",\n"
								"    \"msg\": \"IFDTransmitResponse\"\n"
								"}\n");

			const QSharedPointer<const RemoteMessage> message = mParser.parse(jsonData);

			QVERIFY(!message.isNull());
			QVERIFY(!message->isValid());
			QCOMPARE(message->getType(), RemoteCardMessageType::IFDTransmitResponse);
			const auto& msg = message.staticCast<const IfdTransmitResponse>();
			QCOMPARE(msg->resultHasError(), true);
			QCOMPARE(msg->getResultMinor(), QStringLiteral("Error message"));
			QCOMPARE(msg->getSlotHandle(), QStringLiteral("NFC Reader"));
			QCOMPARE(msg->getResponseApdu(), QByteArray());
			QVERIFY(!msg->isValid());
		}


		void transmitResponseMsg_nullResponseApdu()
		{
			QByteArray jsonData("{\n"
								"    \"ContextHandle\": \"contextHandle\",\n"
								"    \"ResponseAPDUs\": null,\n"
								"    \"SlotHandle\": \"NFC Reader\",\n"
								"    \"ResultMajor\": \"/resultmajor#error\",\n"
								"    \"ResultMinor\": \"Error message\",\n"
								"    \"msg\": \"IFDTransmitResponse\"\n"
								"}\n");

			const QSharedPointer<const RemoteMessage> message = mParser.parse(jsonData);

			QVERIFY(!message.isNull());
			QCOMPARE(message->getType(), RemoteCardMessageType::IFDTransmitResponse);
			const auto& msg = message.staticCast<const IfdTransmitResponse>();
			QCOMPARE(msg->resultHasError(), true);
			QCOMPARE(msg->getResultMinor(), QStringLiteral("Error message"));
			QCOMPARE(msg->getSlotHandle(), QStringLiteral("NFC Reader"));
			QCOMPARE(msg->getResponseApdu(), QByteArray());
			QVERIFY(!msg->isValid());
		}


		void ifdEstablishPaceChannelMsg()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"TestContext\",\n"
						   "    \"InputData\": \"abcd1234\",\n"
						   "    \"SlotHandle\": \"My little Reader\",\n"
						   "    \"msg\": \"IFDEstablishPACEChannel\"\n"
						   "}\n");
		}


		void ifdEstablishPaceChannelMessageResponse()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"TestContext\",\n"
						   "    \"OutputData\": \"abcd1234\",\n"
						   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
						   "    \"ResultMinor\": null,\n"
						   "    \"SlotHandle\": \"My little Reader\",\n"
						   "    \"msg\": \"IFDEstablishPACEChannelResponse\"\n"
						   "}\n");
		}


		void ifdEstablishPaceChannelMessageResponseWithError()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"TestContext\",\n"
						   "    \"OutputData\": \"abcd1234\",\n"
						   "    \"ResultMajor\": \"/resultmajor#error\",\n"
						   "    \"ResultMinor\": \"/resultminor/ifdl/common#timeoutError\",\n"
						   "    \"SlotHandle\": \"My little Reader\",\n"
						   "    \"msg\": \"IFDEstablishPACEChannelResponse\"\n"
						   "}");
		}


		void ifdEstablishPaceChannelMessageResponseWithErrorMissingMinor()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"TestContext\",\n"
						   "    \"OutputData\": \"abcd1234\",\n"
						   "    \"ResultMajor\": \"/resultmajor#error\",\n"
						   "    \"SlotHandle\": \"My little Reader\",\n"
						   "    \"msg\": \"IFDEstablishPACEChannelResponse\"\n"
						   "}", false);

			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"TestContext\",\n"
						   "    \"OutputData\": \"abcd1234\",\n"
						   "    \"ResultMajor\": \"/resultmajor#error\",\n"
						   "    \"ResultMinor\": null,\n"
						   "    \"SlotHandle\": \"My little Reader\",\n"
						   "    \"msg\": \"IFDEstablishPACEChannelResponse\"\n"
						   "}\n", false);
		}


		void ifdError()
		{
			parseAndVerify("{\n"
						   "    \"ContextHandle\": \"TestContext\",\n"
						   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
						   "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/minorResult\",\n"
						   "    \"SlotHandle\": \"NFC Reader\",\n"
						   "    \"msg\": \"IFDError\"\n"
						   "}\n");
		}


	public:
		test_RemoteMessageParser()
			: mParser()
			, mChecker()
		{
		}


};

QTEST_GUILESS_MAIN(test_RemoteMessageParser)
#include "test_RemoteMessageParser.moc"
