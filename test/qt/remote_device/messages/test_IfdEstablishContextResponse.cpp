/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "messages/IfdEstablishContextResponse.h"

#include "LogHandler.h"

#include <QtTest>


using namespace governikus;


class test_IfdEstablishContextResponse
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

			IfdEstablishContextResponse msg(obj);
			QVERIFY(msg.isIncomplete());

			QCOMPARE(logSpy.count(), 7);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Missing value \"msg\""));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Invalid messageType received: \"\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Missing value \"ContextHandle\""));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Missing value \"ResultMajor\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Missing value \"ResultMinor\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Missing value \"IFDName\""));
			QVERIFY(logSpy.at(6).at(0).toString().contains("The value of msg should be IFDEstablishContextResponse"));
		}


		void values()
		{
			const IfdEstablishContextResponse ifdEstablishContextResponse(
				QStringLiteral("IFD Remote Server")
				);

			QVERIFY(!ifdEstablishContextResponse.isIncomplete());
			QCOMPARE(ifdEstablishContextResponse.getType(), RemoteCardMessageType::IFDEstablishContextResponse);
			QCOMPARE(ifdEstablishContextResponse.getContextHandle(), QString());
			QCOMPARE(ifdEstablishContextResponse.getIfdName(), QStringLiteral("IFD Remote Server"));
			QVERIFY(!ifdEstablishContextResponse.resultHasError());
			QCOMPARE(ifdEstablishContextResponse.getResultMinor(), ECardApiResult::Minor::null);
		}


		void toJson()
		{
			const IfdEstablishContextResponse ifdEstablishContextResponse(
				QStringLiteral("IFD Remote Server")
				);

			const QByteArray& byteArray = ifdEstablishContextResponse.toByteArray(QStringLiteral("TestContext"));
			QCOMPARE(byteArray,
					QByteArray("{\n"
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"IFDName\": \"IFD Remote Server\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"msg\": \"IFDEstablishContextResponse\"\n"
							   "}\n"));

			const QJsonObject obj = QJsonDocument::fromJson(byteArray).object();
			QCOMPARE(obj.size(), 5);
			QCOMPARE(obj.value(QLatin1String("msg")).toString(), QStringLiteral("IFDEstablishContextResponse"));
			QCOMPARE(obj.value(QLatin1String("ContextHandle")).toString(), QStringLiteral("TestContext"));
			QCOMPARE(obj.value(QLatin1String("IFDName")).toString(), QStringLiteral("IFD Remote Server"));
			QCOMPARE(obj.value(QLatin1String("ResultMajor")).toString(), QStringLiteral("http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok"));
			QCOMPARE(obj.value(QLatin1String("ResultMinor")).toString(), QString());
		}


		void fromJson()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"IFDName\": \"IFD Remote Server\",\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"msg\": \"IFDEstablishContextResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishContextResponse ifdEstablishContextResponse(obj);
			QVERIFY(!ifdEstablishContextResponse.isIncomplete());
			QCOMPARE(ifdEstablishContextResponse.getType(), RemoteCardMessageType::IFDEstablishContextResponse);
			QCOMPARE(ifdEstablishContextResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishContextResponse.getIfdName(), QStringLiteral("IFD Remote Server"));
			QVERIFY(!ifdEstablishContextResponse.resultHasError());
			QCOMPARE(ifdEstablishContextResponse.getResultMinor(), ECardApiResult::Minor::null);

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
							   "    \"ContextHandle\": \"TestContext\",\n"
							   "    \"IFDName\": \"IFD Remote Server\",\n"
							   "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
							   "    \"ResultMinor\": null,\n"
							   "    \"msg\": \"%1\"\n"
							   "}\n");
			const QJsonObject& obj = QJsonDocument::fromJson(message.replace("%1", QTest::currentDataTag())).object();
			const IfdEstablishContextResponse ifdEstablishContextResponse(obj);

			if (type == RemoteCardMessageType::IFDEstablishContextResponse)
			{
				QVERIFY(!ifdEstablishContextResponse.isIncomplete());
				QCOMPARE(ifdEstablishContextResponse.getType(), RemoteCardMessageType::IFDEstablishContextResponse);

				QCOMPARE(logSpy.count(), 0);

				return;
			}

			QVERIFY(ifdEstablishContextResponse.isIncomplete());
			QCOMPARE(ifdEstablishContextResponse.getType(), type);

			if (type == RemoteCardMessageType::UNDEFINED)
			{
				QCOMPARE(logSpy.count(), 2);
				QVERIFY(logSpy.at(0).at(0).toString().contains("Invalid messageType received: \"UNDEFINED\""));
				QVERIFY(logSpy.at(1).at(0).toString().contains("The value of msg should be IFDEstablishContextResponse"));

				return;
			}

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of msg should be IFDEstablishContextResponse"));
		}


		void wrongTypes()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			const QByteArray message("{\n"
									 "    \"ContextHandle\": \"TestContext\",\n"
									 "    \"IFDName\": 1,\n"
									 "    \"ResultMajor\": \"http://www.bsi.bund.de/ecard/api/1.1/resultmajor#ok\",\n"
									 "    \"ResultMinor\": null,\n"
									 "    \"msg\": \"IFDEstablishContextResponse\"\n"
									 "}\n");

			const QJsonObject& obj = QJsonDocument::fromJson(message).object();
			const IfdEstablishContextResponse ifdEstablishContextResponse(obj);
			QVERIFY(ifdEstablishContextResponse.isIncomplete());
			QCOMPARE(ifdEstablishContextResponse.getType(), RemoteCardMessageType::IFDEstablishContextResponse);
			QCOMPARE(ifdEstablishContextResponse.getContextHandle(), QStringLiteral("TestContext"));
			QCOMPARE(ifdEstablishContextResponse.getIfdName(), QString());
			QVERIFY(!ifdEstablishContextResponse.resultHasError());
			QCOMPARE(ifdEstablishContextResponse.getResultMinor(), ECardApiResult::Minor::null);

			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("The value of \"IFDName\" should be of type \"string\""));
		}


};

QTEST_GUILESS_MAIN(test_IfdEstablishContextResponse)
#include "test_IfdEstablishContextResponse.moc"
