/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdError.h"

#include "LogHandler.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_IfdError
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

			IfdError msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 7);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""_L1));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""_L1));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""_L1));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ResultMajor\""_L1));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"ResultMinor\""_L1));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Missing value \"SlotHandle\""_L1));
			QVERIFY(logSpy.at(6).at(0).toString().contains("The value of msg should be IFDError"_L1));
		}


		void values()
		{
			const IfdError ifdError(
				QStringLiteral("SlotHandle")
				);

			QVERIFY(!ifdError.isIncomplete());
			QCOMPARE(ifdError.getType(), IfdMessageType::IFDError);
			QCOMPARE(ifdError.getContextHandle(), QString());
			QCOMPARE(ifdError.getSlotHandle(), QStringLiteral("SlotHandle"));
			QVERIFY(!ifdError.resultHasError());
			QCOMPARE(ifdError.getResultMinor(), ECardApiResult::Minor::null);
		}


		void toJson()
		{
			const IfdError ifdError(
				QStringLiteral("SlotHandle")
				);

			const QByteArray& byteArray = ifdError.toByteArray(IfdVersion::Version::v0, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDError\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 5);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDError"));
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
									 "    \"msg\": \"IFDError\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdError ifdError(obj);
			QVERIFY(!ifdError.isIncomplete());
			QCOMPARE(ifdError.getType(), IfdMessageType::IFDError);
			QCOMPARE(ifdError.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdError.getSlotHandle(), QStringLiteral("SlotHandle"));
			QVERIFY(!ifdError.resultHasError());
			QCOMPARE(ifdError.getResultMinor(), ECardApiResult::Minor::null);

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
			const IfdError ifdError(obj);

			if (type == IfdMessageType::IFDError)
			{
				QVERIFY(!ifdError.isIncomplete());
				QCOMPARE(ifdError.getType(), IfdMessageType::IFDError);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdError.isIncomplete());
			QCOMPARE(ifdError.getType(), type);

			if (type == IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""_L1));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDError"_L1));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDError"_L1));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": 1,\n"
									 "    \"msg\": \"IFDError\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdError ifdError(obj);
			QVERIFY(ifdError.isIncomplete());
			QCOMPARE(ifdError.getType(), IfdMessageType::IFDError);
			QCOMPARE(ifdError.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdError.getSlotHandle(), QString());
			QVERIFY(!ifdError.resultHasError());
			QCOMPARE(ifdError.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotHandle\" should be of type \"string\""_L1));
		}


};

QTEST_GUILESS_MAIN(test_IfdError)
#include "test_IfdError.moc"
