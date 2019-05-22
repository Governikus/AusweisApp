/*!
 * \brief Unit tests for \ref SelfAuthenticationData
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthenticationData.h"

#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

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
