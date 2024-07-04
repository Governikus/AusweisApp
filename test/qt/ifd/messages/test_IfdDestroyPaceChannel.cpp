/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdDestroyPaceChannel.h"

#include "LogHandler.h"
#include "TestFileHelper.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(IfdVersion::Version)


class test_IfdDestroyPaceChannel
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

			IfdDestroyPaceChannel msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 5);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"msg\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ContextHandle\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"SlotHandle\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDDestroyPACEChannel")));
		}


		void values()
		{
			const IfdDestroyPaceChannel ifdDestroyPaceChannel(QStringLiteral("SlotHandle"));

			QVERIFY(!ifdDestroyPaceChannel.isIncomplete());
			QCOMPARE(ifdDestroyPaceChannel.getType(), IfdMessageType::IFDDestroyPACEChannel);
			QCOMPARE(ifdDestroyPaceChannel.getContextHandle(), QString());
			QCOMPARE(ifdDestroyPaceChannel.getSlotHandle(), QStringLiteral("SlotHandle"));
		}


		void toJson_data()
		{
			QTest::addColumn<IfdVersion::Version>("version");
			QTest::newRow("Unknown") << IfdVersion::Version::Unknown;
			QTest::newRow("v0") << IfdVersion::Version::v0;
			QTest::newRow("v2") << IfdVersion::Version::v2;
		}


		void toJson()
		{
			QFETCH(IfdVersion::Version, version);

			const IfdDestroyPaceChannel ifdDestroyPaceChannel(QStringLiteral("SlotHandle"));

			const QByteArray& byteArray = ifdDestroyPaceChannel.toByteArray(version, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDDestroyPACEChannel\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 3);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDDestroyPACEChannel"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
									"ContextHandle": "TestContext",
									"SlotHandle": "SlotHandle",
									"msg": "IFDDestroyPACEChannel"
								 })");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdDestroyPaceChannel ifdDestroyPaceChannel(obj);
			QVERIFY(!ifdDestroyPaceChannel.isIncomplete());
			QCOMPARE(ifdDestroyPaceChannel.getType(), IfdMessageType::IFDDestroyPACEChannel);
			QCOMPARE(ifdDestroyPaceChannel.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdDestroyPaceChannel.getSlotHandle(), QStringLiteral("SlotHandle"));
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
									"SlotHandle": "SlotHandle",
									"msg": "%1"
							   })");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdDestroyPaceChannel ifdDestroyPaceChannel(obj);

			if (type == IfdMessageType::IFDDestroyPACEChannel)
			{
				QVERIFY(!ifdDestroyPaceChannel.isIncomplete());
				QCOMPARE(ifdDestroyPaceChannel.getType(), IfdMessageType::IFDDestroyPACEChannel);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdDestroyPaceChannel.isIncomplete());
			QCOMPARE(ifdDestroyPaceChannel.getType(), type);

			if (type == IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"UNDEFINED\"")));
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDDestroyPACEChannel")));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDDestroyPACEChannel")));
		}


};

QTEST_GUILESS_MAIN(test_IfdDestroyPaceChannel)
#include "test_IfdDestroyPaceChannel.moc"
