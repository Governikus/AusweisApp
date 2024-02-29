/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref CertificateGenerator
 */

#include "KeyPair.h"

#include "TlsChecker.h"

#include <QSslCertificateExtension>
#include <QSslServer>
#include <QSslSocket>
#include <QtTest>

using namespace governikus;

Q_DECLARE_METATYPE(KeyPair)

class test_KeyPair
	: public QObject
{
	Q_OBJECT

	private:
		KeyPair pair1 = KeyPair::generate();
		KeyPair pair2 = KeyPair::generate();
		const int defaultDeriveSize = 3072;

	private Q_SLOTS:
		void validKey_data()
		{
			QTest::addColumn<QSsl::KeyAlgorithm>("algorithm");
			QTest::addColumn<int>("size");
			QTest::addColumn<QLatin1String>("curve");

			QTest::newRow("RSA 2048") << QSsl::Rsa << 2048 << QLatin1String();
#ifndef Q_CC_MSVC
			QTest::newRow("RSA 3072") << QSsl::Rsa << 3072 << QLatin1String();
			QTest::newRow("RSA 4096") << QSsl::Rsa << 4096 << QLatin1String();
#endif
			QTest::newRow("prime256v1") << QSsl::Ec << 256 << QLatin1String("prime256v1");
			QTest::newRow("secp384r1") << QSsl::Ec << 384 << QLatin1String("secp384r1");
			QTest::newRow("brainpoolP512r1") << QSsl::Ec << 512 << QLatin1String("brainpoolP512r1");
		}


		void validKey()
		{
			QFETCH(QSsl::KeyAlgorithm, algorithm);
			QFETCH(int, size);
			QFETCH(QLatin1String, curve);

			KeyPair pair = curve.isNull() ? KeyPair::generate(size) : KeyPair::generate(curve.data());
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

			KeyPair pair = curve.isNull() ? KeyPair::generate(size) : KeyPair::generate(curve.data());
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


		void certificateChain()
		{
			const KeyPair derived = KeyPair::generate(defaultDeriveSize, pair1.getKey().toPem(), pair1.getCertificate().toPem());

			QVERIFY(derived.isValid());
			QVERIFY(!derived.getCertificate().isNull());
			const auto& derivedCert = derived.getCertificate();

			QCOMPARE(derivedCert.issuerInfo(QSslCertificate::SerialNumber).size(), 1);
			QCOMPARE(pair1.getCertificate().issuerInfo(QSslCertificate::SerialNumber).size(), 1);

			QCOMPARE(derivedCert.issuerInfo(QSslCertificate::SerialNumber).at(0),
					pair1.getCertificate().issuerInfo(QSslCertificate::SerialNumber).at(0));

			QCOMPARE(derivedCert.issuerInfo(QSslCertificate::CommonName).size(), 1);
			QCOMPARE(pair1.getCertificate().issuerInfo(QSslCertificate::CommonName).size(), 1);

			QCOMPARE(derivedCert.issuerInfo(QSslCertificate::CommonName).at(0),
					pair1.getCertificate().issuerInfo(QSslCertificate::CommonName).at(0));

			QVERIFY(!derivedCert.isSelfSigned());
		}


		void sslSocket_data()
		{
			QTest::addColumn<KeyPair>("serverPair");
			QTest::addColumn<KeyPair>("clientPair");
			QTest::addColumn<QList<QSslCertificate>>("serverCaStore");
			QTest::addColumn<QList<QSslCertificate>>("clientCaStore");

			const KeyPair derived1 = KeyPair::generate(defaultDeriveSize, pair1.getKey().toPem(), pair1.getCertificate().toPem());
			const KeyPair derived2 = KeyPair::generate(defaultDeriveSize, pair2.getKey().toPem(), pair2.getCertificate().toPem());

			QTest::newRow("root as truststore") << derived1 << derived2
												<< QList<QSslCertificate>({pair2.getCertificate()})
												<< QList<QSslCertificate>({pair1.getCertificate()
					});

			QTest::newRow("chain as truststore") << derived1 << derived2
												 << QList<QSslCertificate>({derived2.getCertificate(), pair2.getCertificate()})
												 << QList<QSslCertificate>({derived1.getCertificate(), pair1.getCertificate()});

		}


		void sslSocket()
		{
			QFETCH(KeyPair, serverPair);
			QFETCH(KeyPair, clientPair);
			QFETCH(QList<QSslCertificate>, serverCaStore);
			QFETCH(QList<QSslCertificate>, clientCaStore);

			QSslConfiguration configServer;
			configServer.setPrivateKey(serverPair.getKey());
			configServer.setLocalCertificate(serverPair.getCertificate());
			configServer.setCaCertificates(serverCaStore);
			configServer.setPeerVerifyMode(QSslSocket::VerifyPeer);
			QSslServer server;
			server.setSslConfiguration(configServer);
			QVERIFY(server.listen());

			QSslConfiguration configClient;
			configClient.setPrivateKey(clientPair.getKey());
			configClient.setLocalCertificate(clientPair.getCertificate());
			configClient.setCaCertificates(clientCaStore);
			QList<QSslError> allowedErrors;
			allowedErrors << QSslError(QSslError::HostNameMismatch, serverPair.getCertificate());
			QSslSocket client;
			client.ignoreSslErrors(allowedErrors);
			client.setSslConfiguration(configClient);

			QSignalSpy clientEncrypted(&client, &QSslSocket::encrypted);

			connect(&server, &QSslServer::peerVerifyError, this, [](QSslSocket* pSocket, const QSslError& pError){
					QVERIFY(pSocket);
					QCOMPARE(pError.error(), QSslError::NoError);
				});
			connect(&server, &QSslServer::sslErrors, this, [](QSslSocket* pSocket, const QList<QSslError>& pErrors){
					QVERIFY(pSocket);
					QCOMPARE(pErrors, (QList<QSslError>()));
				});
			connect(&client, &QSslSocket::sslErrors, this, [&allowedErrors](const QList<QSslError>& pErrors){
					QCOMPARE(pErrors, allowedErrors);
				});
			client.connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());

			QTRY_COMPARE(clientEncrypted.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_KeyPair)
#include "test_KeyPair.moc"
