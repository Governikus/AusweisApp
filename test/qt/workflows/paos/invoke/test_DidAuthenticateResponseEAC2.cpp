/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref DidAuthenticateEAC2
 */

#include "paos/invoke/DidAuthenticateResponseEac2.h"

#include "TestFileHelper.h"

#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_DidAuthenticateResponseEAC2
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void type()
		{
			DIDAuthenticateResponseEAC2 elem;
			QCOMPARE(elem.mType, PaosType::DID_AUTHENTICATE_RESPONSE_EAC2);
		}


		void emptyAuthenticationProtocolData()
		{
			DIDAuthenticateResponseEAC2 msg;

			QVERIFY(!msg.marshall().contains("EFCardSecurity"));
			QVERIFY(!msg.marshall().contains("AuthenticationToken"));
			QVERIFY(!msg.marshall().contains("Nonce"));
			QVERIFY(!msg.marshall().contains("Challenge"));
		}


		void efCardSecurity()
		{
			DIDAuthenticateResponseEAC2 msg;
			msg.setEfCardSecurity(QByteArray::fromHex(QByteArray("1234567890")));

			QVERIFY(msg.marshall().contains("EFCardSecurity"));
		}


		void authenticationToken()
		{
			DIDAuthenticateResponseEAC2 msg;
			msg.setAuthenticationToken(QByteArray::fromHex(QByteArray("1234567890")));

			QVERIFY(msg.marshall().contains("AuthenticationToken"));
		}


		void nonce()
		{
			DIDAuthenticateResponseEAC2 msg;
			msg.setNonce(QByteArray::fromHex(QByteArray("1234567890")));

			QVERIFY(msg.marshall().contains("Nonce"));
		}


		void challenge()
		{
			DIDAuthenticateResponseEAC2 msg;
			msg.setChallenge("1234567890");

			QVERIFY(msg.marshall().contains("Challenge"));
		}


		void checkTemplate()
		{
			DIDAuthenticateResponseEAC2 msg;
			msg.setEfCardSecurity("a");
			msg.setAuthenticationToken("b");
			msg.setNonce("c");
			auto data = QString::fromLatin1(msg.marshall());
			data.replace(QRegularExpression("<wsa:MessageID>.*</wsa:MessageID>"_L1), "<wsa:MessageID>STRIP ME</wsa:MessageID>"_L1);
			QCOMPARE(data, QString::fromLatin1(TestFileHelper::readFile(":/paos/DIDAuthenticateResponse.xml"_L1)));
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateResponseEAC2)
#include "test_DidAuthenticateResponseEAC2.moc"
