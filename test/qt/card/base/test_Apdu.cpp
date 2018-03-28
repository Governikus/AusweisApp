/*!
 * \brief Unit tests for \ref Apdu
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "Apdu.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_Apdu
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void testRetryCounter0()
		{
			ResponseApdu apdu(QByteArray::fromHex("9000"));
			QCOMPARE(apdu.getRetryCounter(), 3);

			apdu.setBuffer(QByteArray::fromHex("63c2"));
			QCOMPARE(apdu.getRetryCounter(), 2);

			apdu.setBuffer(QByteArray::fromHex("63c1"));
			QCOMPARE(apdu.getRetryCounter(), 1);

			apdu.setBuffer(QByteArray::fromHex("63c0"));
			QCOMPARE(apdu.getRetryCounter(), 0);

			apdu.setBuffer(QByteArray::fromHex("6400"));
			QCOMPARE(apdu.getRetryCounter(), -1);

			apdu.setBuffer(QByteArray::fromHex("1234"));
			QCOMPARE(apdu.getRetryCounter(), -1);

			apdu.setBuffer(QByteArray::fromHex("12"));
			QCOMPARE(apdu.getRetryCounter(), -1);

			apdu.setBuffer(QByteArray());
			QCOMPARE(apdu.getRetryCounter(), -1);
		}


		void testReturnCode_data()
		{
			QTest::addColumn<StatusCode>("statusCode");
			QTest::addColumn<QByteArray>("bufferIn");
			QTest::addColumn<QByteArray>("bufferOut");

			QTest::newRow("empty") << StatusCode::EMPTY << QByteArray() << QByteArray::fromHex("0000");
			QTest::newRow("01") << StatusCode::INVALID << QByteArray::fromHex("01") << QByteArray::fromHex("0001");
			QTest::newRow("63c2") << StatusCode::PIN_RETRY_COUNT_2 << QByteArray::fromHex("63c2") << QByteArray::fromHex("63c2");
			QTest::newRow("6401") << StatusCode::INPUT_CANCELLED << QByteArray::fromHex("6401") << QByteArray::fromHex("6401");
			QTest::newRow("73c2") << StatusCode::INVALID << QByteArray::fromHex("73c2") << QByteArray::fromHex("0001");
		}


		void testReturnCode()
		{
			QFETCH(StatusCode, statusCode);
			QFETCH(QByteArray, bufferIn);
			QFETCH(QByteArray, bufferOut);

			ResponseApdu apdu = ResponseApdu(statusCode);
			QCOMPARE(apdu.getReturnCode(), statusCode);
			QCOMPARE(apdu.getBuffer(), bufferOut);

			apdu.setBuffer(bufferIn);
			QCOMPARE(apdu.getReturnCode(), statusCode);
			QCOMPARE(apdu.getBuffer(), bufferIn);
		}


};


QTEST_GUILESS_MAIN(test_Apdu)
#include "test_Apdu.moc"
