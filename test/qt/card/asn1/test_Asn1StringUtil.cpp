/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref Asn1StringUtil
 */

#include "asn1/ASN1Util.h"

#include <QtCore>
#include <QtTest>

#include <openssl/objects.h>
#include <openssl/x509v3.h>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_Asn1StringUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void setValue()
		{
			ASN1_UTF8STRING* asn1String = ASN1_UTF8STRING_new();

			Asn1StringUtil::setValue("abc"_L1, asn1String);
			QCOMPARE(asn1String->length, 3);
			QCOMPARE(asn1String->data[0], static_cast<uchar>('a'));
			QCOMPARE(asn1String->data[1], static_cast<uchar>('b'));
			QCOMPARE(asn1String->data[2], static_cast<uchar>('c'));

			ASN1_STRING_free(asn1String);
		}


		void getValueFromNull()
		{
			QCOMPARE(Asn1StringUtil::getValue(nullptr), QString());
		}


		void getValue()
		{
			const QString utf8Text(u"Dieß öst äin UTF8-Đext ɃϢݢૂૂૂ"_s);
			ASN1_UTF8STRING* asn1String = ASN1_UTF8STRING_new();
			ASN1_STRING_set(asn1String, utf8Text.toUtf8().data(), static_cast<int>(utf8Text.toUtf8().length()));

			QCOMPARE(Asn1StringUtil::getValue(asn1String), utf8Text);

			ASN1_STRING_free(asn1String);
		}


};

QTEST_GUILESS_MAIN(test_Asn1StringUtil)
#include "test_Asn1StringUtil.moc"
