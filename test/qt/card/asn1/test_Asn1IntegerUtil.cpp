/*!
 * \brief Unit tests for \ref Asn1IntegerUtil
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
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

			QCOMPARE(Asn1IntegerUtil::getValue(asn1Integer), QByteArray());
		}


		void getValue()
		{
			ASN1_INTEGER* asn1Integer = ASN1_INTEGER_new();

			QByteArray encoded = QByteArray::fromHex("02080123456789ABCDEF");
			const unsigned char* encodedPointer = reinterpret_cast<unsigned char*>(encoded.data());
			d2i_ASN1_INTEGER(&asn1Integer, &encodedPointer, encoded.size());

			QCOMPARE(Asn1IntegerUtil::getValue(asn1Integer).toHex().toUpper(), QByteArray("0123456789ABCDEF"));

			ASN1_INTEGER_free(asn1Integer);
		}


};

QTEST_GUILESS_MAIN(test_Asn1IntegerUtil)
#include "test_Asn1IntegerUtil.moc"
