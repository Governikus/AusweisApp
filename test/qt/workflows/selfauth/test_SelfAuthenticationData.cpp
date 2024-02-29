/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref SelfAuthenticationData
 */

#include "SelfAuthenticationData.h"

#include "TestFileHelper.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
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
		void testJsonResponse_data()
		{
			QTest::addColumn<QByteArray>("jsonData");
			QTest::addColumn<QByteArray>("errorMessage");

			QTest::addRow("Empty response") << QByteArray() << QByteArray();
			QTest::addRow("Empty JSON") << QByteArray("{}") << QByteArray("JSON value \"OperationsAllowedByUser\" is of wrong type: 128");
			QTest::addRow("Broken JSON") << QByteArray("abc") << QByteArray("JSON parsing failed: \"illegal value\"");
			QTest::addRow("Non-empty JSON") << QByteArray("{\"foo\": { \"baz\": \"bar\" }}") << QByteArray("JSON value \"OperationsAllowedByUser\" is of wrong type: 128");
			QTest::addRow("OperationsAllowedByUser present") << QByteArray("{\"OperationsAllowedByUser\": {}}") << QByteArray("JSON value \"PersonalData\" is of wrong type: 128");
		}


		void testJsonResponse()
		{
			QFETCH(QByteArray, jsonData);
			QFETCH(QByteArray, errorMessage);

			if (!errorMessage.isEmpty())
			{
				QTest::ignoreMessage(QtDebugMsg, errorMessage.constData());
			}
			SelfAuthenticationData selfAuthenticationData(jsonData);

			QVERIFY(!selfAuthenticationData.isValid());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfExpiry), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::GivenNames), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::Nationality), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::IssuingState), QString());
		}


		void dateFormat()
		{
			SelfAuthenticationData::OrderedSelfData expected;
			expected << qMakePair("Date of birth"_L1, "xx.xx.1946"_L1);
			expected << qMakePair("Date of expiry"_L1, "xx.11.2029"_L1);

			const auto& data = TestFileHelper::readFile(":/self/SelfAuthenticationDataDateFormat.json"_L1);
			const SelfAuthenticationData selfAuthenticationData(data);
			QVERIFY(selfAuthenticationData.isValid());
			QVERIFY(selfAuthenticationData.getDateTime().isValid());
			QCOMPARE(selfAuthenticationData.getOrderedSelfData(), expected);
		}


		void orderedSelfData()
		{
			SelfAuthenticationData::OrderedSelfData expected;
			expected << qMakePair("Family name"_L1, QStringLiteral("von Drebenbusch-Dalgo\u00DFen"));
			expected << qMakePair("Birth name"_L1, QStringLiteral("Wei\u00dF"));
			expected << qMakePair("Given name(s)"_L1, QStringLiteral("Hans-G\u00FCnther"));
			expected << qMakePair("Doctoral degree"_L1, "Dr.eh.Dr."_L1);
			expected << qMakePair("Date of birth"_L1, "25.01.1946"_L1);
			expected << qMakePair("Place of birth"_L1, "BREMERHAVEN"_L1);
			expected << qMakePair("Address"_L1, "WEG NR.12 8E"_L1);
			expected << qMakePair(QString(), "22043 HAMBURG"_L1);
			expected << qMakePair(QString(), "D"_L1);
			expected << qMakePair("Document type"_L1, "ID"_L1);
			expected << qMakePair("Nationality"_L1, "D"_L1);
			expected << qMakePair("Religious / artistic name"_L1, QStringLiteral("Freiherr zu M\u00F6ckern-Windensberg"));
			expected << qMakePair("Issuing country"_L1, "D"_L1);
			expected << qMakePair("Date of expiry"_L1, "30.11.2029"_L1);

			const auto& data = TestFileHelper::readFile(":/self/SelfAuthenticationDataID.json"_L1);
			SelfAuthenticationData selfAuthenticationData(data);
			QVERIFY(selfAuthenticationData.isValid());
			QVERIFY(selfAuthenticationData.getDateTime().isValid());
			QCOMPARE(selfAuthenticationData.getOrderedSelfData(), expected);
		}


		void parsedValues()
		{
			const auto& data = TestFileHelper::readFile(":/self/SelfAuthenticationData.json"_L1);
			SelfAuthenticationData selfAuthenticationData(data);
			QVERIFY(selfAuthenticationData.isValid());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfExpiry), "2020-10-31+01:00"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DocumentType), "TA"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::IssuingState), "D"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::Nationality), "AZE"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::GivenNames), QStringLiteral("ANDR\u00c9"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::FamilyNames), "MUSTERMANN"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::ArtisticName), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::AcademicTitle), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::BirthName), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfBirth), "1981-06-17+02:00"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfBirth), "FRANKFURT (ODER)"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceStreet), QStringLiteral("EHM-WELK-STRA\u00dfE 33"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCity), QStringLiteral("L\u00dcBBENAU/SPREEWALD"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCountry), "D"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceZipCode), "03222"_L1);
		}


		void parsedValuesNoAddress()
		{
			const auto& data = TestFileHelper::readFile(":/self/SelfAuthenticationDataNoAddress.json"_L1);
			SelfAuthenticationData selfAuthenticationData(data);
			QVERIFY(selfAuthenticationData.isValid());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DocumentType), "TP"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::IssuingState), "D"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::GivenNames), "KARL"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::FamilyNames), "HILLEBRANDT"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::ArtisticName), QStringLiteral("GRAF V. L\u00DDSKY"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::AcademicTitle), "DR.HC."_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::BirthName), "This data has not been stored in this chip generation."_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfBirth), "1952-06-17+01:00"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfBirth), "TRIER"_L1);

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceStreet), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCountry), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceZipCode), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceNoPlaceInfo), "keine Hauptwohnung in Deutschland"_L1);
		}


		void parsedValuesNoStreet()
		{
			const auto& data = TestFileHelper::readFile(":/self/SelfAuthenticationDataNoStreet.json"_L1);
			SelfAuthenticationData selfAuthenticationData(data);
			QVERIFY(selfAuthenticationData.isValid());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DocumentType), "TP"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::IssuingState), "D"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::GivenNames), "ANNEKATHRIN"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::FamilyNames), "LERCH"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::ArtisticName), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::AcademicTitle), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::BirthName), QString());
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::DateOfBirth), "1976-07-05+01:00"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfBirth), QStringLiteral("BAD K\u00D6NIGSHOFEN I. GRABFELD"));
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCity), QStringLiteral("L\u00DCBBENAU/SPREEWALD"));

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceStreet), QString());

			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceCountry), "D"_L1);
			QCOMPARE(selfAuthenticationData.getValue(SelfAuthData::PlaceOfResidenceZipCode), "06108"_L1);
		}


};

QTEST_GUILESS_MAIN(test_SelfAuthenticationData)
#include "test_SelfAuthenticationData.moc"
