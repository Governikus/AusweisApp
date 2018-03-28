/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdModifyPinResponse.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_IfdModifyPinResponse
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			LogHandler::getInstance().init();
		}


		void invalidJson()
		{
			QSignalSpy logSpy(&LogHandler::getInstance(), &LogHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			IfdModifyPinResponse msg(obj);

			QCOMPARE(logSpy.count(), 6);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ResultMajor\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ResultMinor\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"SlotHandle\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Missing value \"OutputData\""));
		}


		void values()
		{
			const IfdModifyPinResponse ifdModifyPinResponse(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("abcd1234")
				);

			QCOMPARE(ifdModifyPinResponse.getType(), RemoteCardMessageType::IFDModifyPINResponse);
			QCOMPARE(ifdModifyPinResponse.getContextHandle(), QString());
			QCOMPARE(ifdModifyPinResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdModifyPinResponse.getOutputData(), QByteArray::fromHex("abcd1234"));
			QVERIFY(!ifdModifyPinResponse.resultHasError());
			QCOMPARE(ifdModifyPinResponse.getResultMinor(), QString());
		}


		void toJson()
		{
			const IfdModifyPinResponse ifdModifyPinResponse(
				QStringLiteral("SlotHandle"),
				QByteArray::fromHex("abcd1234")
				);

			const QJsonDocument& doc = ifdModifyPinResponse.toJson(QStringLiteral("TestContext"));
			QVERIFY(doc.isObject());
			QCOMPARE(doc.toJson(),
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"OutputData\": \"abcd1234\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"IFDModifyPINResponse\"\n"
							   "}\n"));

			const QJsonObject obj = doc.object();
			QCOMPARE(obj.size(), 6);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDModifyPINResponse"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotHandle")).toString(), QStringLiteral("SlotHandle"));
			QCOMPARE(obj.value(QLatin1String("OutputData")).toString(), QStringLiteral("abcd1234"));
		}


		void fromJson()
		{
			QSignalSpy logSpy(&LogHandler::getInstance(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"OutputData\": \"abcd1234\",\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": \"SlotHandle\",\n"
									 "    \"msg\": \"IFDModifyPINResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdModifyPinResponse ifdModifyPinResponse(obj);
			QCOMPARE(ifdModifyPinResponse.getType(), RemoteCardMessageType::IFDModifyPINResponse);
			QCOMPARE(ifdModifyPinResponse.getContextHandle(), QString("TestContext"));
			QCOMPARE(ifdModifyPinResponse.getSlotHandle(), QStringLiteral("SlotHandle"));
			QCOMPARE(ifdModifyPinResponse.getOutputData(), QByteArray::fromHex("abcd1234"));
			QVERIFY(!ifdModifyPinResponse.resultHasError());
			QCOMPARE(ifdModifyPinResponse.getResultMinor(), QString());

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

			QSignalSpy logSpy(&LogHandler::getInstance(), &LogHandler::fireLog);

			QByteArray message("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"OutputData\": \"abcd1234\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"SlotHandle\": \"SlotHandle\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdModifyPinResponse ifdModifyPinResponse(obj);

			if (type == RemoteCardMessageType::IFDModifyPINResponse)
			{
				QCOMPARE(ifdModifyPinResponse.getType(), RemoteCardMessageType::IFDModifyPINResponse);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdModifyPinResponse.isValid());
			QCOMPARE(ifdModifyPinResponse.getType(), type);

			QCOMPARE(logSpy.count(), 0);
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(&LogHandler::getInstance(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"OutputData\": 1,\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"SlotHandle\": 2,\n"
									 "    \"msg\": \"IFDModifyPINResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdModifyPinResponse ifdModifyPinResponse(obj);
			QCOMPARE(ifdModifyPinResponse.getType(), RemoteCardMessageType::IFDModifyPINResponse);
			QCOMPARE(ifdModifyPinResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdModifyPinResponse.getSlotHandle(), QString());
			QCOMPARE(ifdModifyPinResponse.getOutputData(), QByteArray());
			QVERIFY(!ifdModifyPinResponse.resultHasError());
			QCOMPARE(ifdModifyPinResponse.getResultMinor(), QString());

			QCOMPARE(logSpy.count(), 2);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotHandle\" should be of type \"string\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"OutputData\" should be of type \"string\""));
		}


};

QTEST_GUILESS_MAIN(test_IfdModifyPinResponse)
#include "test_IfdModifyPinResponse.moc"
