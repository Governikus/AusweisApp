/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref DidAuthenticateEAC1
 */

#include "paos/invoke/DidAuthenticateResponseEac1.h"

#include "asn1/CVCertificate.h"
#include "pinpad/EstablishPaceChannelOutput.h"

#include "CertificateHelper.h"
#include "TestFileHelper.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_DidAuthenticateResponseEAC1
	: public QObject
{
	Q_OBJECT

	QByteArray readFile(const QString& pFileName)
	{
		return TestFileHelper::readFile(QStringLiteral(":/card/").append(pFileName));
	}

	private Q_SLOTS:
		void type()
		{
			DIDAuthenticateResponseEAC1 elem;
			QCOMPARE(elem.mType, PaosType::DID_AUTHENTICATE_RESPONSE_EAC1);
		}


		void getCertificateHolderAuthorizationTemplate()
		{
			DIDAuthenticateResponseEAC1 msg;
			msg.setCertificateHolderAuthorizationTemplate("dummy cert holder");
			QCOMPARE(msg.getCertificateHolderAuthorizationTemplate(), QByteArray("dummy cert holder"));
			QVERIFY(msg.marshall().contains("<CertificateHolderAuthorizationTemplate>dummy cert holder</CertificateHolderAuthorizationTemplate>"));
		}


		void getEFCardAccess()
		{
			DIDAuthenticateResponseEAC1 msg;
			msg.setEFCardAccess("dummy card access");
			QCOMPARE(msg.getEFCardAccess(), QByteArray("dummy card access"));
			QVERIFY(msg.marshall().contains("<EFCardAccess>dummy card access</EFCardAccess>"));
		}


		void getIDPICC()
		{
			DIDAuthenticateResponseEAC1 msg;
			msg.setIDPICC("bla bla bla");
			QCOMPARE(msg.getIDPICC(), QByteArray("bla bla bla"));
			QVERIFY(msg.marshall().contains("<IDPICC>bla bla bla</IDPICC>"));
		}


		void getChallenge()
		{
			DIDAuthenticateResponseEAC1 msg;
			msg.setChallenge("1234567890");
			QVERIFY(msg.marshall().contains("<Challenge>1234567890</Challenge>"));
		}


		void certificationAuthorityReference()
		{
			const auto& cvca_DETESTeID00002 = CertificateHelper::fromHex(readFile("cvca-DETESTeID00002.hex"_L1));
			const auto& cvca_DETESTeID00001 = CertificateHelper::fromHex(readFile("cvca-DETESTeID00001.hex"_L1));

			EstablishPaceChannelOutput channel;
			channel.setCarCurr(cvca_DETESTeID00002->getBody().getCertificateHolderReference());
			channel.setCarPrev(cvca_DETESTeID00001->getBody().getCertificateHolderReference());

			DIDAuthenticateResponseEAC1 msg;
			msg.setCertificationAuthorityReference(channel);
			QVERIFY(msg.marshall().contains("<CertificationAuthorityReference>DETESTeID00002</CertificationAuthorityReference>\n                <CertificationAuthorityReference>DETESTeID00001</CertificationAuthorityReference>"_L1));
		}


		void checkTemplate()
		{
			DIDAuthenticateResponseEAC1 msg;
			msg.setRelatedMessageId("urn:uuid:A9CF4F0B8BFE483B8A5C7E6738C178FE"_L1);
			msg.setCertificateHolderAuthorizationTemplate("a");
			EstablishPaceChannelOutput output;
			output.setCarCurr("b");
			msg.setCertificationAuthorityReference(output);
			msg.setEFCardAccess("c");
			msg.setIDPICC("d");
			msg.setChallenge("e");

			auto data = QString::fromLatin1(msg.marshall());
			data.replace(QRegularExpression("<wsa:MessageID>.*</wsa:MessageID>"_L1), "<wsa:MessageID>STRIP ME</wsa:MessageID>"_L1);
			QCOMPARE(data, QString::fromLatin1(TestFileHelper::readFile(":/paos/DIDAuthenticateResponse2.xml"_L1)));
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateResponseEAC1)
#include "test_DidAuthenticateResponseEAC1.moc"
