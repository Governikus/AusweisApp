/*!
 * \brief Unit tests for \ref ChipAuthenticationInfo
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>

#include "asn1/ChipAuthenticationInfo.h"
#include "asn1/KnownOIDs.h"


using namespace governikus;

class test_ChipAuthenticationInfo
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void parseCrap()
		{
			QVERIFY(ChipAuthenticationInfo::decode("3003020101") == nullptr);
		}


		void parsePACEInfo()
		{
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202040202"
												   "            02 01 02"
												   "            02 01 08");

			QVERIFY(ChipAuthenticationInfo::decode(bytes) == nullptr);
		}


		void parseChipAuthenticationInfoWithWrongContent()
		{
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202030202"
												   "            04 01 02"
												   "            04 01 08");

			auto chipAuthenticationInfo = ChipAuthenticationInfo::decode(bytes);

			QVERIFY(chipAuthenticationInfo == nullptr);
		}


		void missingOid()
		{
			QByteArray bytes = QByteArray::fromHex("30 06"
												   "            02 01 02"
												   "            02 01 08");

			QVERIFY(ChipAuthenticationInfo::decode(bytes) == nullptr);
		}


		void missingVersion()
		{
			QByteArray bytes = QByteArray::fromHex("30 0C"
												   "            06 0A 04007F00070202030202");

			QVERIFY(ChipAuthenticationInfo::decode(bytes) == nullptr);
		}


		void withoutOptionalParameter()
		{
			QByteArray bytes = QByteArray::fromHex("30 0F"
												   "            06 0A 04007F00070202030202"
												   "            02 01 02");

			auto chipAuthenticationInfo = ChipAuthenticationInfo::decode(bytes);

			QVERIFY(chipAuthenticationInfo != nullptr);
			QVERIFY(!chipAuthenticationInfo->getVersion().isNull());
			QVERIFY(chipAuthenticationInfo->getKeyId().isNull());
		}


		void getKeyId()
		{
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202030202"
												   "            02 01 02"
												   "            02 01 08");

			auto chipAuthenticationInfo = ChipAuthenticationInfo::decode(bytes);

			QVERIFY(chipAuthenticationInfo != nullptr);
			QCOMPARE(chipAuthenticationInfo->getKeyId(), QByteArray::fromHex("08"));
		}


		void getVersion()
		{
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202030202"
												   "            02 01 02"
												   "            02 01 08");

			auto chipAuthenticationInfo = ChipAuthenticationInfo::decode(bytes);

			QVERIFY(chipAuthenticationInfo != nullptr);
			QCOMPARE(chipAuthenticationInfo->getVersion(), QByteArray::fromHex("02)"));
		}


};

QTEST_GUILESS_MAIN(test_ChipAuthenticationInfo)
#include "test_ChipAuthenticationInfo.moc"
