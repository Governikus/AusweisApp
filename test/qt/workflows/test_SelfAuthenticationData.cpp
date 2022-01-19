/*!
 * \brief Unit tests for \ref SelfAuthenticationData
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthenticationData.h"

#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

char* toString(const SelfAuthenticationData::OrderedSelfData& pData)
{
	QString msg;
	QDebug dbg(&msg);
	dbg.noquote() << pData;
	return QTest::toString(msg);
}


class test_SelfAuthenticationData
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parsedCrap()
		{
			QTest::ignoreMessage(QtDebugMsg, "JSON parsing failed: \"illegal value\"");
			SelfAuthenticationData selfAuthenticationData("abc");
			QVERIFY(!selfAuthenticationData.isValid());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfExpiry), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::GivenNames), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::Nationality), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::IssuingState), QString());
		}


		void dateFormat()
		{
			SelfAuthenticationData::OrderedSelfData expected;
			expected << qMakePair(QString("Date of birth:"), QString("xx.xx.1946"));
			expected << qMakePair(QString("Date of expiry:"), QString("xx.11.2029"));

			const auto& data = TestFileHelper::readFile(":/self/SelfAuthenticationDataDateFormat.json");
			const SelfAuthenticationData selfAuthenticationData(data);
			QVERIFY(selfAuthenticationData.isValid());
			QVERIFY(selfAuthenticationData.getDateTime().isValid());
			QCOMPARE(selfAuthenticationData.getOrderedSelfData(), expected);
		}


		void orderedSelfData()
		{
			SelfAuthenticationData::OrderedSelfData expected;
			expected << qMakePair(QString("Family name:"), QStringLiteral("von Drebenbusch-Dalgo\u00DFen"));
			expected << qMakePair(QString("Birth name:"), QStringLiteral("Wei\u00dF"));
			expected << qMakePair(QString("Given name(s):"), QStringLiteral("Hans-G\u00FCnther"));
			expected << qMakePair(QString("Doctoral degree:"), QString("Dr.eh.Dr."));
			expected << qMakePair(QString("Date of birth:"), QString("25.01.1946"));
			expected << qMakePair(QString("Place of birth:"), QString("BREMERHAVEN"));
			expected << qMakePair(QString("Address:"), QString("WEG NR.12 8E"));
			expected << qMakePair(QString(), QString("22043 HAMBURG"));
			expected << qMakePair(QString(), QString("D"));
			expected << qMakePair(QString("Document type:"), QString("ID"));
			expected << qMakePair(QString("Nationality:"), QString("D"));
			expected << qMakePair(QString("Religious / artistic name:"), QStringLiteral("Freiherr zu M\u00F6ckern-Windensberg"));
			expected << qMakePair(QString("Issuing country:"), QString("D"));
			expected << qMakePair(QString("Date of expiry:"), QString("30.11.2029"));

			const auto& data = TestFileHelper::readFile(":/self/SelfAuthenticationDataID.json");
			SelfAuthenticationData selfAuthenticationData(data);
			QVERIFY(selfAuthenticationData.isValid());
			QVERIFY(selfAuthenticationData.getDateTime().isValid());
			QCOMPARE(selfAuthenticationData.getOrderedSelfData(), expected);
		}


		void parsedValues()
		{
			const auto& data = TestFileHelper::readFile(":/self/SelfAuthenticationData.json");
			SelfAuthenticationData selfAuthenticationData(data);
			QVERIFY(selfAuthenticationData.isValid());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfExpiry), QString("2020-10-31+01:00"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DocumentType), QString("TA"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::IssuingState), QString("D"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::Nationality), QString("AZE"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::GivenNames), QStringLiteral("ANDR\u00c9"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::FamilyNames), QString("MUSTERMANN"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::ArtisticName), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::AcademicTitle), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::BirthName), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfBirth), QString("1981-06-17+02:00"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfBirth), QString("FRANKFURT (ODER)"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceStreet), QStringLiteral("EHM-WELK-STRA\u00dfE 33"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCity), QStringLiteral("L\u00dcBBENAU/SPREEWALD"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCountry), QString("D"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceZipCode), QString("03222"));
		}


		void parsedValuesNoAddress()
		{
			const auto& data = TestFileHelper::readFile(":/self/SelfAuthenticationDataNoAddress.json");
			SelfAuthenticationData selfAuthenticationData(data);
			QVERIFY(selfAuthenticationData.isValid());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DocumentType), QString("TP"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::IssuingState), QString("D"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::GivenNames), QString("KARL"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::FamilyNames), QString("HILLEBRANDT"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::ArtisticName), QStringLiteral("GRAF V. L\u00DDSKY"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::AcademicTitle), QString("DR.HC."));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::BirthName), QString("This data has not been stored in this chip generation."));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfBirth), QString("1952-06-17+01:00"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfBirth), QString("TRIER"));

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceStreet), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCountry), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceZipCode), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo), QString("keine Hauptwohnung in Deutschland"));
		}


		void parsedValuesNoStreet()
		{
			const auto& data = TestFileHelper::readFile(":/self/SelfAuthenticationDataNoStreet.json");
			SelfAuthenticationData selfAuthenticationData(data);
			QVERIFY(selfAuthenticationData.isValid());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DocumentType), QString("TP"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::IssuingState), QString("D"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::GivenNames), QString("ANNEKATHRIN"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::FamilyNames), QString("LERCH"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::ArtisticName), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::AcademicTitle), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::BirthName), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfBirth), QString("1976-07-05+01:00"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfBirth), QStringLiteral("BAD K\u00D6NIGSHOFEN I. GRABFELD"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCity), QStringLiteral("L\u00DCBBENAU/SPREEWALD"));

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceStreet), QString());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCountry), QString("D"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceZipCode), QString("06108"));
		}


};

QTEST_GUILESS_MAIN(test_SelfAuthenticationData)
#include "test_SelfAuthenticationData.moc"
