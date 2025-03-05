/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>


#include "pinpad/PinModifyOutput.h"


using namespace governikus;


class test_PinModifyOutput
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void check_data()
		{
			QTest::addColumn<QByteArray>("hexData");

			QTest::newRow("UNKNOWN1") << QByteArray("0200");
			QTest::newRow("UNKNOWN2") << QByteArray("6982");
			QTest::newRow("UNKNOWN3") << QByteArray("beef");
			QTest::newRow("INPUT_TIME_OUT") << QByteArray("6400");
			QTest::newRow("CANCELLATION_BY_USER") << QByteArray("6401");
			QTest::newRow("NEW_PIN_MISMATCH") << QByteArray("6402");
			QTest::newRow("NEW_PIN_INVALID_LENGTH") << QByteArray("6403");
			QTest::newRow("COMMAND_FAILED") << QByteArray("6a80");
			QTest::newRow("OK") << QByteArray("9000");

		}


		void check()
		{
			QFETCH(QByteArray, hexData);

			const auto data = QByteArray::fromHex(hexData);
			PinModifyOutput output((ResponseApdu(data)));

			QCOMPARE(output.getResponseApdu(), data);
			QCOMPARE(output.toCcid(), data);
		}


};

QTEST_GUILESS_MAIN(test_PinModifyOutput)
#include "test_PinModifyOutput.moc"
