/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdEstablishPaceChannelResponse.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_IfdEstablishPaceChannelResponse
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void invalidJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			IfdEstablishPaceChannelResponse msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 8);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ResultMajor\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"ResultMinor\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Missing value \"SlotHandle\""));
			QVERIFY(logSpy.at(6).at(0).toString().contains("Missing value \"OutputData\""));
			QVERIFY(logSpy.at(7).at(0).toString().contains("The value of msg should be IFDEstablishPACEChannelResponse"));
		}


		void values()
		{
			const IfdEstablishPaceChannelResponse ifdEstablishPaceChannelResponse(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("abcd1234")
				);

			QVERIFY(!ifdEstablishPaceChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannelResponse.getType(), RemoteCardMessageType::IFDEstablishPACEChannelResponse);
			QCOMPARE(ifdEstablishPaceChannelResponse.getContextHandle(), QString());
			QCOMPARE(ifdEstablishPaceChannelResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdEstablishPaceChannelResponse.getOutputData(), QByteArray::fromHex("abcd1234"));
			QVERIFY(!ifdEstablishPaceChannelResponse.resultHasError());
			QCOMPARE(ifdEstablishPaceChannelResponse.getResultMinor(), ECardApiResult::Minor::null);
		}


		void toJson()
		{
			const IfdEstablishPaceChannelResponse ifdEstablishPaceChannelResponse(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("abcd1234")
				);

			const QByteArray& byteArray = ifdEstablishPaceChannelResponse.toByteArray(QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"OutputData\": \"abcd1234\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDEstablishPACEChannelResponse\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 6);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDEstablishPACEChannelResponse"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			QCOMPARE(obj.value(QLatin1String("OutputData")).toString(), QStringLiteral("abcd1234"));
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"OutputData\": \"abcd1234\",\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDEstablishPACEChannelResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannelResponse ifdEstablishPaceChannelResponse(obj);
			QVERIFY(!ifdEstablishPaceChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannelResponse.getType(), RemoteCardMessageType::IFDEstablishPACEChannelResponse);
			QCOMPARE(ifdEstablishPaceChannelResponse.getContextHandle(), QString("TestContext"));
			QCOMPARE(ifdEstablishPaceChannelResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdEstablishPaceChannelResponse.getOutputData(), QByteArray::fromHex("abcd1234"));
			QVERIFY(!ifdEstablishPaceChannelResponse.resultHasError());
			QCOMPARE(ifdEstablishPaceChannelResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 0);
		}


		void msgField_data()
		{
			QTest::addColumn<RemoteCardMessageType>("type");

			const auto& msgTypes = Enum<RemoteCardMessageType>::getList();
			for (const auto& type : msgTypes)
			{
				QTest::newRow(getEnumName(type).data()) << type;
			}
		}


		void msgField()
		{
			QFETCH(RemoteCardMessageType, type);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			QByteArray message("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"OutputData\": \"abcd1234\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdEstablishPaceChannelResponse ifdEstablishPaceChannelResponse(obj);

			if (type == RemoteCardMessageType::IFDEstablishPACEChannelResponse)
			{
				QVERIFY(!ifdEstablishPaceChannelResponse.isIncomplete());
				QCOMPARE(ifdEstablishPaceChannelResponse.getType(), RemoteCardMessageType::IFDEstablishPACEChannelResponse);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdEstablishPaceChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannelResponse.getType(), type);

			if (type == RemoteCardMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDEstablishPACEChannelResponse"));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDEstablishPACEChannelResponse"));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"OutputData\": 1,\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": 2,\n"
									 "    \"msg\": \"IFDEstablishPACEChannelResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishPaceChannelResponse ifdEstablishPaceChannelResponse(obj);
			QVERIFY(ifdEstablishPaceChannelResponse.isIncomplete());
			QCOMPARE(ifdEstablishPaceChannelResponse.getType(), RemoteCardMessageType::IFDEstablishPACEChannelResponse);
			QCOMPARE(ifdEstablishPaceChannelResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishPaceChannelResponse.getSlotHandle(), QString());
			QCOMPARE(ifdEstablishPaceChannelResponse.getOutputData(), QByteArray());
			QVERIFY(!ifdEstablishPaceChannelResponse.resultHasError());
			QCOMPARE(ifdEstablishPaceChannelResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotHandle\" should be of type \"string\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"OutputData\" should be of type \"string\""));
		}


};

QTEST_GUILESS_MAIN(test_IfdEstablishPaceChannelResponse)
#include "test_IfdEstablishPaceChannelResponse.moc"
