/*!
 * \brief Unit tests for \ref Asn1BCDDateUtil
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/ASN1Util.h"

#include <QByteArray>
#include <QObject>
#include <QScopeGuard>
#include <QtTest>

#include <openssl/objects.h>
#include <openssl/x509v3.h>

using namespace governikus;


class test_Asn1BCDDateUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void convertQDateToUnpackedBCDAndBack()
		{
			QDate date = QDate(2015, 2, 3);
			ASN1_OCTET_STRING* strDate = ASN1_OCTET_STRING_new();
			const auto guard = qScopeGuard([strDate] {
						ASN1_STRING_free(strDate);
					});
			QByteArray array = Asn1BCDDateUtil::convertFromQDateToUnpackedBCD(date);
			QCOMPARE(array.toHex(), QByteArray("010500020003"));

			Asn1OctetStringUtil::setValue(array, strDate);
			QDate date2 = Asn1BCDDateUtil::convertFromUnpackedBCDToQDate(strDate);
			QCOMPARE(date2, QDate(2015, 2, 3));
		}


		void convertBCDToQDateInvalidLength()
		{
			ASN1_OCTET_STRING* strDate = ASN1_OCTET_STRING_new();
			const auto guard = qScopeGuard([strDate] {
						ASN1_STRING_free(strDate);
					});
			Asn1OctetStringUtil::setValue(QByteArray("0000000"), strDate);

			QDate date = Asn1BCDDateUtil::convertFromUnpackedBCDToQDate(strDate);
			QCOMPARE(date.isValid(), false);
		}


		void convertBCDToQDateNull()
		{
			QDate date = Asn1BCDDateUtil::convertFromUnpackedBCDToQDate(nullptr);
			QCOMPARE(date.isValid(), false);
		}


};

QTEST_GUILESS_MAIN(test_Asn1BCDDateUtil)
#include "test_Asn1BCDDateUtil.moc"
