/*!
 * \brief Unit tests for \ref SecurityInfos
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>

#include "asn1/ChipAuthenticationInfo.h"
#include "asn1/PaceInfo.h"
#include "asn1/SecurityInfos.h"


using namespace governikus;

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
		}


		void setWithManySecurityInfos()
		{
			QByteArray hexString("31 3C"
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
								 "            02 01 08");

			auto securityInfos = SecurityInfos::fromHex(hexString);

			QVERIFY(securityInfos != nullptr);
			QCOMPARE(securityInfos->getSecurityInfos().size(), 3);
			QCOMPARE(securityInfos->getPaceInfos().size(), 1);
			QCOMPARE(securityInfos->getChipAuthenticationInfos().size(), 1);
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
