/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdDestroyPaceChannelResponse.h"

#include "LogHandler.h"
#include "TestFileHelper.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(IfdVersion::Version)


namespace
{
// https://bugreports.qt.io/browse/QTBUG-89363
inline QByteArray RESULT_OK()
{
	return QByteArray("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok");
}


} // namespace


class test_IfdDestroyPaceChannelResponse
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

			IfdDestroyPaceChannelResponse msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 7);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"msg\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ContextHandle\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ResultMajor\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"ResultMinor\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Missing value \"SlotHandle\"")));
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDDestroyPACEChannelResponse")));
		}


		void values()
		{
			const IfdDestroyPaceChannelResponse ifdDestroyPaceChannelResponse(QStringLiteral("SlotHandle"));

			QVERIFY(!ifdDestroyPaceChannelResponse.isIncomplete());
			QCOMPARE(ifdDestroyPaceChannelResponse.getType(), IfdMessageType::IFDDestroyPACEChannelResponse);
			QCOMPARE(ifdDestroyPaceChannelResponse.getContextHandle(), QString());
			QCOMPARE(ifdDestroyPaceChannelResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QVERIFY(!ifdDestroyPaceChannelResponse.resultHasError());
			QCOMPARE(ifdDestroyPaceChannelResponse.getResultMinor(), ECardApiResult::Minor::null);
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

			const IfdDestroyPaceChannelResponse ifdDestroyPaceChannelResponse(QStringLiteral("SlotHandle"));

			const QByteArray& byteArray = ifdDestroyPaceChannelResponse.toByteArray(version, QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDDestroyPACEChannelResponse\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 5);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDDestroyPACEChannelResponse"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			QCOMPARE(obj.value(QLatin1String("ResultMajor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"));
			QCOMPARE(obj.value(QLatin1String("ResultMinor")).toString(), QString());
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			QByteArray message(R"({
										"ContextHandle": "TestContext",
										"ResultMajor": "[OK]",
										"ResultMinor": null,
										"SlotHandle": "SlotHandle",
										"msg": "IFDDestroyPACEChannelResponse"
									 })");
			message.replace("[OK]", RESULT_OK());

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdDestroyPaceChannelResponse ifdDestroyPaceChannelResponse(obj);
			QVERIFY(!ifdDestroyPaceChannelResponse.isIncomplete());
			QCOMPARE(ifdDestroyPaceChannelResponse.getType(), IfdMessageType::IFDDestroyPACEChannelResponse);
			QCOMPARE(ifdDestroyPaceChannelResponse.getContextHandle(), "TestContext"_L1);
			QCOMPARE(ifdDestroyPaceChannelResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QVERIFY(!ifdDestroyPaceChannelResponse.resultHasError());
			QCOMPARE(ifdDestroyPaceChannelResponse.getResultMinor(), ECardApiResult::Minor::null);
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
									"ResultMajor": "[OK]",
									"ResultMinor": null,
									"SlotHandle": "SlotHandle",
									"msg": "%1"
							   })");
			message.replace("[OK]", RESULT_OK());

			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdDestroyPaceChannelResponse ifdDestroyPaceChannelResponse(obj);

			if (type == IfdMessageType::IFDDestroyPACEChannelResponse)
			{
				QVERIFY(!ifdDestroyPaceChannelResponse.isIncomplete());
				QCOMPARE(ifdDestroyPaceChannelResponse.getType(), IfdMessageType::IFDDestroyPACEChannelResponse);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdDestroyPaceChannelResponse.isIncomplete());
			QCOMPARE(ifdDestroyPaceChannelResponse.getType(), type);

			if (type == IfdMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("Invalid messageType received: \"UNDEFINED\"")));
				QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDDestroyPACEChannelResponse")));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("The value of msg should be IFDDestroyPACEChannelResponse")));
		}


};

QTEST_GUILESS_MAIN(test_IfdDestroyPaceChannelResponse)
#include "test_IfdDestroyPaceChannelResponse.moc"
