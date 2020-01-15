/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "messages/Discovery.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_Discovery
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

			Discovery msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 6);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be REMOTE_IFD"));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"IFDName\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"IFDID\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"port\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Missing value \"SupportedAPI\""));
		}


		void values()
		{
			const Discovery discovery(
				QStringLiteral("Sony Xperia Z5 compact"),
				QStringLiteral("0123456789ABCDEF"),
				24728,
				{IfdVersion::Version::v0, IfdVersion::Version::v_test}
				);

			QVERIFY(!discovery.isIncomplete());
			QCOMPARE(discovery.getType(), RemoteCardMessageType::UNDEFINED);
			QCOMPARE(discovery.getContextHandle(), QString());
			QCOMPARE(discovery.getIfdName(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(discovery.getIfdId(), QStringLiteral("0123456789ABCDEF"));
			QVERIFY(discovery.getPort() == static_cast<quint16>(24728));
			QCOMPARE(discovery.getSupportedApis(), QVector<IfdVersion::Version>({IfdVersion::Version::v0, IfdVersion::Version::v_test}));
		}


		void toJson()
		{
			const Discovery discovery(
				QStringLiteral("Sony Xperia Z5 compact"),
				QStringLiteral("0123456789ABCDEF"),
				24728,
				{IfdVersion::Version::v0, IfdVersion::Version::v_test}
				);

			const QByteArray& byteArray = discovery.toByteArray();
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"IFDID\": \"0123456789ABCDEF\",\n"
							   "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
							   "    \"SupportedAPI\": [\n"
							   "        \"IFDInterface_WebSocket_v0\",\n"
							   "        \"IFDInterface_WebSocket_v_test\"\n"
							   "    ],\n"
							   "    \"msg\": \"REMOTE_IFD\",\n"
							   "    \"port\": 24728\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 5);
			QCOMPARE(obj.value(QLatin1String("IFDName")).toString(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(obj.value(QLatin1String("IFDID")).toString(), QStringLiteral("0123456789ABCDEF"));
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("REMOTE_IFD"));
			QCOMPARE(obj.value(QLatin1String("port")).toInt(), 24728);
			const QJsonValue apiLevels = obj.value(QLatin1String("SupportedAPI"));
			QVERIFY(apiLevels.isArray());
			QCOMPARE(apiLevels.toArray().size(), 2);
			QCOMPARE(apiLevels.toArray().at(0).toString(), QStringLiteral("IFDInterface_WebSocket_v0"));
			QCOMPARE(apiLevels.toArray().at(1).toString(), QStringLiteral("IFDInterface_WebSocket_v_test"));
		}


		void fromJson()
		{
			const QByteArray message("{\n"
									 "    \"IFDID\": \"0123456789ABCDEF\",\n"
									 "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
									 "    \"SupportedAPI\": [\n"
									 "        \"IFDInterface_WebSocket_v0\",\n"
									 "        \"IFDInterface_WebSocket_v_test\"\n"
									 "    ],\n"
									 "    \"msg\": \"REMOTE_IFD\",\n"
									 "    \"port\": 24728\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(!discovery.isIncomplete());
			QCOMPARE(discovery.getType(), RemoteCardMessageType::UNDEFINED);
			QCOMPARE(discovery.getContextHandle(), QString());
			QCOMPARE(discovery.getIfdName(), QStringLiteral("Sony Xperia Z5 compact"));
			QCOMPARE(discovery.getIfdId(), QStringLiteral("0123456789ABCDEF"));
			QVERIFY(discovery.getPort() == static_cast<quint16>(24728));
			QCOMPARE(discovery.getSupportedApis(), QVector<IfdVersion::Version>({IfdVersion::Version::v0, IfdVersion::Version::v_test}));
		}


		void msgField_data()
		{
			QTest::addColumn<QByteArray>("type");

			const auto& msgTypes = Enum<RemoteCardMessageType>::getList();
			for (const auto& type : msgTypes)
			{
				QTest::newRow(getEnumName(type).data()) << QString(getEnumName(type)).toUtf8();
			}
		}


		void msgField()
		{
			QFETCH(QByteArray, type);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			QByteArray message("{\n"
							   "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
							   "    \"IFDID\": \"0123456789ABCDEF\",\n"
							   "    \"port\": 24728,\n"
							   "    \"SupportedAPI\": [\"IFDInterface_WebSocket_v0\", \"IFDInterface_WebSocket_v_test\"],\n"
							   "    \"msg\": \"%1\"\n"
							   "}");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", type)).object();
			const Discovery discovery(obj);

			QVERIFY(discovery.isIncomplete());
			QCOMPARE(discovery.getType(), RemoteCardMessageType::UNDEFINED);

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be REMOTE_IFD"));
		}


		void ignoreContext()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"IFDID\": \"0123456789ABCDEF\",\n"
									 "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
									 "    \"SupportedAPI\": [\n"
									 "        \"IFDInterface_WebSocket_v0\",\n"
									 "        \"IFDInterface_WebSocket_v_test\"\n"
									 "    ],\n"
									 "    \"msg\": \"REMOTE_IFD\",\n"
									 "    \"port\": 24728\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(!discovery.isIncomplete());
			QCOMPARE(discovery.getContextHandle(), QString());

			QCOMPARE(logSpy.count(), 0);
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"IFDID\": 1,\n"
									 "    \"IFDName\": 2,\n"
									 "    \"SupportedAPI\": \"IFDInterface_WebSocket_v0\",\n"
									 "    \"msg\": \"REMOTE_IFD\",\n"
									 "    \"port\": \"3\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(discovery.isIncomplete());
			QCOMPARE(discovery.getIfdName(), QString());
			QCOMPARE(discovery.getIfdId(), QString());
			QVERIFY(discovery.getPort() == 0);
			QCOMPARE(discovery.getSupportedApis(), QVector<IfdVersion::Version>());

			QCOMPARE(logSpy.count(), 4);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"IFDName\" should be of type \"string\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"IFDID\" should be of type \"string\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("The value of \"port\" should be of type \"number\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("The value of \"SupportedAPI\" should be of type \"array\""));
		}


		void wrongApiType()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"IFDID\": \"0123456789ABCDEF\",\n"
									 "    \"IFDName\": \"Sony Xperia Z5 compact\",\n"
									 "    \"SupportedAPI\": [\n"
									 "        0,\n"
									 "        \"IFDInterface_WebSocket_v_test\"\n"
									 "    ],\n"
									 "    \"msg\": \"REMOTE_IFD\",\n"
									 "    \"port\": 24728\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const Discovery discovery(obj);
			QVERIFY(discovery.isIncomplete());
			QCOMPARE(discovery.getSupportedApis(), QVector<IfdVersion::Version>({IfdVersion::Version::v_test}));

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SupportedAPI\" should be of type \"string array\""));
		}


};

QTEST_GUILESS_MAIN(test_Discovery)
#include "test_Discovery.moc"
