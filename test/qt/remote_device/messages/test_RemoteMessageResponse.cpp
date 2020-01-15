/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "messages/RemoteMessageResponse.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_RemoteMessageResponse
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

			RemoteMessageResponse msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 5);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ResultMajor\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"ResultMinor\""));
		}


		void checkResult_data()
		{
			QTest::addColumn<ECardApiResult::Minor>("resultMinor");
			QTest::addColumn<bool>("hasError");

			QTest::newRow("empty") << ECardApiResult::Minor::null << false;
			QTest::newRow("result") << ECardApiResult::Minor::AL_Unknown_Error << true;
		}


		void checkResult()
		{
			QFETCH(ECardApiResult::Minor, resultMinor);
			QFETCH(bool, hasError);

			const RemoteMessageResponse remoteMessageResponse(
				RemoteCardMessageType::IFDEstablishContextResponse,
				resultMinor
				);

			QVERIFY(!remoteMessageResponse.isIncomplete());
			QCOMPARE(remoteMessageResponse.getType(), RemoteCardMessageType::IFDEstablishContextResponse);
			QCOMPARE(remoteMessageResponse.getContextHandle(), QString());
			QCOMPARE(remoteMessageResponse.resultHasError(), hasError);
			const ECardApiResult::Minor minor = (hasError ? ECardApiResult::Minor::AL_Unknown_Error : ECardApiResult::Minor::null);
			QCOMPARE(remoteMessageResponse.getResultMinor(), minor);
		}


		void fromJson_data()
		{
			QTest::addColumn<QByteArray>("message");
			QTest::addColumn<bool>("isIncomplete");
			QTest::addColumn<bool>("hasError");
			QTest::addColumn<ECardApiResult::Minor>("resultMinor");
			QTest::addColumn<int>("spyCount");

			QTest::newRow("withoutError") << QByteArray("{\n"
														"    \"ContextHandle\": \"TestContext\",\n"
														"    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
														"    \"ResultMinor\": null,\n"
														"    \"msg\": \"IFDEstablishContextResponse\"\n"
														"}\n") << false << false << ECardApiResult::Minor::null << 0;

			QTest::newRow("withError") << QByteArray("{\n"
													 "    \"ContextHandle\": \"TestContext\",\n"
													 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
													 "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownError\",\n"
													 "    \"msg\": \"IFDEstablishContextResponse\"\n"
													 "}\n") << false << true << ECardApiResult::Minor::AL_Unknown_Error << 0;

			QTest::newRow("WithoutErrorWithMinor") << QByteArray("{\n"
																 "    \"ContextHandle\": \"TestContext\",\n"
																 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
																 "    \"ResultMinor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownError\",\n"
																 "    \"msg\": \"IFDEstablishContextResponse\"\n"
																 "}\n") << false << false << ECardApiResult::Minor::null << 0;

			QTest::newRow("WithErrorWithoutMinor") << QByteArray("{\n"
																 "    \"ContextHandle\": \"TestContext\",\n"
																 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#error\",\n"
																 "    \"ResultMinor\": null,\n"
																 "    \"msg\": \"IFDEstablishContextResponse\"\n"
																 "}\n") << true << true << ECardApiResult::Minor::null << 1;
		}


		void fromJson()
		{
			QFETCH(QByteArray, message);
			QFETCH(bool, isIncomplete);
			QFETCH(bool, hasError);
			QFETCH(ECardApiResult::Minor, resultMinor);
			QFETCH(int, spyCount);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const RemoteMessageResponse remoteMessageResponse(obj);
			QCOMPARE(remoteMessageResponse.isIncomplete(), isIncomplete);
			QCOMPARE(remoteMessageResponse.getType(), RemoteCardMessageType::IFDEstablishContextResponse);
			QCOMPARE(remoteMessageResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(remoteMessageResponse.resultHasError(), hasError);
			QCOMPARE(remoteMessageResponse.getResultMinor(), resultMinor);

			QCOMPARE(logSpy.count(), spyCount);
			if (spyCount > 0)
			{
				QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"ResultMinor\" should be of type \"string\""));
			}
		}


};

QTEST_GUILESS_MAIN(test_RemoteMessageResponse)
#include "test_RemoteMessageResponse.moc"
