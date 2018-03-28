/*!
 * \brief Unit tests for \DidAuthenticateEac1
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "paos/retrieve/DidAuthenticateEac2.h"
#include "paos/retrieve/DidAuthenticateEac2Parser.h"
#include "TestFileHelper.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>

using namespace governikus;

class test_DidAuthenticateEac2
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
		}


		void parseXml()
		{
			QByteArray content = TestFileHelper::readFile(":/paos/DIDAuthenticateEAC2.xml");
			QScopedPointer<DIDAuthenticateEAC2> eac2(static_cast<DIDAuthenticateEAC2*>(DidAuthenticateEac2Parser().parse(content)));
			QVERIFY(!eac2.isNull());

			QCOMPARE(eac2->getConnectionHandle().getCardApplication(), QString("4549445F49534F5F32343732375F42415345"));
			QCOMPARE(eac2->getConnectionHandle().getContextHandle(), QString("4549445F4946445F434F4E544558545F42415345"));
			QCOMPARE(eac2->getConnectionHandle().getIfdName(), QString("REINER SCT cyberJack RFID komfort USB 52"));
			QCOMPARE(eac2->getConnectionHandle().getSlotHandle(), QString("34366364653038392D623031322D346664372D386233362D343664346232393537636236"));
			QCOMPARE(eac2->getConnectionHandle().getSlotIndex(), QString("0"));

			QCOMPARE(eac2->getDidName(), QString("PIN"));
			QCOMPARE(eac2->getCvCertificates().size(), 0);
			QCOMPARE(eac2->getCvCertificatesAsBinary().size(), 0);
			QCOMPARE(eac2->getEphemeralPublicKey(), QString("046E35D82A4F370F9B4C7B6415B7FF936B2FE2DAFD8E008037E8B0464918FDACD33AE730BABAE05C45E9567F010A4A40CA3CF04409C2EEBAC0E38B9E36975E2AE1"));
			QCOMPARE(eac2->getSignature(), QString("2DE1E928C7E6C40F3389BF1973214EAB082F816EB42B6A3C3B36EF4BDD61A2594C7814DDBF7D5A32ADE60188BFCA12BBF7C2FB8DF93A4FE042121015F1FAC7EA"));
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateEac2)
#include "test_DidAuthenticateEac2.moc"
