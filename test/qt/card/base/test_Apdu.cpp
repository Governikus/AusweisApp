/*!
 * \brief Unit tests for \ref Apdu
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
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


		void testReturnCode()
		{
			ResponseApdu apdu = ResponseApdu(QByteArray());
			QCOMPARE(apdu.getReturnCode(), StatusCode::EMPTY);

			apdu.setBuffer(QByteArray::fromHex("01"));
			QCOMPARE(apdu.getReturnCode(), StatusCode::INVALID);

			apdu.setBuffer(QByteArray::fromHex("73c2"));
			QCOMPARE(apdu.getReturnCode(), StatusCode::INVALID);

			apdu.setBuffer(QByteArray::fromHex("63c2"));
			QCOMPARE(apdu.getReturnCode(), StatusCode::PIN_RETRY_COUNT_2);
		}


};


QTEST_GUILESS_MAIN(test_Apdu)
#include "test_Apdu.moc"
