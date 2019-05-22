/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdEstablishContext.h"

#include "LogHandler.h"

#include <QtTest>


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
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			IfdEstablishContext msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 6);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"Protocol\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"UDName\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("The value of msg should be IFDEstablishContext"));
		}


		void values()
		{
			const IfdEstablishContext ifdEstablishContext(
				QStringLiteral("IFDInterface_WebSocket_v0"),
				QStringLiteral("MAC-MINI")
				);

			QVERIFY(!ifdEstablishContext.isIncomplete());
			QCOMPARE(ifdEstablishContext.getType(), RemoteCardMessageType::IFDEstablishContext);
			QCOMPARE(ifdEstablishContext.getContextHandle(), QString());
			QCOMPARE(ifdEstablishContext.getProtocol(), QStringLiteral("IFDInterface_WebSocket_v0"));
			QCOMPARE(ifdEstablishContext.getUdName(), QStringLiteral("MAC-MINI"));
		}


		void toJson()
		{
			const IfdEstablishContext ifdEstablishContext(
				QStringLiteral("IFDInterface_WebSocket_v0"),
				QStringLiteral("MAC-MINI")
				);

			const QByteArray& byteArray = ifdEstablishContext.toByteArray(QStringLiteral("TestContext"));
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
			QCOMPARE(ifdEstablishContext.getType(), RemoteCardMessageType::IFDEstablishContext);
			QCOMPARE(ifdEstablishContext.getContextHandle(), QString());
			QCOMPARE(ifdEstablishContext.getProtocol(), QStringLiteral("IFDInterface_WebSocket_v0"));
			QCOMPARE(ifdEstablishContext.getUdName(), QStringLiteral("MAC-MINI"));
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
							   "    \"Protocol\": \"IFDInterface_WebSocket_v0\",\n"
							   "    \"UDName\": \"MAC-MINI\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdEstablishContext ifdEstablishContext(obj);

			if (type == RemoteCardMessageType::IFDEstablishContext)
			{
				QVERIFY(!ifdEstablishContext.isIncomplete());
				QCOMPARE(ifdEstablishContext.getType(), RemoteCardMessageType::IFDEstablishContext);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdEstablishContext.isIncomplete());
			QCOMPARE(ifdEstablishContext.getType(), type);

			if (type == RemoteCardMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 3);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("Missing value \"ContextHandle\""));
				QVERIFY(logSpy.at(2).at(0).toString().contains("The value of msg should be IFDEstablishContext"));

				return;
			}

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDEstablishContext"));
		}


		void ignoreContext()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

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
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"Protocol\": 1,\n"
									 "    \"UDName\": 2,\n"
									 "    \"msg\": \"IFDEstablishContext\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishContext ifdEstablishContext(obj);
			QVERIFY(ifdEstablishContext.isIncomplete());

			QCOMPARE(ifdEstablishContext.getProtocol(), QString());
			QCOMPARE(ifdEstablishContext.getUdName(), QString());

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"Protocol\" should be of type \"string\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"UDName\" should be of type \"string\""));
		}


};

QTEST_GUILESS_MAIN(test_IfdEstablishContext)
#include "test_IfdEstablishContext.moc"
