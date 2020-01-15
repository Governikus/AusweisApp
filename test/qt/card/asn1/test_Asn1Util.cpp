/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/ASN1Util.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_Asn1Util
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void encodeEmptyData()
		{
			auto data = QByteArray();

			auto encoded = Asn1Util::encode(0x20, data);

			QCOMPARE(encoded, QByteArray::fromHex("2000"));
		}


		void encodeShortLengthData()
		{
			auto data = QByteArray::fromHex("01").repeated(0x10);

			auto encoded = Asn1Util::encode(0x20, data);

			QCOMPARE(encoded, QByteArray::fromHex("2010").append(data));
		}


		void encodeShortLengthMaxData()
		{
			auto data = QByteArray::fromHex("01").repeated(0x7F);

			auto encoded = Asn1Util::encode(0x20, data);

			QCOMPARE(encoded, QByteArray::fromHex("207F").append(data));
		}


		void encodeExtendedLengthData()
		{
			auto data = QByteArray::fromHex("01").repeated(0xFF);

			auto encoded = Asn1Util::encode(0x20, data);

			QCOMPARE(encoded, QByteArray::fromHex("2081FF").append(data));
		}


};

QTEST_GUILESS_MAIN(test_Asn1Util)
#include "test_Asn1Util.moc"
