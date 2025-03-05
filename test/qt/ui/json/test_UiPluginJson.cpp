/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginJson.h"

#include "LogHandler.h"
#include "ReaderManager.h"
#include "SmartCardDefinitions.h"

#include "MockReader.h"
#include "MockReaderManagerPlugin.h"

#include <QSignalSpy>
#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugin)

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_UiPluginJson
	: public QObject
{
	Q_OBJECT

	private:
		QJsonObject getJsonObject(const QByteArray& pData)
		{
			QJsonParseError jsonError {};
			const auto& json = QJsonDocument::fromJson(pData, &jsonError);
			if (jsonError.error == QJsonParseError::NoError)
			{
				return json.object();
			}

			return QJsonObject();
		}

	private Q_SLOTS:
		void initTestCase()
		{
			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->reset();
		}


		void log()
		{
			Env::getSingleton<LogHandler>()->init();
			const QLatin1String testDummy("Yeah, piece of cake!");
			qDebug() << testDummy;

			const QByteArray msg(R"({"cmd": "GET_LOG"})");
			QByteArray result;
			UiPluginJson api;
			connect(&api, &UiPluginJson::fireMessage, this, [&result](const QByteArray& pMsg){result = pMsg;});
			api.setEnabled();

			api.doMessageProcessing(msg);
			QVERIFY(!result.isEmpty());
			QVERIFY(result.contains(R"("msg":"LOG")"));
			auto json = getJsonObject(result);
			QVERIFY(!json.isEmpty());
			auto data = json["data"_L1].toString();
			QVERIFY(!data.isEmpty());

			const QLatin1String loggedType("Process type: GET_LOG");
			QCOMPARE(data.count(loggedType), 1);
			QCOMPARE(data.count(testDummy), 1);

			result.clear();
			api.doMessageProcessing(msg);
			QVERIFY(!result.isEmpty());
			json = getJsonObject(result);
			QVERIFY(!json.isEmpty());
			data = json["data"_L1].toString();
			QVERIFY(!data.isEmpty());
			QCOMPARE(data.count(loggedType), 2);
			QCOMPARE(data.count(testDummy), 1);
		}


		void newCard_data()
		{
#if __has_include("SmartManager.h")
			const QByteArray eidType = QByteArray(R"("eidType":"CARD_CERTIFIED",)");
#else
			const QByteArray eidType;
#endif
			const QByteArray unknownCard(R"({"attached":true,"card":{},"insertable":false,"keypad":false,"msg":"READER","name":"Reader"})");
			const QByteArray knownCard(R"({"attached":true,"card":{"deactivated":false,)" + eidType + R"("inoperative":false,"retryCounter":-1},"insertable":false,"keypad":false,"msg":"READER","name":"Reader"})");

			QTest::addColumn<MsgLevel>("msgLevel");
			QTest::addColumn<CardType>("cardType");
			QTest::addColumn<QByteArray>("result");

			QTest::newRow("NONE v2") << MsgLevel::v2 << CardType::NONE << QByteArray();
			QTest::newRow("NONE v3") << MsgLevel::v3 << CardType::NONE << QByteArray();
			QTest::newRow("UNKNOWN v2") << MsgLevel::v2 << CardType::UNKNOWN << QByteArray();
			QTest::newRow("UNKNOWN v3") << MsgLevel::v3 << CardType::UNKNOWN << unknownCard;
			QTest::newRow("EID_CARD v2") << MsgLevel::v2 << CardType::EID_CARD << knownCard;
			QTest::newRow("EID_CARD v3") << MsgLevel::v3 << CardType::EID_CARD << knownCard;
			QTest::newRow("SMART_EID v2") << MsgLevel::v2 << CardType::SMART_EID << knownCard;
			QTest::newRow("SMART_EID v3") << MsgLevel::v3 << CardType::SMART_EID << knownCard;
		}


		void newCard()
		{
			QFETCH(MsgLevel, msgLevel);
			QFETCH(CardType, cardType);
			QFETCH(QByteArray, result);

			UiPluginJson api;
			api.setEnabled(true);
			QByteArray setApiLevel(R"({"cmd": "SET_API_LEVEL", "level": <LEVEL>})");
			setApiLevel.replace("<LEVEL>", QByteArray::number(Enum<MsgLevel>::getValue(msgLevel)));
			api.doMessageProcessing(setApiLevel);

			QSignalSpy spy(&api, &UiPluginJson::fireMessage);
			ReaderInfo info(QStringLiteral("Reader"), ReaderManagerPluginType::PCSC, CardInfo(cardType));
			api.onCardInserted(info);
			if (result.isNull())
			{
				QCOMPARE(spy.size(), 0);
			}
			else
			{
				QCOMPARE(spy.size(), 1);
				QCOMPARE(spy.at(0).at(0).toByteArray(), result);
			}
		}


		void enabledOnce()
		{
			UiPluginJson api;
			api.setEnabled(true);
			api.setEnabled(true);
			api.setEnabled(false);
			api.setEnabled(false);
			api.setEnabled(true);

			QSignalSpy spy(&api, &UiPluginJson::fireMessage);

			MockReader* reader = MockReaderManagerPlugin::getInstance().addReader("MockReader 0815"_L1);
			reader->setCard(MockCardConfig());

			QCOMPARE(spy.size(), 2);
			const auto notAttached = R"({"attached":true,"card":null,"insertable":false,"keypad":false,"msg":"READER","name":"MockReader 0815"})";
#if __has_include("SmartManager.h")
			const QByteArray eidType = QByteArray(R"("eidType":"CARD_CERTIFIED",)");
#else
			const QByteArray eidType;
#endif
			const auto attached = R"({"attached":true,"card":{"deactivated":false,)" + eidType + R"("inoperative":false,"retryCounter":-1},"insertable":false,"keypad":false,"msg":"READER","name":"MockReader 0815"})";
			QCOMPARE(spy.at(0).at(0).toByteArray(), notAttached);
			QCOMPARE(spy.at(1).at(0).toByteArray(), attached);
		}


};

QTEST_GUILESS_MAIN(test_UiPluginJson)
#include "test_UiPluginJson.moc"
