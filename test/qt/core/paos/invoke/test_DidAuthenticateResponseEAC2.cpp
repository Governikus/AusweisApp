/*!
 * \brief Unit tests for \ref DidAuthenticateEAC2
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "paos/invoke/DidAuthenticateResponseEac2.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>


using namespace governikus;

class test_DidAuthenticateResponseEAC2
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void type()
		{
			DIDAuthenticateResponseEAC2 elem;
			elem.setMessageId("dummy");
			QCOMPARE(elem.mType, PaosType::DID_AUTHENTICATE_RESPONSE_EAC2);
		}


		void emptyAuthenticationProtocolData()
		{
			DIDAuthenticateResponseEAC2 msg;
			msg.setMessageId("dummy");

			QVERIFY(!msg.marshall().contains("EFCardSecurity"));
			QVERIFY(!msg.marshall().contains("AuthenticationToken"));
			QVERIFY(!msg.marshall().contains("Nonce"));
			QVERIFY(!msg.marshall().contains("Challenge"));
		}


		void efCardSecurity()
		{
			DIDAuthenticateResponseEAC2 msg;
			msg.setMessageId("dummy");
			msg.setEfCardSecurity(QByteArray::fromHex(QByteArray("1234567890")));

			QVERIFY(msg.marshall().contains("EFCardSecurity"));
		}


		void authenticationToken()
		{
			DIDAuthenticateResponseEAC2 msg;
			msg.setMessageId("dummy");
			msg.setAuthenticationToken(QByteArray::fromHex(QByteArray("1234567890")));

			QVERIFY(msg.marshall().contains("AuthenticationToken"));
		}


		void nonce()
		{
			DIDAuthenticateResponseEAC2 msg;
			msg.setMessageId("dummy");
			msg.setNonce(QByteArray::fromHex(QByteArray("1234567890")));

			QVERIFY(msg.marshall().contains("Nonce"));
		}


		void challenge()
		{
			DIDAuthenticateResponseEAC2 msg;
			msg.setMessageId("dummy");
			msg.setChallenge("1234567890");

			QVERIFY(msg.marshall().contains("Challenge"));
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateResponseEAC2)
#include "test_DidAuthenticateResponseEAC2.moc"
