/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInJson.h"

#include "LogHandler.h"
#include "SmartCardDefinitions.h"

#include <QSignalSpy>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_UIPlugInJson
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
			UIPlugInJson api;
			connect(&api, &UIPlugInJson::fireMessage, this, [&result](const QByteArray& pMsg){result = pMsg;});
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
			const QByteArray knownCard(R"({"attached":true,"card":{"deactivated":false,)" + eidType + R"("inoperative":false,"retryCounter":-1},"insertable":false,"keypad":false,"msg":"READER","name":"Reader"})");

			QTest::addColumn<CardType>("cardType");
			QTest::addColumn<QByteArray>("result");

			QTest::newRow("NONE") << CardType::NONE << QByteArray();
			QTest::newRow("UNKNOWN") << CardType::UNKNOWN << QByteArray();
			QTest::newRow("EID_CARD") << CardType::EID_CARD << knownCard;
			QTest::newRow("SMART_EID") << CardType::SMART_EID << knownCard;
		}


		void newCard()
		{
			QFETCH(CardType, cardType);
			QFETCH(QByteArray, result);

			UIPlugInJson api;
			QSignalSpy spy(&api, &UIPlugInJson::fireMessage);

			ReaderInfo info(QStringLiteral("Reader"), ReaderManagerPlugInType::PCSC, CardInfo(cardType));
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


};

QTEST_GUILESS_MAIN(test_UIPlugInJson)
#include "test_UIPlugInJson.moc"
