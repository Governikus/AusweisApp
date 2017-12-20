/*!
 * \brief Unit tests for \ref AccessRoleAndRight
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
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
			const AccessRight undefined = static_cast<AccessRight>(UINT_MAX);
			AccessRight right = undefined;
			const auto& func = [&](AccessRight pRight){
						right = pRight;
					};

			bool success = AccessRoleAndRightsUtil::fromTechnicalName(QTest::currentDataTag(), func);
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
			const AccessRight undefined = static_cast<AccessRight>(UINT_MAX);
			AccessRight right = undefined;
			const auto& func = [&](AccessRight pRight){
						right = pRight;
					};

			QVERIFY(!AccessRoleAndRightsUtil::fromTechnicalName("crap", func));
			QCOMPARE(right, undefined);

			QVERIFY(!AccessRoleAndRightsUtil::fromTechnicalName("CAN_ALLOWED", func));
			QCOMPARE(right, undefined);

			QVERIFY(!AccessRoleAndRightsUtil::fromTechnicalName("CanAllowed", func));
			QCOMPARE(right, undefined);

			QVERIFY(!AccessRoleAndRightsUtil::fromTechnicalName("familyname", func));
			QCOMPARE(right, undefined);
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


};


QTEST_GUILESS_MAIN(test_AccessRoleAndRight)
#include "test_AccessRoleAndRight.moc"
