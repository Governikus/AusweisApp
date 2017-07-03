/*!
 * \brief Unit tests for \ref SecureStorage
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include <QCoreApplication>
#include <QFile>
#include <QtTest>

#include "SecureStorage.h"

#include "asn1/CVCertificate.h"

using namespace governikus;

class test_SecureStorage
	: public QObject
{
	Q_OBJECT
	SecureStorage secureStorage;

	private Q_SLOTS:
		void initTestCase()
		{
			AbstractSettings::mTestDir.clear();
			secureStorage.load();
		}


		void unloaded()
		{
			SecureStorage store1;
			SecureStorage store2;
			QCOMPARE(store1, store2);
			store1.load();
			QVERIFY(store1 != store2);
			store2.load();
			QCOMPARE(store1, store2);
		}


		void testGetCVRootCertificates()
		{
			QVector<QSharedPointer<CVCertificate> > cvcs = CVCertificate::fromHex(secureStorage.getCVRootCertificates(true));
			cvcs += CVCertificate::fromHex(secureStorage.getCVRootCertificates(false));

			const int count = cvcs.count();
			QCOMPARE(count, 10);

			// Check that each certificate has a unique car/chr.
			for (int j = 0; j < count; ++j)
			{
				for (int i = 0; i < j; ++i)
				{
					const CVCertificateBody& bodyI = cvcs[i]->getBody();
					const QByteArray carI = bodyI.getCertificationAuthorityReference();
					const QByteArray chrI = bodyI.getCertificateHolderReference();

					const CVCertificateBody& bodyJ = cvcs[j]->getBody();
					const QByteArray carJ = bodyJ.getCertificationAuthorityReference();
					const QByteArray chrJ = bodyJ.getCertificateHolderReference();

					if (carI == carJ && chrI == chrJ)
					{
						qWarning() << "certificate" << i << "and certificate" << j << "have the same car/chr";
					}

					QVERIFY(carI != carJ || chrI != chrJ);
				}
			}
		}


		void testGetUpdateCertificate()
		{
			QVector<QSslCertificate> certificates = secureStorage.getUpdateCertificates();
			QCOMPARE(certificates.count(), 3);
		}


		void testGetSelfAuthentication()
		{
			QVERIFY(secureStorage.getSelfAuthenticationUrl(false).isValid());
			QVERIFY(secureStorage.getSelfAuthenticationUrl(true).isValid());
			QVERIFY(!secureStorage.getSelfAuthenticationCertDescr(false).isEmpty());
			QVERIFY(!secureStorage.getSelfAuthenticationCertDescr(true).isEmpty());
		}


		void testGetProvidersUpdateUrl()
		{
			QVERIFY(secureStorage.getProviderUpdateUrl().isValid());
		}


		void testAppcast()
		{
			QCOMPARE(secureStorage.getAppcastUpdateUrl(), QUrl("https://appl.governikus-asp.de/ausweisapp2/Appcast.json"));
			QCOMPARE(secureStorage.getAppcastBetaUpdateUrl(), QUrl("https://appl.governikus-asp.de/ausweisapp2/beta/Appcast.json"));
		}


		void testMinStaticKeySizes()
		{
			QCOMPARE(secureStorage.getMinimumStaticKeySize(QSsl::KeyAlgorithm::Rsa), 2000);
			QCOMPARE(secureStorage.getMinimumStaticKeySize(QSsl::KeyAlgorithm::Dsa), 2000);
			QCOMPARE(secureStorage.getMinimumStaticKeySize(QSsl::KeyAlgorithm::Ec), 224);
		}


		void testMinEphemeralKeySizes()
		{
			QCOMPARE(secureStorage.getMinimumEphemeralKeySize(QSsl::KeyAlgorithm::Rsa), 2000);
			QCOMPARE(secureStorage.getMinimumEphemeralKeySize(QSsl::KeyAlgorithm::Dsa), 1024);
			QCOMPARE(secureStorage.getMinimumEphemeralKeySize(QSsl::KeyAlgorithm::Ec), 224);
		}


		void testSignatureAlgorithms()
		{
			const auto& tlsSettings = secureStorage.getTlsSettings();
			QCOMPARE(tlsSettings.getSignatureAlgorithms().size(), 12);
			QCOMPARE(tlsSettings.getSignatureAlgorithms().constFirst().first, QSsl::KeyAlgorithm::Rsa);
			QCOMPARE(tlsSettings.getSignatureAlgorithms().constFirst().second, QCryptographicHash::Algorithm::Sha512);
			QCOMPARE(tlsSettings.getSignatureAlgorithms().constLast().first, QSsl::KeyAlgorithm::Ec);
			QCOMPARE(tlsSettings.getSignatureAlgorithms().constLast().second, QCryptographicHash::Algorithm::Sha224);
		}


		void testSignatureAlgorithmsPsk()
		{
			const auto& tlsSettings = secureStorage.getTlsSettingsPsk();
			QCOMPARE(tlsSettings.getSignatureAlgorithms().size(), 4);
			QCOMPARE(tlsSettings.getSignatureAlgorithms().constFirst().first, QSsl::KeyAlgorithm::Rsa);
			QCOMPARE(tlsSettings.getSignatureAlgorithms().constFirst().second, QCryptographicHash::Algorithm::Sha512);
			QCOMPARE(tlsSettings.getSignatureAlgorithms().constLast().first, QSsl::KeyAlgorithm::Rsa);
			QCOMPARE(tlsSettings.getSignatureAlgorithms().constLast().second, QCryptographicHash::Algorithm::Sha224);
		}


		void orderOfCiphers()
		{
			const auto& ciphersForwardSecrecy = secureStorage.getTlsSettings().getCiphers();
			QCOMPARE(ciphersForwardSecrecy.count(), 24);
			QCOMPARE(ciphersForwardSecrecy.first(), QSslCipher("ECDHE-ECDSA-AES256-GCM-SHA384"));
			QCOMPARE(ciphersForwardSecrecy.last(), QSslCipher("DHE-RSA-AES128-SHA"));

			const auto& ciphersPsk = secureStorage.getTlsSettingsPsk().getCiphers();
			QCOMPARE(ciphersPsk.count(), 5);
			QCOMPARE(ciphersPsk.first(), QSslCipher("RSA-PSK-AES256-GCM-SHA384"));
			QCOMPARE(ciphersPsk.last(), QSslCipher("RSA-PSK-AES256-CBC-SHA"));

			const auto& ciphersEc = secureStorage.getTlsSettings().getEllipticCurves();
			QCOMPARE(ciphersEc.count(), 6);
			QCOMPARE(ciphersEc.first(), QSslEllipticCurve::fromLongName("brainpoolP512r1"));
			QCOMPARE(ciphersEc.last(), QSslEllipticCurve::fromLongName("secp224r1"));
		}


		void loadChangedFileOnly()
		{
			QBENCHMARK
			{
				secureStorage.load();
			}
		}


		void getSslProtocolVersion()
		{
			QCOMPARE(secureStorage.getTlsSettings().getProtocolVersion(), QSsl::SslProtocol::TlsV1_0OrLater);
		}


		void getSslProtocolVersionPsk()
		{
			QCOMPARE(secureStorage.getTlsSettingsPsk().getProtocolVersion(), QSsl::SslProtocol::TlsV1_1OrLater);
		}


		void getConfiguration_data()
		{
			QTest::addColumn<QSslConfiguration>("configuration");
			QTest::addColumn<int>("cipherSize");

			QTest::newRow("ciphers non PSK") << secureStorage.getTlsSettings().getConfiguration() << 24;
			QTest::newRow("ciphers for PSK") << secureStorage.getTlsSettingsPsk().getConfiguration() << 5;
		}


		void getConfiguration()
		{
			QFETCH(QSslConfiguration, configuration);
			QFETCH(int, cipherSize);

			QCOMPARE(configuration.ciphers().size(), cipherSize);
		}


};

QTEST_GUILESS_MAIN(test_SecureStorage)
#include "test_SecureStorage.moc"
