/*!
 * \brief Unit tests for \ref Asn1StringUtil
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "asn1/ASN1Util.h"

#include <QtCore>
#include <QtTest>

#include <openssl/objects.h>
#include <openssl/x509v3.h>

using namespace governikus;


class test_Asn1StringUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void setValue()
		{
			ASN1_UTF8STRING* asn1String = ASN1_UTF8STRING_new();

			Asn1StringUtil::setValue(QString("abc"), asn1String);
			QCOMPARE(asn1String->length, 3);
			QCOMPARE(asn1String->data[0], (unsigned char) 'a');
			QCOMPARE(asn1String->data[1], (unsigned char) 'b');
			QCOMPARE(asn1String->data[2], (unsigned char) 'c');

			ASN1_STRING_free(asn1String);
		}


		void getValueFromNull()
		{
			QCOMPARE(Asn1StringUtil::getValue(nullptr), QString());
		}


		void getValue()
		{
			QString utf8Text("Dieß öst äin UTF8-Đext ɃϢݢૂૂૂ");
			ASN1_UTF8STRING* asn1String = ASN1_UTF8STRING_new();
			ASN1_STRING_set(asn1String, utf8Text.toUtf8().data(), utf8Text.toUtf8().length());

			QCOMPARE(Asn1StringUtil::getValue(asn1String), utf8Text);

			ASN1_STRING_free(asn1String);
		}


};

QTEST_GUILESS_MAIN(test_Asn1StringUtil)
#include "test_Asn1StringUtil.moc"
