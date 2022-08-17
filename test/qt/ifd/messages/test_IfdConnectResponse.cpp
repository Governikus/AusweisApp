/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdConnectResponse.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_IfdConnectResponse
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
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			IfdConnectResponse msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 7);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ResultMajor\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"ResultMinor\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Missing value \"SlotHandle\""));
			QVERIFY(logSpy.at(6).at(0).toString().contains("The value of msg should be IFDConnectResponse"));
		}


		void values()
		{
			const IfdConnectResponse ifdConnectResponse(
				QStringLiteral("SlotHandle")
				);

			QVERIFY(!ifdConnectResponse.isIncomplete());
			QCOMPARE(ifdConnectResponse.getType(), IfdMessageType::IFDConnectResponse);
			QCOMPARE(ifdConnectResponse.getContextHandle(), QString());
			QCOMPARE(ifdConnectResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QVERIFY(!ifdConnectResponse.resultHasError());
			QCOMPARE(ifdConnectResponse.getResultMinor(), ECardApiResult::Minor::null);
		}


		void toJson()
		{
			const IfdConnectResponse ifdConnectResponse(
				QStringLiteral("SlotHandle")
				);

			const QByteArray& byteArray = ifdConnectResponse.toByteArray(IfdVersion::Version::v0, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDConnectResponse\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 5);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDConnectResponse"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			QCOMPARE(obj.value(QLatin1String("ResultMajor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"));
			QCOMPARE(obj.value(QLatin1String("ResultMinor")).toString(), QString());
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDConnectResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdConnectResponse ifdConnectResponse(obj);
			QVERIFY(!ifdConnectResponse.isIncomplete());
			QCOMPARE(ifdConnectResponse.getType(), IfdMessageType::IFDConnectResponse);
			QCOMPARE(ifdConnectResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdConnectResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QVERIFY(!ifdConnectResponse.resultHasError());
			QCOMPARE(ifdConnectResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 0);
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

			QByteArray message("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdConnectResponse ifdConnectResponse(obj);

			if (type == IfdMessageType::IFDConnectResponse)
			{
				QVERIFY(!ifdConnectResponse.isIncomplete());
				QCOMPARE(ifdConnectResponse.getType(), IfdMessageType::IFDConnectResponse);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdConnectResponse.isIncomplete());
			QCOMPARE(ifdConnectResponse.getType(), type);

			if (type == IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDConnectResponse"));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDConnectResponse"));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": 1,\n"
									 "    \"msg\": \"IFDConnectResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdConnectResponse ifdConnectResponse(obj);
			QVERIFY(ifdConnectResponse.isIncomplete());
			QCOMPARE(ifdConnectResponse.getType(), IfdMessageType::IFDConnectResponse);
			QCOMPARE(ifdConnectResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdConnectResponse.getSlotHandle(), QString());
			QVERIFY(!ifdConnectResponse.resultHasError());
			QCOMPARE(ifdConnectResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotHandle\" should be of type \"string\""));
		}


};

QTEST_GUILESS_MAIN(test_IfdConnectResponse)
#include "test_IfdConnectResponse.moc"
