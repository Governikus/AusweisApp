/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdDisconnectResponse.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_IfdDisconnectResponse
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

			IfdDisconnectResponse msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 7);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ResultMajor\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"ResultMinor\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Missing value \"SlotHandle\""));
			QVERIFY(logSpy.at(6).at(0).toString().contains("The value of msg should be IFDDisconnectResponse"));
		}


		void values()
		{
			const IfdDisconnectResponse ifdDisconnectResponse(
				QStringLiteral("SlotHandle")
				);

			QVERIFY(!ifdDisconnectResponse.isIncomplete());
			QCOMPARE(ifdDisconnectResponse.getType(), RemoteCardMessageType::IFDDisconnectResponse);
			QCOMPARE(ifdDisconnectResponse.getContextHandle(), QString());
			QCOMPARE(ifdDisconnectResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QVERIFY(!ifdDisconnectResponse.resultHasError());
			QCOMPARE(ifdDisconnectResponse.getResultMinor(), ECardApiResult::Minor::null);
		}


		void toJson()
		{
			const IfdDisconnectResponse ifdDisconnectResponse(
				QStringLiteral("SlotHandle")
				);

			const QByteArray& byteArray = ifdDisconnectResponse.toByteArray(IfdVersion::Version::v0, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDDisconnectResponse\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 5);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDDisconnectResponse"));
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
									 "    \"msg\": \"IFDDisconnectResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdDisconnectResponse ifdDisconnectResponse(obj);
			QVERIFY(!ifdDisconnectResponse.isIncomplete());
			QCOMPARE(ifdDisconnectResponse.getType(), RemoteCardMessageType::IFDDisconnectResponse);
			QCOMPARE(ifdDisconnectResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdDisconnectResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QVERIFY(!ifdDisconnectResponse.resultHasError());
			QCOMPARE(ifdDisconnectResponse.getResultMinor(), ECardApiResult::Minor::null);

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

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdDisconnectResponse ifdDisconnectResponse(obj);

			if (type == RemoteCardMessageType::IFDDisconnectResponse)
			{
				QVERIFY(!ifdDisconnectResponse.isIncomplete());
				QCOMPARE(ifdDisconnectResponse.getType(), RemoteCardMessageType::IFDDisconnectResponse);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdDisconnectResponse.isIncomplete());
			QCOMPARE(ifdDisconnectResponse.getType(), type);

			if (type == RemoteCardMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDDisconnectResponse"));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDDisconnectResponse"));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": 1,\n"
									 "    \"msg\": \"IFDDisconnectResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdDisconnectResponse ifdDisconnectResponse(obj);
			QVERIFY(ifdDisconnectResponse.isIncomplete());
			QCOMPARE(ifdDisconnectResponse.getType(), RemoteCardMessageType::IFDDisconnectResponse);
			QCOMPARE(ifdDisconnectResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdDisconnectResponse.getSlotHandle(), QString());
			QVERIFY(!ifdDisconnectResponse.resultHasError());
			QCOMPARE(ifdDisconnectResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotHandle\" should be of type \"string\""));
		}


};

QTEST_GUILESS_MAIN(test_IfdDisconnectResponse)
#include "test_IfdDisconnectResponse.moc"
