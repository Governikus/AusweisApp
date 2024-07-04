/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref CVCertificateBody
 */

#include "asn1/CVCertificateBody.h"

#include "asn1/ASN1Util.h"
#include "asn1/CVCertificate.h"
#include "asn1/Oid.h"

#include "CertificateHelper.h"
#include "TestFileHelper.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
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

			QCOMPARE(body->getCertificateExpirationDate().toString("yyMMdd"_L1), "030429"_L1);
			QCOMPARE(body->getCertificateEffectiveDate().toString("yyMMdd"_L1), "150102"_L1);
		}


		void getExtensions()
		{
			const auto& rawCert = TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"_L1);
			auto cvc = CertificateHelper::fromHex(rawCert);

			QByteArray expectedDescription = QByteArray::fromHex("12CA9D0A51DF9297EABA7EBE9AB49DF2F4CF83E0DBB02772EFAD89C8AD75FCCD");
			QCOMPARE(cvc->getBody().getExtension(KnownOid::ID_DESCRIPTION), expectedDescription);

			QByteArray expectedSector = QByteArray::fromHex("CB1E1940159F11DC96845B87C23B86F9BAA755A789A914BBD5B8FA9784019D1C");
			QCOMPARE(cvc->getBody().getExtension(KnownOid::ID_SECTOR_RI), expectedSector);

			QVERIFY(rawCert.contains(cvc->getRawBody().toHex()));
		}


		void testExtendedExtensions()
		{
			const auto& rawCert = TestFileHelper::readFile(":/card/cvdv-DEDVtIDGVK300029.hex"_L1);
			auto cvc = CertificateHelper::fromHex(rawCert);

			QByteArray expectedDescription = QByteArray::fromHex("315A1DA468AE12C839F8AB386E5CAB88C9FFAC2033C9910E202E2004BDE07FE6");
			QCOMPARE(cvc->getBody().getExtension(KnownOid::ID_DESCRIPTION), expectedDescription);

			QByteArray expectedSectorRi = QByteArray::fromHex("39D51F1942520AE8EC7F5CC654E78A521D8C09F607CD9ADF7BB9FDC5BFFB90D7");
			QCOMPARE(cvc->getBody().getExtension(KnownOid::ID_SECTOR_RI), expectedSectorRi);

			QByteArray expectedSectorPs = QByteArray::fromHex("0D");
			QCOMPARE(cvc->getBody().getExtension(KnownOid::ID_SECTOR_PS), expectedSectorPs);

			QVERIFY(rawCert.contains(cvc->getRawBody().toHex()));
		}


		void getPublicKey_noCompletePublicKey()
		{
			auto cvc = CertificateHelper::fromHex(TestFileHelper::readFile(":/card/cvdv-DEDVeIDDPST00035.hex"_L1));
			const auto& key = cvc->getBody().getPublicKey();

			QVERIFY(!key.getUncompressedPublicPoint().isNull());
			QVERIFY(!key.isComplete());
			QVERIFY(key.createKey(key.getUncompressedPublicPoint()).isNull());
		}


		void getPublicKey_completePublicKey()
		{
			auto cvc = CertificateHelper::fromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00002_DETESTeID00001.hex"_L1));
			const auto& key = cvc->getBody().getPublicKey();

			QVERIFY(!key.getUncompressedPublicPoint().isNull());
			QVERIFY(key.isComplete());
			QVERIFY(!key.createKey(key.getUncompressedPublicPoint()).isNull());
		}


		void getCertificationAuthorityReference()
		{
			auto cvc = CertificateHelper::fromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"_L1));

			QCOMPARE(cvc->getBody().getCertificationAuthorityReference(), QByteArray("DETESTeID00001"));
		}


		void getCertificateHolderReference()
		{
			auto cvc = CertificateHelper::fromHex(TestFileHelper::readFile(":/card/cvca-DETESTeID00001.hex"_L1));

			QCOMPARE(cvc->getBody().getCertificateHolderReference(), QByteArray("DETESTeID00001"));
		}


		void getCHAT()
		{
			auto cvc = CertificateHelper::fromHex(TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"_L1));

			auto chat = const_cast<CHAT&>(cvc->getBody().getCHAT());
			QCOMPARE(encodeObject(&chat).toHex().toUpper(), "7F4C12060904007F0007030102025305000513FF87"_ba);
		}


		void getCertificateEffectiveDate()
		{
			auto cvc = CertificateHelper::fromHex(TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"_L1));

			QCOMPARE(cvc->getBody().getCertificateEffectiveDate(), QDate(2014, 03, 13));
		}


		void getCertificateExpirationDate()
		{
			auto cvc = CertificateHelper::fromHex(TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"_L1));

			QCOMPARE(cvc->getBody().getCertificateExpirationDate(), QDate(2014, 04, 11));
		}


		void getHashAlgorithm()
		{
			auto cvc = CertificateHelper::fromHex(TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"_L1));

			QCOMPARE(static_cast<int>(cvc->getBody().getHashAlgorithm()), static_cast<int>(QCryptographicHash::Sha256));
		}


		void getCertificateProfileIdentifier()
		{
			auto cvc = CertificateHelper::fromHex(TestFileHelper::readFile(":/card/cvat-DEDEMODEV00038.hex"_L1));

			QCOMPARE(cvc->getBody().getCertificateProfileIdentifier(), QByteArray::fromHex("00"));
		}


};

QTEST_GUILESS_MAIN(test_CVCertificateBody)
#include "test_CVCertificateBody.moc"
