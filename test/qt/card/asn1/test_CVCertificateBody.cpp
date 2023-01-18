/*!
 * \brief Unit tests for \ref CVCertificateBody
 *
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/CVCertificateBody.h"

#include "asn1/ASN1Util.h"
#include "asn1/CVCertificate.h"
#include "asn1/Oid.h"

#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

class test_CVCertificateBody
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void getDates()
		{
			auto body = newObject<CVCertificateBody>();
			body->setCertificateEffectiveDate(QDate(2015, 1, 2));
			body->setCertificateExpirationDate(QDate(2003, 4, 29));

			body->encode();

			QCOMPARE(body->getCertificateExpirationDate().toString("yyMMdd"), QString("030429"));
			QCOMPARE(body->getCertificateEffectiveDate().toString("yyMMdd"), QString("150102"));
		}


		void getExtensions_noExtensions()
		{
			auto cvc = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00002_DETESTeID00001.hex"));
			QVERIFY(cvc->getBody().getExtensions().isEmpty());
		}


		void getExtensions()
		{
			auto cvc = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"));
			QCOMPARE(cvc->getBody().getExtensions().size(), 2);

			QByteArray expectedDescription = QByteArray::fromHex("12CA9D0A51DF9297EABA7EBE9AB49DF2F4CF83E0DBB02772EFAD89C8AD75FCCD");
			QByteArray expectedSector = QByteArray::fromHex("CB1E1940159F11DC96845B87C23B86F9BAA755A789A914BBD5B8FA9784019D1C");
			QCOMPARE(cvc->getBody().getExtensions().value(KnownOid::ID_DESCRIPTION), expectedDescription);
			QCOMPARE(cvc->getBody().getExtensions().value(KnownOid::ID_SECTOR), expectedSector);
		}


		void getPublicKey_noCompletePublicKey()
		{
			auto cvc = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvdv-DEDVeIDDPST00035.hex"));
			const auto& key = cvc->getBody().getPublicKey();

			QVERIFY(!key.getUncompressedPublicPoint().isNull());
			QVERIFY(!key.isComplete());
			QVERIFY(key.createKey(key.getUncompressedPublicPoint()).isNull());
		}


		void getPublicKey_completePublicKey()
		{
			auto cvc = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00002_DETESTeID00001.hex"));
			const auto& key = cvc->getBody().getPublicKey();

			QVERIFY(!key.getUncompressedPublicPoint().isNull());
			QVERIFY(key.isComplete());
			QVERIFY(!key.createKey(key.getUncompressedPublicPoint()).isNull());
		}


		void getCertificationAuthorityReference()
		{
			auto cvc = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"));

			QCOMPARE(cvc->getBody().getCertificationAuthorityReference(), QByteArray("DETESTeID00001"));
		}


		void getCertificateHolderReference()
		{
			auto cvc = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"));

			QCOMPARE(cvc->getBody().getCertificateHolderReference(), QByteArray("DETESTeID00001"));
		}


		void getCHAT()
		{
			auto cvc = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"));

			auto chat = const_cast<CHAT&>(cvc->getBody().getCHAT());
			QCOMPARE(encodeObject(&chat).toHex().toUpper(), QByteArray("7F4C12060904007F0007030102025305000513FF87"));
		}


		void getCertificateEffectiveDate()
		{
			auto cvc = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"));

			QCOMPARE(cvc->getBody().getCertificateEffectiveDate(), QDate(2014, 03, 13));
		}


		void getCertificateExpirationDate()
		{
			auto cvc = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"));

			QCOMPARE(cvc->getBody().getCertificateExpirationDate(), QDate(2014, 04, 11));
		}


		void getHashAlgorithm()
		{
			auto cvc = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"));

			QCOMPARE(static_cast<int>(cvc->getBody().getHashAlgorithm()), static_cast<int>(QCryptographicHash::Sha256));
		}


		void getCertificateProfileIdentifier()
		{
			auto cvc = CVCertificate::fromHex(TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"));

			QCOMPARE(cvc->getBody().getCertificateProfileIdentifier(), QByteArray::fromHex("00"));
		}


};

QTEST_GUILESS_MAIN(test_CVCertificateBody)
#include "test_CVCertificateBody.moc"
