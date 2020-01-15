/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdStatus.h"

#include "AppSettings.h"
#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_IfdStatus
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void paceCapabilities_data()
		{
			QTest::addColumn<bool>("pace");
			QTest::addColumn<bool>("eid");
			QTest::addColumn<bool>("esign");
			QTest::addColumn<bool>("destroy");

			QTest::newRow("false") << false << false << false << false;
			QTest::newRow("true") << true << true << true << true;
		}


		void paceCapabilities()
		{
			QFETCH(bool, pace);
			QFETCH(bool, eid);
			QFETCH(bool, esign);
			QFETCH(bool, destroy);

			PaceCapabilities capabilities(pace, eid, esign, destroy);

			QCOMPARE(capabilities.getPace(), pace);
			QCOMPARE(capabilities.getEId(), eid);
			QCOMPARE(capabilities.getESign(), esign);
			QCOMPARE(capabilities.getDestroy(), destroy);
		}


		void invalidJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			QByteArray message("FooBar");
			const auto& obj = QJsonDocument::fromJson(message).object();
			QVERIFY(obj.isEmpty());

			IfdStatus msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 9);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"SlotName\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"PINCapabilities\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Missing value \"MaxAPDULength\""));
			QVERIFY(logSpy.at(6).at(0).toString().contains("Missing value \"ConnectedReader\""));
			QVERIFY(logSpy.at(7).at(0).toString().contains("Missing value \"CardAvailable\""));
			QVERIFY(logSpy.at(8).at(0).toString().contains("The value of msg should be IFDStatus"));
		}


		void values()
		{
			const IfdStatus ifdStatus(
				QStringLiteral("SlotName"),
				PaceCapabilities(true, false, true, false),
				500,
				false,
				false
				);

			QVERIFY(!ifdStatus.isIncomplete());
			QCOMPARE(ifdStatus.getType(), RemoteCardMessageType::IFDStatus);
			QCOMPARE(ifdStatus.getContextHandle(), QString());
			QCOMPARE(ifdStatus.getSlotName(), QStringLiteral("SlotName"));
			QVERIFY(ifdStatus.getPaceCapabilities().getPace());
			QVERIFY(!ifdStatus.getPaceCapabilities().getEId());
			QVERIFY(ifdStatus.getPaceCapabilities().getESign());
			QVERIFY(!ifdStatus.getPaceCapabilities().getDestroy());
			QCOMPARE(ifdStatus.getMaxApduLength(), 500);
			QVERIFY(!ifdStatus.getConnectedReader());
			QVERIFY(!ifdStatus.getCardAvailable());
		}


		void toJson()
		{
			const IfdStatus ifdStatus(
				QStringLiteral("SlotName"),
				PaceCapabilities(true, false, true, false),
				500,
				false,
				false
				);

			const QByteArray& byteArray = ifdStatus.toByteArray(QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"CardAvailable\": false,\n"
							   "    \"ConnectedReader\": false,\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"EFATR\": null,\n"
							   "    \"EFDIR\": null,\n"
							   "    \"MaxAPDULength\": 500,\n"
							   "    \"PINCapabilities\": {\n"
							   "        \"Destroy\": false,\n"
							   "        \"PACE\": true,\n"
							   "        \"eID\": false,\n"
							   "        \"eSign\": true\n"
							   "    },\n"
							   "    \"SlotName\": \"SlotName\",\n"
							   "    \"msg\": \"IFDStatus\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 9);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDStatus"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("SlotName")).toString(), QStringLiteral("SlotName"));
			const QJsonObject cap = obj.value(QLatin1String("PINCapabilities")).toObject();
			QCOMPARE(cap.size(), 4);
			QVERIFY(cap.value(QLatin1String("PACE")).toBool());
			QVERIFY(!cap.value(QLatin1String("eID")).toBool());
			QVERIFY(cap.value(QLatin1String("eSign")).toBool());
			QVERIFY(!cap.value(QLatin1String("Destroy")).toBool());
			QCOMPARE(obj.value(QLatin1String("MaxAPDULength")).toInt(), 500);
			QVERIFY(!obj.value(QLatin1String("ConnectedReader")).toBool());
			QVERIFY(!obj.value(QLatin1String("CardAvailable")).toBool());
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"CardAvailable\": false,\n"
									 "    \"ConnectedReader\": false,\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"EFATR\": null,\n"
									 "    \"EFDIR\": null,\n"
									 "    \"MaxAPDULength\": 500,\n"
									 "    \"PINCapabilities\": {\n"
									 "        \"Destroy\": false,\n"
									 "        \"PACE\": true,\n"
									 "        \"eID\": false,\n"
									 "        \"eSign\": true\n"
									 "    },\n"
									 "    \"SlotName\": \"SlotName\",\n"
									 "    \"msg\": \"IFDStatus\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdStatus ifdStatus(obj);
			QVERIFY(!ifdStatus.isIncomplete());
			QCOMPARE(ifdStatus.getType(), RemoteCardMessageType::IFDStatus);
			QCOMPARE(ifdStatus.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdStatus.getSlotName(), QStringLiteral("SlotName"));
			QVERIFY(ifdStatus.getPaceCapabilities().getPace());
			QVERIFY(!ifdStatus.getPaceCapabilities().getEId());
			QVERIFY(ifdStatus.getPaceCapabilities().getESign());
			QVERIFY(!ifdStatus.getPaceCapabilities().getDestroy());
			QCOMPARE(ifdStatus.getMaxApduLength(), 500);
			QVERIFY(!ifdStatus.getConnectedReader());
			QVERIFY(!ifdStatus.getCardAvailable());

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
							   "    \"CardAvailable\": false,\n"
							   "    \"ConnectedReader\": false,\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"EFATR\": null,\n"
							   "    \"EFDIR\": null,\n"
							   "    \"MaxAPDULength\": 500,\n"
							   "    \"PINCapabilities\": {\n"
							   "        \"Destroy\": false,\n"
							   "        \"PACE\": true,\n"
							   "        \"eID\": false,\n"
							   "        \"eSign\": true\n"
							   "    },\n"
							   "    \"SlotName\": \"SlotName\",\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdStatus ifdStatus(obj);

			if (type == RemoteCardMessageType::IFDStatus)
			{
				QVERIFY(!ifdStatus.isIncomplete());
				QCOMPARE(ifdStatus.getType(), RemoteCardMessageType::IFDStatus);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdStatus.isIncomplete());
			QCOMPARE(ifdStatus.getType(), type);

			if (type == RemoteCardMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDStatus"));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDStatus"));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"CardAvailable\": 1,\n"
									 "    \"ConnectedReader\": 2,\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"EFATR\": null,\n"
									 "    \"EFDIR\": null,\n"
									 "    \"MaxAPDULength\": \"3\",\n"
									 "    \"PINCapabilities\": 4,\n"
									 "    \"SlotName\": 5,\n"
									 "    \"msg\": \"IFDStatus\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdStatus ifdStatus(obj);
			QVERIFY(ifdStatus.isIncomplete());
			QCOMPARE(ifdStatus.getType(), RemoteCardMessageType::IFDStatus);
			QCOMPARE(ifdStatus.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdStatus.getSlotName(), QString());
			QVERIFY(!ifdStatus.getPaceCapabilities().getPace());
			QVERIFY(!ifdStatus.getPaceCapabilities().getEId());
			QVERIFY(!ifdStatus.getPaceCapabilities().getESign());
			QVERIFY(!ifdStatus.getPaceCapabilities().getDestroy());
			QCOMPARE(ifdStatus.getMaxApduLength(), 0);
			QVERIFY(!ifdStatus.getConnectedReader());
			QVERIFY(!ifdStatus.getCardAvailable());

			QCOMPARE(logSpy.count(), 5);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"SlotName\" should be of type \"string\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"PINCapabilities\" should be of type \"object\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("The value of \"MaxAPDULength\" should be of type \"number\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("The value of \"ConnectedReader\" should be of type \"boolean\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("The value of \"CardAvailable\" should be of type \"boolean\""));
		}


		void wrongPINCapabilityTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"CardAvailable\": false,\n"
									 "    \"ConnectedReader\": false,\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"EFATR\": null,\n"
									 "    \"EFDIR\": null,\n"
									 "    \"MaxAPDULength\": 500,\n"
									 "    \"PINCapabilities\": {\n"
									 "        \"Destroy\": \"1\",\n"
									 "        \"PACE\": \"2\",\n"
									 "        \"eID\": \"3\",\n"
									 "        \"eSign\": \"4\"\n"
									 "    },\n"
									 "    \"SlotName\": \"SlotName\",\n"
									 "    \"msg\": \"IFDStatus\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdStatus ifdStatus(obj);
			QVERIFY(ifdStatus.isIncomplete());
			QCOMPARE(ifdStatus.getType(), RemoteCardMessageType::IFDStatus);
			QCOMPARE(ifdStatus.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdStatus.getSlotName(), QStringLiteral("SlotName"));
			QVERIFY(!ifdStatus.getPaceCapabilities().getPace());
			QVERIFY(!ifdStatus.getPaceCapabilities().getEId());
			QVERIFY(!ifdStatus.getPaceCapabilities().getESign());
			QVERIFY(!ifdStatus.getPaceCapabilities().getDestroy());
			QCOMPARE(ifdStatus.getMaxApduLength(), 500);
			QVERIFY(!ifdStatus.getConnectedReader());
			QVERIFY(!ifdStatus.getCardAvailable());

			QCOMPARE(logSpy.count(), 4);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"PACE\" should be of type \"boolean\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("The value of \"eID\" should be of type \"boolean\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("The value of \"eSign\" should be of type \"boolean\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("The value of \"Destroy\" should be of type \"boolean\""));
		}


		void constructionWithReaderInfo_data()
		{
			QTest::addColumn<QString>("slotName");
			QTest::addColumn<bool>("isBasicReader");
			QTest::addColumn<int>("maxApduLength");
			QTest::addColumn<bool>("connected");
			QTest::addColumn<bool>("cardAvailable");
			QTest::addColumn<bool>("pinPadMode");

			QTest::newRow("Unconnected basic reader") << "BasicReader" << true << 500 << false << false << false;
			QTest::newRow("Connected basic reader without card") << "SlotName" << true << 500 << true << false << false;
			QTest::newRow("Connected basic reader with card") << "SlotName" << true << 500 << true << true << false;
			QTest::newRow("Connected basic reader with card without extended length") << "SlotName" << true << 200 << true << true << false;
			QTest::newRow("Connected basic reader with card and pin pad mode") << "SlotName" << true << 500 << true << true << true;
			QTest::newRow("Connected comfort reader with card and enabled pin pad mode") << "SlotName" << false << 500 << true << true << true;
			QTest::newRow("Connected comfort reader with card and disabled pin pad mode") << "SlotName" << false << 500 << true << true << false;
		}


		void constructionWithReaderInfo()
		{
			QFETCH(QString, slotName);
			QFETCH(bool, isBasicReader);
			QFETCH(int, maxApduLength);
			QFETCH(bool, connected);
			QFETCH(bool, cardAvailable);
			QFETCH(bool, pinPadMode);

			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(pinPadMode);

			const CardInfo cardInfo(cardAvailable ? CardType::EID_CARD : CardType::NONE);

			ReaderInfo readerInfo(slotName, ReaderManagerPlugInType::UNKNOWN, cardInfo);
			readerInfo.setBasicReader(isBasicReader);
			readerInfo.setMaxApduLength(maxApduLength);
			readerInfo.setConnected(connected);

			const IfdStatus ifdStatus(readerInfo);
			QVERIFY(!ifdStatus.isIncomplete());
			QCOMPARE(ifdStatus.getType(), RemoteCardMessageType::IFDStatus);
			QCOMPARE(ifdStatus.getContextHandle(), QString());
			QCOMPARE(ifdStatus.getSlotName(), slotName);
			QCOMPARE(ifdStatus.getPaceCapabilities().getPace(), !isBasicReader || pinPadMode);
			QVERIFY(!ifdStatus.getPaceCapabilities().getEId());
			QVERIFY(!ifdStatus.getPaceCapabilities().getESign());
			QVERIFY(!ifdStatus.getPaceCapabilities().getDestroy());
			QCOMPARE(ifdStatus.getMaxApduLength(), maxApduLength);
			QCOMPARE(ifdStatus.getConnectedReader(), connected);
			QCOMPARE(ifdStatus.getCardAvailable(), cardAvailable);
		}


};

QTEST_GUILESS_MAIN(test_IfdStatus)
#include "test_IfdStatus.moc"
