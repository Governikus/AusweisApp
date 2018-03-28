/*!
 * \brief Unit tests for \ref SelfAuthenticationData
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthenticationData.h"
#include <QtTest>

using namespace governikus;

class test_SelfAuthenticationData
	: public QObject
{
	Q_OBJECT

	private:
		QFile selfAuthenticationDataXmlFile;
		QFile selfAuthenticationDataNoStreetXmlFile;
		QFile selfAuthenticationDataNoAddressXmlFile;

		void checkAndOpenFile(QFile& file)
		{
			QVERIFY(file.exists());
			QVERIFY(file.open(QIODevice::ReadOnly | QIODevice::Text));
		}


	public:
		test_SelfAuthenticationData()
			: selfAuthenticationDataXmlFile(":/self/SelfAuthenticationData.xml")
			, selfAuthenticationDataNoStreetXmlFile(":/self/SelfAuthenticationDataNoStreet.xml")
			, selfAuthenticationDataNoAddressXmlFile(":/self/SelfAuthenticationDataNoAddress.xml")
		{
			checkAndOpenFile(selfAuthenticationDataXmlFile);
			checkAndOpenFile(selfAuthenticationDataNoStreetXmlFile);
			checkAndOpenFile(selfAuthenticationDataNoAddressXmlFile);
		}


	private Q_SLOTS:
		void parsedValues()
		{
			SelfAuthenticationData selfAuthenticationData(selfAuthenticationDataXmlFile.readAll());
			QVERIFY(selfAuthenticationData.isValid());


			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DocumentType), QString("TP"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::IssuingState), QString("D"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::GivenNames), QString("ERIKA"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::FamilyNames), QString("MUSTERMANN"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::ArtisticName), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::AcademicTitle), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::BirthName), QString("GABLER"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfBirth), QString("1964-08-12+01:00"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfBirth), QString("BERLIN"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceStreet), QString("HEIDESTRASSE 17"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCity), QStringLiteral("K\u00D6LN"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCountry), QString("D"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceZipCode), QString("51147"));
		}


		void parsedValuesNoStreet()
		{
			SelfAuthenticationData selfAuthenticationData(selfAuthenticationDataNoStreetXmlFile.readAll());
			QVERIFY(selfAuthenticationData.isValid());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DocumentType), QString("TP"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::IssuingState), QString("D"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::GivenNames), QString("ANNEKATHRIN"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::FamilyNames), QString("LERCH"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::ArtisticName), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::AcademicTitle), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfBirth), QString("1976-07-05+01:00"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCity), QString("HALLE (SAALE)"));

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceStreet), QString());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCountry), QString("D"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceZipCode), QString("06108"));
		}


		void parsedValuesNoAddress()
		{
			SelfAuthenticationData selfAuthenticationData(selfAuthenticationDataNoAddressXmlFile.readAll());
			QVERIFY(selfAuthenticationData.isValid());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DocumentType), QString("TP"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::IssuingState), QString("D"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::GivenNames), QString("KARL"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::FamilyNames), QString("HILLEBRANDT"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::ArtisticName), QStringLiteral("GRAF V. L\u00DDSKY"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::AcademicTitle), QString("DR.HC."));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfBirth), QString("1952-06-17+01:00"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfBirth), QString("TRIER"));

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceStreet), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCountry), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceZipCode), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo), QString("keine Hauptwohnung in Deutschland"));
		}


		void tryToParseClosedFile()
		{
			selfAuthenticationDataXmlFile.close();
			SelfAuthenticationData selfAuthenticationData(selfAuthenticationDataXmlFile.readAll());
			QVERIFY(!selfAuthenticationData.isValid());

			selfAuthenticationDataNoStreetXmlFile.close();
			SelfAuthenticationData selfAuthenticationDataNoStreet(selfAuthenticationDataNoStreetXmlFile.readAll());
			QVERIFY(!selfAuthenticationDataNoStreet.isValid());

			selfAuthenticationDataNoAddressXmlFile.close();
			SelfAuthenticationData selfAuthenticationDataNoAddress(selfAuthenticationDataNoAddressXmlFile.readAll());
			QVERIFY(!selfAuthenticationDataNoAddress.isValid());
		}


};

QTEST_GUILESS_MAIN(test_SelfAuthenticationData)
#include "test_SelfAuthenticationData.moc"
