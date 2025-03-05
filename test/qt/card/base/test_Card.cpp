/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "VolatileSettings.h"

#include "Card.h"

#include <QtTest>


using namespace governikus;


class test_Card
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<VolatileSettings>()->setMessages(VolatileSettings::Messages(
					QStringLiteral("scan started"),
					QStringLiteral("scan failed"),
					QStringLiteral("scan succeeded"),
					QStringLiteral("scan progress")));
		}


		void test_generateProgressMessage_data()
		{
			QTest::addColumn<bool>("sdk");
			QTest::addColumn<QString>("message");
			QTest::addColumn<int>("progress");
			QTest::addColumn<QString>("expectedMessage");

			QTest::addRow("Message without Progress") << false << "specific message" << -1 << "specific message";
			QTest::addRow("Message with Progress") << false << "specific message" << 20 << "specific message\n20 %";
			QTest::addRow("No Message but Progress") << false << QString() << 30 << "30 %";

			QTest::addRow("SDK - Message without Progress") << true << "specific message" << -1 << "scan progress\n0 %";
			QTest::addRow("SDK - Message with Progress") << true << "specific message" << 20 << "scan progress\n20 %";
			QTest::addRow("SDK - No Message but Progress") << true << QString() << 30 << "scan progress\n30 %";
		}


		void test_generateProgressMessage()
		{
			QFETCH(bool, sdk);
			QFETCH(QString, message);
			QFETCH(int, progress);
			QFETCH(QString, expectedMessage);

			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(sdk);
			const auto result = Card::generateProgressMessage(message, progress);
			QCOMPARE(result, expectedMessage);
		}


		void test_generateErrorMessage_data()
		{
			QTest::addColumn<bool>("sdk");
			QTest::addColumn<QString>("message");
			QTest::addColumn<QString>("expectedMessage");

			QTest::addRow("Normal message") << false << "specific message" << "specific message";
			QTest::addRow("Null message") << false << QString() << QString();
			QTest::addRow("Empty message") << false << QStringLiteral("") << QStringLiteral("");

			QTest::addRow("SDK - Normal message") << true << "specific message" << "scan failed";
			QTest::addRow("SDK - Null message") << true << QString() << QString();
			QTest::addRow("SDK - Empty message") << true << QStringLiteral("") << "scan failed";
		}


		void test_generateErrorMessage()
		{
			QFETCH(bool, sdk);
			QFETCH(QString, message);
			QFETCH(QString, expectedMessage);

			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(sdk);
			const auto result = Card::generateErrorMessage(message);
			QCOMPARE(result, expectedMessage);
		}


};

QTEST_GUILESS_MAIN(test_Card)
#include "test_Card.moc"
