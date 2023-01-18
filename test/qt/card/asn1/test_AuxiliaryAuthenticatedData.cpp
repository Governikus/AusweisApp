/*!
 * \brief Unit tests for \ref AuxiliaryAuthenticatedData
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/ASN1Util.h"
#include "asn1/AuthenticatedAuxiliaryData.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_AuxiliaryAuthenticatedData
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseCrap()
		{
			QByteArray hexString("30 8202A4");
			auto auxData = AuthenticatedAuxiliaryData::fromHex(hexString);
			QVERIFY(auxData == nullptr);

			auxData = AuthenticatedAuxiliaryData::fromHex(QByteArray());
			QVERIFY(auxData == nullptr);
		}


		void empty()
		{
			QByteArray hexString("67 00");

			auto auxData = AuthenticatedAuxiliaryData::fromHex(hexString);

			QVERIFY(auxData != nullptr);
			QVERIFY(!auxData->hasAgeVerificationDate());
			QVERIFY(!auxData->hasCommunityID());
			QVERIFY(!auxData->hasValidityDate());
		}


		void ageVerification()
		{
			QByteArray hexString("67 17"
								 "73 15 "
								 "	06 09 04007F000703010401"
								 "	53 08 3139373830383136");

			auto auxData = AuthenticatedAuxiliaryData::fromHex(hexString);

			QVERIFY(auxData != nullptr);
			QVERIFY(auxData->hasAgeVerificationDate());
			QCOMPARE(auxData->getAgeVerificationDate(), QDate::fromString("1978-08-16", Qt::DateFormat::ISODate));
		}


		void ageVerification_twice()
		{
			QByteArray hexString("67 2E"
								 "73 15 "
								 "	06 09 04007F000703010401"
								 "	53 08 3139373830383136"
								 "73 15 "
								 "	06 09 04007F000703010401"
								 "	53 08 3139373830383136");

			auto auxData = AuthenticatedAuxiliaryData::fromHex(hexString);

			QVERIFY(auxData == nullptr);
		}


		void requiredAge_data()
		{
			// date of birth: 1978-08-16
			QTest::addColumn<QString>("age");
			QTest::addColumn<QDate>("effectiveDate");

			QTest::newRow("invalid") << QString() << QDate();

			QTest::newRow("vor der Geburt (1 Jahr, 2 Tage)") << QStringLiteral("-2") << QDate(1977, 8, 14);
			QTest::newRow("vor der Geburt (2 Tage)") << QStringLiteral("-1") << QDate(1978, 8, 14);
			QTest::newRow("vor der Geburt (1 Tag)") << QStringLiteral("-1") << QDate(1978, 8, 15);

			QTest::newRow("Geburt") << QStringLiteral("0") << QDate(1978, 8, 16);

			QTest::newRow("1 Tag") << QStringLiteral("0") << QDate(1978, 8, 17);
			QTest::newRow("Ende des Jahres") << QStringLiteral("0") << QDate(1978, 12, 31);
			QTest::newRow("Anfang naechstes Jahr") << QStringLiteral("0") << QDate(1979, 1, 1);

			QTest::newRow("vorm 1. Geburtstag") << QStringLiteral("0") << QDate(1979, 8, 15);
			QTest::newRow("1. Geburtstag") << QStringLiteral("1") << QDate(1979, 8, 16);
			QTest::newRow("nach 1. Geburtstag") << QStringLiteral("1") << QDate(1979, 8, 17);
			QTest::newRow("Ende Monats nach 1. Geburtstag") << QStringLiteral("1") << QDate(1979, 8, 31);
		}


		void requiredAge()
		{
			QFETCH(QString, age);
			QFETCH(QDate, effectiveDate);

			QByteArray hexString("67 17"
								 "73 15 "
								 "	06 09 04007F000703010401"
								 "	53 08 3139373830383136");

			auto auxData = AuthenticatedAuxiliaryData::fromHex(hexString);

			QVERIFY(auxData);
			QVERIFY(auxData->hasAgeVerificationDate());
			QCOMPARE(auxData->getRequiredAge(effectiveDate), age);
		}


		void communityID()
		{
			QByteArray hexString("67 16"
								 "73 14 "
								 "	06 09 04007F000703010403"
								 "	53 07 02760400110000");

			auto auxData = AuthenticatedAuxiliaryData::fromHex(hexString);

			QVERIFY(auxData != nullptr);
			QVERIFY(auxData->hasCommunityID());
			QCOMPARE(auxData->getCommunityID(), QByteArray("02760400110000"));
		}


		void communityID_twice()
		{
			QByteArray hexString("67 2C"
								 "73 14 "
								 "	06 09 04007F000703010403"
								 "	53 07 02760400110000"
								 "73 14 "
								 "	06 09 04007F000703010403"
								 "	53 07 02760400110000");

			auto auxData = AuthenticatedAuxiliaryData::fromHex(hexString);

			QVERIFY(auxData == nullptr);
		}


		void validityDate()
		{
			QByteArray hexString("67 17"
								 "73 15 "
								 "	06 09 04007F000703010402"
								 "	53 08 3230313530363330");

			auto auxData = AuthenticatedAuxiliaryData::fromHex(hexString);

			QVERIFY(auxData != nullptr);
			QVERIFY(auxData->hasValidityDate());
			QCOMPARE(auxData->getValidityDate(), QDate::fromString("2015-06-30", Qt::DateFormat::ISODate));
		}


		void validityDate_twice()
		{
			QByteArray hexString("67 2E"
								 "73 15 "
								 "	06 09 04007F000703010402"
								 "	53 08 3230313530363330"
								 "73 15 "
								 "	06 09 04007F000703010402"
								 "	53 08 3230313530363330");

			auto auxData = AuthenticatedAuxiliaryData::fromHex(hexString);

			QVERIFY(auxData == nullptr);
		}


		void unknown()
		{
			QByteArray hexString("67 10"
								 "73 0E "
								 "	06 0A 04007F00070301040909"
								 "	53 00 ");

			auto auxData = AuthenticatedAuxiliaryData::fromHex(hexString);

			QVERIFY(auxData == nullptr);
		}


		void allExtensions()
		{
			QByteArray hexString("67447315060904007F000703010401530831393937303633307315060904007F000703010402530832303135303633307314060904007F000703010403530702760400110000");

			auto auxData = AuthenticatedAuxiliaryData::fromHex(hexString);

			QVERIFY(auxData != nullptr);
			QVERIFY(auxData->hasAgeVerificationDate());
			QCOMPARE(auxData->getAgeVerificationDate(), QDate(1997, 06, 30));
			QVERIFY(auxData->hasCommunityID());
			QCOMPARE(auxData->getCommunityID(), QByteArray("02760400110000"));
			QVERIFY(auxData->hasValidityDate());
			QCOMPARE(auxData->getValidityDate(), QDate(2015, 06, 30));
		}


};

QTEST_GUILESS_MAIN(test_AuxiliaryAuthenticatedData)
#include "test_AuxiliaryAuthenticatedData.moc"
