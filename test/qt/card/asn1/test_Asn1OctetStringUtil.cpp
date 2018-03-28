/*!
 * \brief Unit tests for \ref Asn1OctetStringUtil
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/ASN1Util.h"

#include <QtCore>
#include <QtTest>

#include <openssl/objects.h>
#include <openssl/x509v3.h>

using namespace governikus;


class test_Asn1OctetStringUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void getValueFromNull()
		{
			ASN1_OCTET_STRING* asn1OctetString = nullptr;

			QVERIFY(Asn1OctetStringUtil::getValue(asn1OctetString).isNull());
		}


		void getValue()
		{
			QByteArray bytes = QByteArray::fromHex("0102030405060708090a0b0c0d0e0f");
			ASN1_OCTET_STRING* asn1OctetString = ASN1_OCTET_STRING_new();
			ASN1_OCTET_STRING_set(asn1OctetString, reinterpret_cast<unsigned char*>(bytes.data()), bytes.length());

			QCOMPARE(Asn1OctetStringUtil::getValue(asn1OctetString), bytes);
		}


		void setValue()
		{
			QByteArray bytes = QByteArray::fromHex("0102030405060708090a0b0c0d0e0f");
			ASN1_OCTET_STRING* asn1OctetString = ASN1_OCTET_STRING_new();
			Asn1OctetStringUtil::setValue(bytes, asn1OctetString);

			QCOMPARE(asn1OctetString->length, 15);
			for (int i = 0; i < 15; i++)
			{
				QCOMPARE(asn1OctetString->data[i], uchar(i + 1));
			}
		}


};

QTEST_GUILESS_MAIN(test_Asn1OctetStringUtil)
#include "test_Asn1OctetStringUtil.moc"
