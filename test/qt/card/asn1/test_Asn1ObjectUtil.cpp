/*!
 * \brief Unit tests for \ref Asn1ObjectUtil
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/ASN1Util.h"

#include <QtCore>
#include <QtTest>

#include <openssl/objects.h>
#include <openssl/x509v3.h>

using namespace governikus;


class test_Asn1ObjectUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseFromValidNumericString()
		{
			ASN1_OBJECT* asn1Object = Asn1ObjectUtil::parseFrom("0.4.0.128");

			QVERIFY(asn1Object != nullptr);

			QByteArray derEncoding = Asn1ObjectUtil::getValue(asn1Object);
			QCOMPARE(derEncoding.toHex(), QByteArray("04008100"));

			ASN1_OBJECT_free(asn1Object);
		}


		void parseFromInvalidNumericString()
		{
			ASN1_OBJECT* asn1Object = Asn1ObjectUtil::parseFrom("4711.4.0.128");

			QVERIFY(asn1Object == nullptr);
		}


		void parseFromInvalidString()
		{
			ASN1_OBJECT* asn1Object = Asn1ObjectUtil::parseFrom("md2WithRSAEncryption");

			QVERIFY(asn1Object == nullptr);
		}


		void convertToFromNull()
		{
			QCOMPARE(Asn1ObjectUtil::convertTo(nullptr), QByteArray(""));
		}


		void convertTo()
		{
			ASN1_OBJECT* asn1Object = Asn1ObjectUtil::parseFrom("1.2.840.113549.1.1.2");

			QCOMPARE(Asn1ObjectUtil::convertTo(asn1Object), QByteArray("1.2.840.113549.1.1.2"));

			ASN1_OBJECT_free(asn1Object);
		}


};

QTEST_GUILESS_MAIN(test_Asn1ObjectUtil)
#include "test_Asn1ObjectUtil.moc"
