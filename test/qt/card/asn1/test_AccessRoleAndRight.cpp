/*!
 * \brief Unit tests for \ref AccessRoleAndRight
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/AccessRoleAndRight.h"

#include <QtTest>

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
		QTest::newRow("Pseudonym") << AccessRight::RESTRICTED_IDENTIFICATION;
		QTest::newRow("AddressVerification") << AccessRight::COMMUNITY_ID_VERIFICATION;
		QTest::newRow("AgeVerification") << AccessRight::AGE_VERIFICATION;
	}


	private Q_SLOTS:
		void checkAccessRightNames_data()
		{
			fillTestData();
			QTest::newRow("") << AccessRight::WRITE_DG17;
			QTest::newRow("") << AccessRight::RFU_32;
			QTest::newRow("") << AccessRight::CAN_ALLOWED;
			QTest::newRow("") << AccessRight::PRIVILEGED_TERMINAL;
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
			const auto& func = [&](AccessRight pRight){
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
			const auto& func = [&](AccessRight pRight){
						right = pRight;
					};

			QVERIFY(!AccessRoleAndRightsUtil::fromTechnicalName(QString("crap"), func));
			QCOMPARE(right, undefined);

			QVERIFY(!AccessRoleAndRightsUtil::fromTechnicalName(QString("CAN_ALLOWED"), func));
			QCOMPARE(right, undefined);

			QVERIFY(!AccessRoleAndRightsUtil::fromTechnicalName(QString("CanAllowed"), func));
			QCOMPARE(right, undefined);

			QVERIFY(!AccessRoleAndRightsUtil::fromTechnicalName(QString("familyname"), func));
			QCOMPARE(right, undefined);
		}


		void checkJoinFromTechnicalName()
		{
			auto data = AccessRoleAndRightsUtil::joinFromTechnicalName({"DocumentType", "DoctoralDegree"});
			QCOMPARE(data, QStringLiteral("Document type, Doctoral degree"));

			data = AccessRoleAndRightsUtil::joinFromTechnicalName({"crap", "FamilyName"});
			QCOMPARE(data, QStringLiteral("crap, Family name"));
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
			QTest::addColumn<QString>("output");

			const auto& reserved = QStringLiteral("Unknown (reserved)");
			QTest::newRow("RFU_29") << AccessRight::RFU_29 << reserved;
			QTest::newRow("RFU_30") << AccessRight::RFU_30 << reserved;
			QTest::newRow("RFU_31") << AccessRight::RFU_31 << reserved;
			QTest::newRow("RFU_32") << AccessRight::RFU_32 << reserved;

			QTest::newRow("writeDG17") << AccessRight::WRITE_DG17 << QString("WRITE_DG17");
			QTest::newRow("writeDG18") << AccessRight::WRITE_DG18 << QString("WRITE_DG18");
			QTest::newRow("writeDG19") << AccessRight::WRITE_DG19 << QString("WRITE_DG19");
			QTest::newRow("writeDG20") << AccessRight::WRITE_DG20 << QString("WRITE_DG20");
			QTest::newRow("writeDG21") << AccessRight::WRITE_DG21 << QString("WRITE_DG21");
			QTest::newRow("readDG21") << AccessRight::READ_DG21 << QString("Optional data");
			QTest::newRow("readDG20") << AccessRight::READ_DG20 << QString("Residence permit II");
			QTest::newRow("readDG19") << AccessRight::READ_DG19 << QString("Residence permit I");
			QTest::newRow("readDG18") << AccessRight::READ_DG18 << QString("Community-ID");
			QTest::newRow("readDG17") << AccessRight::READ_DG17 << QString("Address");
			QTest::newRow("readDG16") << AccessRight::READ_DG16 << QString("RFU");
			QTest::newRow("readDG15") << AccessRight::READ_DG15 << QString("RFU");
			QTest::newRow("readDG14") << AccessRight::READ_DG14 << QString("RFU");
			QTest::newRow("readDG13") << AccessRight::READ_DG13 << QString("Birth name");
			QTest::newRow("readDG12") << AccessRight::READ_DG12 << QString("Optional data");
			QTest::newRow("readDG11") << AccessRight::READ_DG11 << QString("Gender");
			QTest::newRow("readDG10") << AccessRight::READ_DG10 << QString("Nationality");
			QTest::newRow("readDG09") << AccessRight::READ_DG09 << QString("Place of birth");
			QTest::newRow("readDG08") << AccessRight::READ_DG08 << QString("Date of birth");
			QTest::newRow("readDG07") << AccessRight::READ_DG07 << QString("Doctoral degree");
			QTest::newRow("readDG06") << AccessRight::READ_DG06 << QString("Religious / artistic name");
			QTest::newRow("readDG05") << AccessRight::READ_DG05 << QString("Family name");
			QTest::newRow("readDG04") << AccessRight::READ_DG04 << QString("Given name(s)");
			QTest::newRow("readDG03") << AccessRight::READ_DG03 << QString("Valid until");
			QTest::newRow("readDG02") << AccessRight::READ_DG02 << QString("Issuing country");
			QTest::newRow("readDG01") << AccessRight::READ_DG01 << QString("Document type");
			QTest::newRow("installQualCert") << AccessRight::INSTALL_QUAL_CERT << QString("Installation of qualified signature certificates");
			QTest::newRow("installCert") << AccessRight::INSTALL_CERT << QString("Installation of signature certificates");
			QTest::newRow("pinManagement") << AccessRight::PIN_MANAGEMENT << QString("PIN Management");
			QTest::newRow("canAllowed") << AccessRight::CAN_ALLOWED << QString("CAN allowed");
			QTest::newRow("privilegedTerminal") << AccessRight::PRIVILEGED_TERMINAL << QString("Privileged terminal");
			QTest::newRow("restrictedIdentification") << AccessRight::RESTRICTED_IDENTIFICATION << QString("Pseudonym");
			QTest::newRow("comunityIdVerification") << AccessRight::COMMUNITY_ID_VERIFICATION << QString("Address verification");
			QTest::newRow("ageVerification") << AccessRight::AGE_VERIFICATION << QString("Age verification");

			QTest::newRow("default") << static_cast<AccessRight>(100) << QString("Unknown");
		}


		void test_ToDisplayText()
		{
			QFETCH(AccessRight, input);
			QFETCH(QString, output);

			QCOMPARE(AccessRoleAndRightsUtil::toDisplayText(input), output);
		}


};


QTEST_GUILESS_MAIN(test_AccessRoleAndRight)
#include "test_AccessRoleAndRight.moc"
