/*!
 * \brief Unit tests for \ref Asn1TypeUtil
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/ASN1Util.h"

#include <QtTest>
#include <openssl/objects.h>
#include <openssl/x509v3.h>

using namespace governikus;


class test_Asn1TypeUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void encodeNull()
		{
			ASN1_TYPE* asn1Type = nullptr;

			QCOMPARE(Asn1TypeUtil::encode(asn1Type), QByteArray());
		}


		void encodeOctetString()
		{
			ASN1_TYPE* asn1Type = ASN1_TYPE_new();
			ASN1_OCTET_STRING* asn1OctetString = ASN1_OCTET_STRING_new();
			QByteArray octetBytes = QByteArray::fromHex("0123456789ABCDEF");
			ASN1_OCTET_STRING_set(asn1OctetString, reinterpret_cast<uchar*>(octetBytes.data()), octetBytes.length());
			ASN1_TYPE_set(asn1Type, V_ASN1_OCTET_STRING, asn1OctetString);

			QCOMPARE(Asn1TypeUtil::encode(asn1Type).toHex(), QByteArray("0408").append(octetBytes.toHex()));

			ASN1_TYPE_free(asn1Type);
			// ASN1_OCTET_STRING_free(asn1OctetString); no need to free, because it is owned and freed by ASN1_TYPE
		}


};

QTEST_GUILESS_MAIN(test_Asn1TypeUtil)
#include "test_Asn1TypeUtil.moc"
