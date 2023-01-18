/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "TestFileHelper.h"
#include "asn1/ChipAuthenticationInfo.h"
#include "asn1/PaceInfo.h"
#include "asn1/SecurityInfos.h"

#include <QtTest>

using namespace governikus;

class test_efCardAccess
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
		}


		void parseEFCardAccess()
		{
			QByteArray bytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/efCardAccess.hex"));

			auto efCardAccess = EFCardAccess::decode(bytes);

			QVERIFY(efCardAccess != nullptr);
			QCOMPARE(efCardAccess->getChipAuthenticationInfos().size(), 1);
			QCOMPARE(efCardAccess->getPaceInfos().size(), 1);
			QCOMPARE(efCardAccess->getContentBytes(), bytes);
		}


};

QTEST_GUILESS_MAIN(test_efCardAccess)
#include "test_efCardAccess.moc"
