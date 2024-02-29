/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref UIPlugInJsonApi
 */

#include "UIPlugInJson.h"

#include "LogHandler.h"

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


};

QTEST_GUILESS_MAIN(test_UIPlugInJson)
#include "test_UIPlugInJson.moc"
