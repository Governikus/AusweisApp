/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "TestFileHelper.h"
#include "asn1/ChipAuthenticationInfo.h"
#include "asn1/PACEInfo.h"
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
			QCOMPARE(efCardAccess->getPACEInfos().size(), 1);
			QCOMPARE(efCardAccess->getContentBytes(), bytes);
		}


};

QTEST_GUILESS_MAIN(test_efCardAccess)
#include "test_efCardAccess.moc"
