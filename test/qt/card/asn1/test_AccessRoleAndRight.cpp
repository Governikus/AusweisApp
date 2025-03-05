/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/AccessRoleAndRight.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_AccessRoleAndRight
	: public QObject
{
	Q_OBJECT

	void fillTestData()
	{
		QTest::addColumn<AccessRight>("value");

		QTest::newRow("ResidencePermitII") << AccessRight::READ_DG20;
		QTest::newRow("ResidencePermitI") << AccessRight::READ_DG19;
		QTest::newRow("CommunityID") << AccessRight::READ_DG18;
		QTest::newRow("Address") << AccessRight::READ_DG17;
		QTest::newRow("BirthName") << AccessRight::READ_DG13;
		QTest::newRow("Nationality") << AccessRight::READ_DG10;
		QTest::newRow("PlaceOfBirth") << AccessRight::READ_DG09;
		QTest::newRow("DateOfBirth") << AccessRight::READ_DG08;
		QTest::newRow("DoctoralDegree") << AccessRight::READ_DG07;
		QTest::newRow("ArtisticName") << AccessRight::READ_DG06;
		QTest::newRow("FamilyName") << AccessRight::READ_DG05;
		QTest::newRow("GivenNames") << AccessRight::READ_DG04;
		QTest::newRow("ValidUntil") << AccessRight::READ_DG03;
		QTest::newRow("IssuingCountry") << AccessRight::READ_DG02;
		QTest::newRow("DocumentType") << AccessRight::READ_DG01;
		QTest::newRow("CanAllowed") << AccessRight::CAN_ALLOWED;
		QTest::newRow("Pseudonym") << AccessRight::RESTRICTED_IDENTIFICATION;
		QTest::newRow("AddressVerification") << AccessRight::COMMUNITY_ID_VERIFICATION;
		QTest::newRow("AgeVerification") << AccessRight::AGE_VERIFICATION;
		QTest::newRow("WriteAddress") << AccessRight::WRITE_DG17;
		QTest::newRow("WriteCommunityID") << AccessRight::WRITE_DG18;
		QTest::newRow("WriteResidencePermitI") << AccessRight::WRITE_DG19;
		QTest::newRow("WriteResidencePermitII") << AccessRight::WRITE_DG20;
		QTest::newRow("PinManagement") << AccessRight::PIN_MANAGEMENT;
	}

	private Q_SLOTS:
		void checkAccessRightNames_data()
		{
			fillTestData();
			QTest::newRow("") << AccessRight::WRITE_DG21;
			QTest::newRow("") << AccessRight::WRITE_DG22;
			QTest::newRow("") << AccessRight::PRIVILEGED_TERMINAL;
			QTest::newRow("") << AccessRight::PSA;
		}


		void checkAccessRightNames()
		{
			QFETCH(AccessRight, value);
			QCOMPARE(AccessRoleAndRightsUtil::toTechnicalName(value), QLatin1String(QTest::currentDataTag()));
		}


		void checkFromTechnicalName_data()
		{
			fillTestData();
		}


		void checkFromTechnicalName()
		{
			QFETCH(AccessRight, value);
			const auto undefined = static_cast<AccessRight>(UINT_MAX);
			AccessRight right = undefined;
			const auto& func = [&right](AccessRight pRight){
						right = pRight;
					};

			bool success = AccessRoleAndRightsUtil::fromTechnicalName(QString::fromLatin1(QTest::currentDataTag()), func);
			if (right != value)
			{
				QVERIFY(!success);
			}
			QCOMPARE(right, value);
			QVERIFY(success);

			QVERIFY(right != undefined);
		}


		void checkFromTechnicalNameInvalid()
		{
			const auto undefined = static_cast<AccessRight>(UINT_MAX);
			AccessRight right = undefined;
			const auto& func = [&right](AccessRight pRight){
						right = pRight;
					};

			QVERIFY(!AccessRoleAndRightsUtil::fromTechnicalName("crap"_L1, func));
			QCOMPARE(right, undefined);

			QVERIFY(!AccessRoleAndRightsUtil::fromTechnicalName("CAN_ALLOWED"_L1, func));
			QCOMPARE(right, undefined);

			QVERIFY(!AccessRoleAndRightsUtil::fromTechnicalName("familyname"_L1, func));
			QCOMPARE(right, undefined);
		}


		void checkJoinFromTechnicalName()
		{
			auto data = AccessRoleAndRightsUtil::joinFromTechnicalName({"DocumentType"_L1, "DoctoralDegree"_L1});
			QCOMPARE(data, QStringLiteral("Document type, Doctoral degree"));

			// previous versions stored translated display names instead of technical names, so be compatible here.
			data = AccessRoleAndRightsUtil::joinFromTechnicalName({"Alte Rechte"_L1, "FamilyName"_L1});
			QCOMPARE(data, QStringLiteral("Alte Rechte, Family name"));

			// do not add "Alte Rechte" for WRITE only rights as previous version had no WRITE.
			data = AccessRoleAndRightsUtil::joinFromTechnicalName({"Alte Rechte"_L1, "FamilyName"_L1}, AccessRoleAndRightsUtil::WRITE);
			QCOMPARE(data, QString());

			data = AccessRoleAndRightsUtil::joinFromTechnicalName({"Alte Rechte"_L1, "WriteCommunityID"_L1}, AccessRoleAndRightsUtil::WRITE);
			QCOMPARE(data, QStringLiteral("Community-ID"));

			const auto right = AccessRoleAndRightsUtil::JoinRights(AccessRoleAndRightsUtil::READ | AccessRoleAndRightsUtil::WRITE);
			data = AccessRoleAndRightsUtil::joinFromTechnicalName({"DocumentType"_L1, "WriteAddress"_L1}, right);
			QCOMPARE(data, QStringLiteral("Document type, Address"));

			data = AccessRoleAndRightsUtil::joinFromTechnicalName({"DocumentType"_L1, "WriteAddress"_L1}, AccessRoleAndRightsUtil::READWRITE);
			QCOMPARE(data, QStringLiteral("Document type, Address"));

			data = AccessRoleAndRightsUtil::joinFromTechnicalName({"DocumentType"_L1, "WriteAddress"_L1}, AccessRoleAndRightsUtil::WRITE);
			QCOMPARE(data, QStringLiteral("Address"));

			data = AccessRoleAndRightsUtil::joinFromTechnicalName({"DocumentType"_L1, "WriteAddress"_L1}, AccessRoleAndRightsUtil::READ);
			QCOMPARE(data, QStringLiteral("Document type"));
		}


		/**
		 * We use UINT_MAX as "undefined" state in AccessRoleAndRightsUtil::fromTechnicalName.
		 * Ensure that we never use UINT_MAX as a valid value!
		 */
		void checkUndefinedUsage()
		{
			const auto& names = Enum<AccessRightNames>::getList();
			for (AccessRightNames entry : names)
			{
				QVERIFY(entry != static_cast<AccessRightNames>(UINT_MAX));
			}

			const auto& rights = Enum<AccessRight>::getList();
			for (AccessRight entry : rights)
			{
				QVERIFY(entry != static_cast<AccessRight>(UINT_MAX));
			}
		}


		void test_ToDisplayText_data()
		{
			QTest::addColumn<AccessRight>("input");
			QTest::addColumn<QLatin1String>("output");

			QTest::newRow("RFU") << AccessRight::RFU << "Unknown (reserved)"_L1;

			QTest::newRow("writeDG17") << AccessRight::WRITE_DG17 << "Address"_L1;
			QTest::newRow("writeDG18") << AccessRight::WRITE_DG18 << "Community-ID"_L1;
			QTest::newRow("writeDG19") << AccessRight::WRITE_DG19 << "Residence permit I"_L1;
			QTest::newRow("writeDG20") << AccessRight::WRITE_DG20 << "Residence permit II"_L1;
			QTest::newRow("writeDG21") << AccessRight::WRITE_DG21 << "Optional data"_L1;
			QTest::newRow("writeDG22") << AccessRight::WRITE_DG22 << "Optional data"_L1;
			QTest::newRow("readDG22") << AccessRight::READ_DG22 << "Optional data"_L1;
			QTest::newRow("readDG21") << AccessRight::READ_DG21 << "Optional data"_L1;
			QTest::newRow("readDG20") << AccessRight::READ_DG20 << "Residence permit II"_L1;
			QTest::newRow("readDG19") << AccessRight::READ_DG19 << "Residence permit I"_L1;
			QTest::newRow("readDG18") << AccessRight::READ_DG18 << "Community-ID"_L1;
			QTest::newRow("readDG17") << AccessRight::READ_DG17 << "Address"_L1;
			QTest::newRow("readDG16") << AccessRight::READ_DG16 << "RFU"_L1;
			QTest::newRow("readDG15") << AccessRight::READ_DG15 << "RFU"_L1;
			QTest::newRow("readDG14") << AccessRight::READ_DG14 << "RFU"_L1;
			QTest::newRow("readDG13") << AccessRight::READ_DG13 << "Birth name"_L1;
			QTest::newRow("readDG12") << AccessRight::READ_DG12 << "Optional data"_L1;
			QTest::newRow("readDG11") << AccessRight::READ_DG11 << "Gender"_L1;
			QTest::newRow("readDG10") << AccessRight::READ_DG10 << "Nationality"_L1;
			QTest::newRow("readDG09") << AccessRight::READ_DG09 << "Place of birth"_L1;
			QTest::newRow("readDG08") << AccessRight::READ_DG08 << "Date of birth"_L1;
			QTest::newRow("readDG07") << AccessRight::READ_DG07 << "Doctoral degree"_L1;
			QTest::newRow("readDG06") << AccessRight::READ_DG06 << "Religious / artistic name"_L1;
			QTest::newRow("readDG05") << AccessRight::READ_DG05 << "Family name"_L1;
			QTest::newRow("readDG04") << AccessRight::READ_DG04 << "Given name(s)"_L1;
			QTest::newRow("readDG03") << AccessRight::READ_DG03 << "Valid until"_L1;
			QTest::newRow("readDG02") << AccessRight::READ_DG02 << "Issuing country"_L1;
			QTest::newRow("readDG01") << AccessRight::READ_DG01 << "Document type"_L1;
			QTest::newRow("installQualCert") << AccessRight::INSTALL_QUAL_CERT << "Installation of qualified signature certificates"_L1;
			QTest::newRow("installCert") << AccessRight::INSTALL_CERT << "Installation of signature certificates"_L1;
			QTest::newRow("pinManagement") << AccessRight::PIN_MANAGEMENT << "PIN Management"_L1;
			QTest::newRow("canAllowed") << AccessRight::CAN_ALLOWED << "CAN allowed"_L1;
			QTest::newRow("privilegedTerminal") << AccessRight::PRIVILEGED_TERMINAL << "Privileged terminal"_L1;
			QTest::newRow("pseudonymousSignatureAuthentication") << AccessRight::PSA << "Pseudonym"_L1;
			QTest::newRow("restrictedIdentification") << AccessRight::RESTRICTED_IDENTIFICATION << "Pseudonym"_L1;
			QTest::newRow("comunityIdVerification") << AccessRight::COMMUNITY_ID_VERIFICATION << "Address verification"_L1;
			QTest::newRow("ageVerification") << AccessRight::AGE_VERIFICATION << "Age verification"_L1;

			QTest::newRow("default") << static_cast<AccessRight>(100) << "Unknown"_L1;
		}


		void test_ToDisplayText()
		{
			QFETCH(AccessRight, input);
			QFETCH(QLatin1String, output);

			QCOMPARE(AccessRoleAndRightsUtil::toDisplayText(input), output);
		}


};


QTEST_GUILESS_MAIN(test_AccessRoleAndRight)
#include "test_AccessRoleAndRight.moc"
