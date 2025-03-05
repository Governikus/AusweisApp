/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdEstablishContext.h"

#include "LogHandler.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_IfdEstablishContext
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

			IfdEstablishContext msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 6);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""_L1));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""_L1));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""_L1));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"Protocol\""_L1));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"UDName\""_L1));
			QVERIFY(logSpy.at(5).at(0).toString().contains("The value of msg should be IFDEstablishContext"_L1));
		}


		void values()
		{
			const IfdEstablishContext ifdEstablishContext(
				IfdVersion::Version::v0,
				QStringLiteral("MAC-MINI")
				);

			QVERIFY(!ifdEstablishContext.isIncomplete());
			QCOMPARE(ifdEstablishContext.getType(), IfdMessageType::IFDEstablishContext);
			QCOMPARE(ifdEstablishContext.getContextHandle(), QString());
			QCOMPARE(ifdEstablishContext.getProtocol(), IfdVersion::Version::v0);
			QCOMPARE(ifdEstablishContext.getUdName(), QStringLiteral("MAC-MINI"));
		}


		void toJson()
		{
			const IfdEstablishContext ifdEstablishContext(
				IfdVersion::Version::v0,
				QStringLiteral("MAC-MINI")
				);

			const QByteArray& byteArray = ifdEstablishContext.toByteArray(IfdVersion::Version::v0, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"Protocol\": \"IFDInterface_WebSocket_v0\",\n"
							   "    \"UDName\": \"MAC-MINI\",\n"
							   "    \"msg\": \"IFDEstablishContext\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 3);
			QCOMPARE(obj.value(QLatin1String("Protocol")).toString(), QStringLiteral("IFDInterface_WebSocket_v0"));
			QCOMPARE(obj.value(QLatin1String("UDName")).toString(), QStringLiteral("MAC-MINI"));
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDEstablishContext"));
		}


		void fromJson()
		{
			const QByteArray message("{\n"
									 "    \"Protocol\": \"IFDInterface_WebSocket_v0\",\n"
									 "    \"UDName\": \"MAC-MINI\",\n"
									 "    \"msg\": \"IFDEstablishContext\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishContext ifdEstablishContext(obj);
			QVERIFY(!ifdEstablishContext.isIncomplete());
			QCOMPARE(ifdEstablishContext.getType(), IfdMessageType::IFDEstablishContext);
			QCOMPARE(ifdEstablishContext.getContextHandle(), QString());
			QCOMPARE(ifdEstablishContext.getProtocol(), IfdVersion::Version::v0);
			QCOMPARE(ifdEstablishContext.getUdName(), QStringLiteral("MAC-MINI"));
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
							   "    \"Protocol\": \"IFDInterface_WebSocket_v0\",\n"
							   "    \"UDName\": \"MAC-MINI\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdEstablishContext ifdEstablishContext(obj);

			if (type == IfdMessageType::IFDEstablishContext)
			{
				QVERIFY(!ifdEstablishContext.isIncomplete());
				QCOMPARE(ifdEstablishContext.getType(), IfdMessageType::IFDEstablishContext);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdEstablishContext.isIncomplete());
			QCOMPARE(ifdEstablishContext.getType(), type);

			if (type == IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 3);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""_L1));
				QVERIFY(logSpy.at(1).at(0).toString().contains("Missing value \"ContextHandle\""_L1));
				QVERIFY(logSpy.at(2).at(0).toString().contains("The value of msg should be IFDEstablishContext"_L1));

				return;
			}

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"ContextHandle\""_L1));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDEstablishContext"_L1));
		}


		void ignoreContext()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"Protocol\": \"IFDInterface_WebSocket_v0\",\n"
									 "    \"UDName\": \"MAC-MINI\",\n"
									 "    \"msg\": \"IFDEstablishContext\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishContext ifdEstablishContext(obj);
			QVERIFY(!ifdEstablishContext.isIncomplete());
			QCOMPARE(ifdEstablishContext.getContextHandle(), QString());

			QCOMPARE(logSpy.count(), 0);
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"Protocol\": 1,\n"
									 "    \"UDName\": 2,\n"
									 "    \"msg\": \"IFDEstablishContext\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishContext ifdEstablishContext(obj);
			QVERIFY(ifdEstablishContext.isIncomplete());

			QCOMPARE(ifdEstablishContext.getProtocol(), IfdVersion::Version::Unknown);
			QCOMPARE(ifdEstablishContext.getUdName(), QString());

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"Protocol\" should be of type \"string\""_L1));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"UDName\" should be of type \"string\""_L1));
		}


};

QTEST_GUILESS_MAIN(test_IfdEstablishContext)
#include "test_IfdEstablishContext.moc"
