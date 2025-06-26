/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>

#include "asn1/ChipAuthenticationInfo.h"
#include "asn1/PaceInfo.h"
#include "asn1/SecurityInfos.h"


using namespace governikus;


Q_DECLARE_METATYPE(Oid)


class test_SecurityInfos
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseCrap()
		{
			QVERIFY(SecurityInfos::decode("06020400") == nullptr);
		}


		void emmptySet()
		{
			QByteArray hexString("3100");

			auto securityInfos = SecurityInfos::fromHex(hexString);

			QVERIFY(securityInfos != nullptr);
			QCOMPARE(securityInfos->getSecurityInfos().size(), 0);
		}


		void setWithUnknownElement()
		{
			QByteArray hexString("31 03"
								 "        020101");

			auto securityInfos = SecurityInfos::fromHex(hexString);

			QVERIFY(securityInfos == nullptr);
		}


		void setWithPaceInfo()
		{
			QByteArray hexString("31 14"
								 "        30 12"
								 "            06 0A 04007F00070202040202"
								 "            02 01 02"
								 "            02 01 08");

			auto securityInfos = SecurityInfos::fromHex(hexString);

			QVERIFY(securityInfos != nullptr);
			QCOMPARE(securityInfos->getSecurityInfos().size(), 1);
			QCOMPARE(securityInfos->getPaceInfos().size(), 1);
			QCOMPARE(securityInfos->getChipAuthenticationInfos().size(), 0);
			QVERIFY(!securityInfos->getMobileEIDTypeInfo());
		}


		void setWithChipAuthenticationInfo()
		{
			QByteArray hexString("31 14"
								 "        30 12"
								 "            06 0A 04007F00070202030202"
								 "            02 01 02"
								 "            02 01 08");

			auto securityInfos = SecurityInfos::fromHex(hexString);

			QVERIFY(securityInfos != nullptr);
			QCOMPARE(securityInfos->getSecurityInfos().size(), 1);
			QCOMPARE(securityInfos->getPaceInfos().size(), 0);
			QCOMPARE(securityInfos->getChipAuthenticationInfos().size(), 1);
			QVERIFY(!securityInfos->getMobileEIDTypeInfo());
		}


		void setWithManySecurityInfos()
		{
			QByteArray hexString("31 4C"
								 "        30 12"
								 "            06 0A 04007F00070202030202"
								 "            02 01 02"
								 "            02 01 08"
								 "        30 12"
								 "            06 0A 04007F00070202040202"
								 "            02 01 02"
								 "            02 01 08"
								 "        30 12"
								 "            06 0A 04007F000702020F0F0F"
								 "            02 01 02"
								 "            02 01 08"
								 "        30 0E"
								 "            06 0A 04007F00070302030201"
								 "            05 00");

			auto securityInfos = SecurityInfos::fromHex(hexString);

			QVERIFY(securityInfos != nullptr);
			QCOMPARE(securityInfos->getSecurityInfos().size(), 4);
			QCOMPARE(securityInfos->getPaceInfos().size(), 1);
			QCOMPARE(securityInfos->getChipAuthenticationInfos().size(), 1);
			QVERIFY(securityInfos->getMobileEIDTypeInfo());
			QCOMPARE(securityInfos->getMobileEIDTypeInfo()->getOid(), Oid(KnownOid::ID_MOBILE_EID_TYPE_SE_CERTIFIED));
		}


		void setWithMultipleMobileEIDTypeInfo()
		{
			QByteArray hexString("31 30"
								 "        30 0E"
								 "            06 0A 04007F00070302030201"
								 "            05 00"
								 "        30 0E"
								 "            06 0A 04007F00070302030202"
								 "            05 00"
								 "        30 0E"
								 "            06 0A 04007F00070302030203"
								 "            05 00");

			auto securityInfos = SecurityInfos::fromHex(hexString);

			QVERIFY(securityInfos == nullptr);
		}


		void testAllPossibleMobileEIDTypeInfo_data()
		{
			QTest::addColumn<QByteArray>("hexString");
			QTest::addColumn<Oid>("expectedOid");

			QTest::newRow("SE_CERTIFIED") << QByteArray("31 10"
														"        30 0E"
														"            06 0A 04007F00070302030201"
														"            05 00")
										  << Oid(KnownOid::ID_MOBILE_EID_TYPE_SE_CERTIFIED);
			QTest::newRow("SE_ENDORSED") << QByteArray("31 10"
													   "        30 0E"
													   "            06 0A 04007F00070302030202"
													   "            05 00")
										 << Oid(KnownOid::ID_MOBILE_EID_TYPE_SE_ENDORSED);
			QTest::newRow("HW_KEYSTORE") << QByteArray("31 10"
													   "        30 0E"
													   "            06 0A 04007F00070302030203"
													   "            05 00")
										 << Oid(KnownOid::ID_MOBILE_EID_TYPE_HW_KEYSTORE);
			QTest::newRow("Invalid unknown ID") << QByteArray("31 10"
															  "        30 0E"
															  "            06 0A 04007F00070302030204"
															  "            05 00")
												<< Oid();
			QTest::newRow("No MobileEIDTypeInfo") << QByteArray("31 14"
																"        30 12"
																"            06 0A 04007F00070202030202"
																"            02 01 02"
																"            02 01 08")
												  << Oid();
		}


		void testAllPossibleMobileEIDTypeInfo()
		{
			QFETCH(QByteArray, hexString);
			QFETCH(Oid, expectedOid);

			auto securityInfos = SecurityInfos::fromHex(hexString);

			QVERIFY(securityInfos != nullptr);
			if (expectedOid.isUndefined())
			{
				QVERIFY(!securityInfos->getMobileEIDTypeInfo());
			}
			else
			{
				QVERIFY(securityInfos->getMobileEIDTypeInfo());
				QCOMPARE(securityInfos->getMobileEIDTypeInfo()->getOid(), Oid(expectedOid));
			}
		}


		void getContentBytes()
		{
			QByteArray hexString("31 14"
								 "        30 12"
								 "            06 0A 04007F00070202030202"
								 "            02 01 02"
								 "            02 01 08");

			auto securityInfos = SecurityInfos::fromHex(hexString);

			QVERIFY(securityInfos != nullptr);
			QCOMPARE(securityInfos->getContentBytes(), QByteArray::fromHex(hexString));
		}


};

QTEST_GUILESS_MAIN(test_SecurityInfos)
#include "test_SecurityInfos.moc"
