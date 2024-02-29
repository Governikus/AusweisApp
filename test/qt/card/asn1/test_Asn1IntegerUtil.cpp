/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref Asn1IntegerUtil
 */

#include "asn1/ASN1Util.h"

#include <QtCore>
#include <QtTest>

#include <openssl/objects.h>
#include <openssl/x509v3.h>


using namespace governikus;


class test_Asn1IntegerUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void getValueFromNull()
		{
			ASN1_INTEGER* asn1Integer = nullptr;

			QCOMPARE(Asn1IntegerUtil::getValue(asn1Integer), -1);
		}


		void getValue_data()
		{
			QTest::addColumn<QByteArray>("data");
			QTest::addColumn<int>("number");

			QTest::newRow("-1") << QByteArray::fromHex("0201FF") << -1;
			QTest::newRow("0") << QByteArray::fromHex("020100") << 0;
			QTest::newRow("1") << QByteArray::fromHex("020101") << 1;
			QTest::newRow("127") << QByteArray::fromHex("02017F") << 127;
			QTest::newRow("128") << QByteArray::fromHex("02020080") << 128;
			QTest::newRow("256") << QByteArray::fromHex("02020100") << 256;
			QTest::newRow("32767") << QByteArray::fromHex("02027FFF") << 32767;
			QTest::newRow("32768") << QByteArray::fromHex("0203008000") << 32768;
		}


		void getValue()
		{
			QFETCH(QByteArray, data);
			QFETCH(int, number);

			const uchar* dataPointer = reinterpret_cast<uchar*>(data.data());
			ASN1_INTEGER* asn1Integer = d2i_ASN1_INTEGER(nullptr, &dataPointer, static_cast<long>(data.length()));
			QCOMPARE(Asn1IntegerUtil::getValue(asn1Integer), number);
			ASN1_INTEGER_free(asn1Integer);
		}


};

QTEST_GUILESS_MAIN(test_Asn1IntegerUtil)
#include "test_Asn1IntegerUtil.moc"
