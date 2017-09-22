/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include <QApplication>
#include <QSharedPointer>
#include <QtCore/QtCore>
#include <QtTest/QtTest>

#include "asn1/ChipAuthenticationInfo.h"
#include "asn1/PACEInfo.h"
#include "asn1/SecurityInfos.h"
#include "TestFileHelper.h"

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
			QCOMPARE(efCardAccess->getSecurityInfos<ChipAuthenticationInfo>().size(), 1);
			QCOMPARE(efCardAccess->getSecurityInfos<PACEInfo>().size(), 1);
			QCOMPARE(efCardAccess->getContentBytes(), bytes);
		}


};

QTEST_GUILESS_MAIN(test_efCardAccess)
#include "test_efCardAccess.moc"
