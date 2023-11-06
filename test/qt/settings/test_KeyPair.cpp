/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref CertificateGenerator
 */

#include "KeyPair.h"

#include "TlsChecker.h"

#include <QSslCertificateExtension>
#include <QtTest>

using namespace governikus;


class test_KeyPair
	: public QObject
{
	Q_OBJECT

	private:
		KeyPair pair1 = KeyPair::generate();
		KeyPair pair2 = KeyPair::generate();

	private Q_SLOTS:
		void validKey_data()
		{
			QTest::addColumn<QSsl::KeyAlgorithm>("algorithm");
			QTest::addColumn<int>("size");
			QTest::addColumn<QLatin1String>("curve");

			QTest::newRow("RSA") << QSsl::Rsa << 2048 << QLatin1String();
			QTest::newRow("prime256v1") << QSsl::Ec << 256 << QLatin1String("prime256v1");
			QTest::newRow("secp384r1") << QSsl::Ec << 384 << QLatin1String("secp384r1");
			QTest::newRow("brainpoolP512r1") << QSsl::Ec << 512 << QLatin1String("brainpoolP512r1");
		}


		void validKey()
		{
			QFETCH(QSsl::KeyAlgorithm, algorithm);
			QFETCH(int, size);
			QFETCH(QLatin1String, curve);

			KeyPair pair = KeyPair::generate(curve.data());
			QVERIFY(pair.isValid());
			const auto& key = pair.getKey();
			QVERIFY(!key.isNull());
			QCOMPARE(key.length(), size);
			QCOMPARE(key.algorithm(), algorithm);
			QCOMPARE(key.type(), QSsl::PrivateKey);
		}


		void multiKey()
		{
			QVERIFY(!pair1.getKey().isNull());

			QVERIFY(!pair2.getKey().isNull());

			QVERIFY(pair1.getKey() != pair2.getKey());
		}


		void validCertificate_data()
		{
			QTest::addColumn<QSsl::KeyAlgorithm>("algorithm");
			QTest::addColumn<int>("size");
			QTest::addColumn<QLatin1String>("curve");

			QTest::newRow("RSA") << QSsl::Rsa << 2048 << QLatin1String();
			QTest::newRow("prime256v1") << QSsl::Ec << 256 << QLatin1String("prime256v1");
			QTest::newRow("secp384r1") << QSsl::Ec << 384 << QLatin1String("secp384r1");
			QTest::newRow("brainpoolP512r1") << QSsl::Ec << 512 << QLatin1String("brainpoolP512r1");
		}


		void validCertificate()
		{
			QFETCH(QSsl::KeyAlgorithm, algorithm);
			QFETCH(int, size);
			QFETCH(QLatin1String, curve);

			KeyPair pair = KeyPair::generate(curve.data());
			QVERIFY(pair.isValid());
			const auto& cert = pair.getCertificate();
			QVERIFY(!cert.isNull());
			QCOMPARE(cert.issuerInfo(QSslCertificate::CommonName).size(), 1);
			QCOMPARE(cert.issuerInfo(QSslCertificate::CommonName).at(0), QCoreApplication::applicationName());
			QCOMPARE(cert.issuerInfo(QSslCertificate::SerialNumber).size(), 1);
			QVERIFY(cert.issuerInfo(QSslCertificate::SerialNumber).at(0).size() > 0);
			QVERIFY(cert.extensions().isEmpty());
			QCOMPARE(cert.version(), QByteArray("1"));
			QVERIFY(cert.isSelfSigned());

			X509* rawCert = static_cast<X509*>(cert.handle());
			const qlonglong serialNumberValue = ASN1_INTEGER_get(X509_get_serialNumber(rawCert));
			QVERIFY(serialNumberValue > 0);
			QVERIFY(QByteArray::number(serialNumberValue).size() < 21);

			const auto& key = cert.publicKey();
			QCOMPARE(key.length(), size);
			QCOMPARE(key.algorithm(), algorithm);
			QCOMPARE(key.type(), QSsl::PublicKey);
			QVERIFY(TlsChecker::hasValidCertificateKeyLength(cert));

			QVERIFY(cert.expiryDate().isValid());
			QVERIFY(cert.effectiveDate().isValid());
			QVERIFY(cert.expiryDate() > QDateTime::currentDateTime());
			QVERIFY(cert.effectiveDate() <= QDateTime::currentDateTime());
		}


		void multiCertificate()
		{
			QVERIFY(!pair1.getCertificate().isNull());

			QVERIFY(!pair2.getCertificate().isNull());

			QVERIFY(pair1.getCertificate() != pair2.getCertificate());
		}


};

QTEST_GUILESS_MAIN(test_KeyPair)
#include "test_KeyPair.moc"
