/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
 */

#include "asn1/ASN1Util.h"

#include <QByteArray>
#include <QObject>
#include <QScopeGuard>
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
			QByteArray bytes = QByteArray::fromHex("0102030405060708090A0B0C0D0E0F");
			ASN1_OCTET_STRING* asn1OctetString = ASN1_OCTET_STRING_new();
			const auto guard = qScopeGuard([asn1OctetString] {
						ASN1_STRING_free(asn1OctetString);
					});
			ASN1_OCTET_STRING_set(asn1OctetString, reinterpret_cast<uchar*>(bytes.data()), static_cast<int>(bytes.length()));

			QCOMPARE(Asn1OctetStringUtil::getValue(asn1OctetString), bytes);
		}


		void setValue()
		{
			QByteArray bytes = QByteArray::fromHex("0102030405060708090A0B0C0D0E0F");
			ASN1_OCTET_STRING* asn1OctetString = ASN1_OCTET_STRING_new();
			const auto guard = qScopeGuard([asn1OctetString] {
						ASN1_STRING_free(asn1OctetString);
					});
			Asn1OctetStringUtil::setValue(bytes, asn1OctetString);

			QCOMPARE(ASN1_STRING_length(asn1OctetString), 15);
			const auto* data = ASN1_STRING_get0_data(asn1OctetString);
			for (int i = 0; i < 15; i++)
			{
				QCOMPARE(data[i], static_cast<uchar>(i + 1));
			}
		}


};

QTEST_GUILESS_MAIN(test_Asn1OctetStringUtil)
#include "test_Asn1OctetStringUtil.moc"
