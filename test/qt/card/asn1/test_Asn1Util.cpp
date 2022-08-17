/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
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
		void encode_data()
		{
			QTest::addColumn<int>("xclass");
			QTest::addColumn<int>("tag");
			QTest::addColumn<bool>("constructed");
			QTest::addColumn<int>("dataLen");
			QTest::addColumn<QByteArray>("header");

			QTest::newRow("Empty") << V_ASN1_UNIVERSAL << 0 << true << 0 << QByteArray::fromHex("2000");
			QTest::newRow("ShortLengthData") << V_ASN1_UNIVERSAL << 0 << true << 0x10 << QByteArray::fromHex("2010");
			QTest::newRow("ShortLengthMaxData") << V_ASN1_UNIVERSAL << 0 << true << 0x7F << QByteArray::fromHex("207F");
			QTest::newRow("ExtendedLengthData") << V_ASN1_UNIVERSAL << 0 << true << 0xFF << QByteArray::fromHex("2081FF");

			QTest::newRow("uni") << V_ASN1_UNIVERSAL << 1 << false << 1 << QByteArray::fromHex("0101");
			QTest::newRow("app") << V_ASN1_APPLICATION << 1 << false << 1 << QByteArray::fromHex("4101");
			QTest::newRow("con") << V_ASN1_CONTEXT_SPECIFIC << 1 << false << 1 << QByteArray::fromHex("8101");
			QTest::newRow("pri") << V_ASN1_PRIVATE << 1 << false << 1 << QByteArray::fromHex("C101");

			QTest::newRow("tag0") << V_ASN1_UNIVERSAL << 0 << false << 1 << QByteArray::fromHex("0001");
			QTest::newRow("tag1") << V_ASN1_UNIVERSAL << 1 << false << 1 << QByteArray::fromHex("0101");
			QTest::newRow("tag30") << V_ASN1_UNIVERSAL << 30 << false << 1 << QByteArray::fromHex("1E01");
			QTest::newRow("tag31") << V_ASN1_UNIVERSAL << 31 << false << 1 << QByteArray::fromHex("1F1F01");
			QTest::newRow("tag73") << V_ASN1_UNIVERSAL << 73 << false << 1 << QByteArray::fromHex("1F4901");

			QTest::newRow("primitive") << V_ASN1_UNIVERSAL << 1 << false << 1 << QByteArray::fromHex("0101");
			QTest::newRow("constructed") << V_ASN1_UNIVERSAL << 1 << true << 1 << QByteArray::fromHex("2101");
		}


		void encode()
		{
			QFETCH(int, xclass);
			QFETCH(int, tag);
			QFETCH(bool, constructed);
			QFETCH(int, dataLen);
			QFETCH(QByteArray, header);

			const auto data = QByteArray::fromHex("01").repeated(dataLen);
			const auto encoded = Asn1Util::encode(xclass, tag, data, constructed);
			QCOMPARE(encoded, header + data);
		}


};

QTEST_GUILESS_MAIN(test_Asn1Util)
#include "test_Asn1Util.moc"
