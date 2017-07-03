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
			QCOMPARE(cvcs.count(), 4);
		}


		void testGetCVRootCertificatesTest()
		{
			QVector<QSharedPointer<CVCertificate> > cvcs = CVCertificate::fromHex(secureStorage.getCVRootCertificates(false));
			QCOMPARE(cvcs.count(), 4);
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
			QCOMPARE(secureStorage.getAppcastUpdateUrl(), QUrl("https://appl.governikus-asp.de/ausweisapp2/Appcast.xml"));
			QCOMPARE(secureStorage.getAppcastBetaUpdateUrl(), QUrl("https://appl.governikus-asp.de/ausweisapp2/beta/Appcast.xml"));
		}


		void testSignatureAlgorithms()
		{
			QCOMPARE(secureStorage.getSignatureAlgorithms().size(), 12);
			QCOMPARE(secureStorage.getSignatureAlgorithms().first().first, QSsl::KeyAlgorithm::Rsa);
			QCOMPARE(secureStorage.getSignatureAlgorithms().first().second, QCryptographicHash::Algorithm::Sha512);
			QCOMPARE(secureStorage.getSignatureAlgorithms().last().first, QSsl::KeyAlgorithm::Ec);
			QCOMPARE(secureStorage.getSignatureAlgorithms().last().second, QCryptographicHash::Algorithm::Sha224);
		}


		void testSignatureAlgorithmsPsk()
		{
			QCOMPARE(secureStorage.getSignatureAlgorithmsPsk().size(), 4);
			QCOMPARE(secureStorage.getSignatureAlgorithmsPsk().first().first, QSsl::KeyAlgorithm::Rsa);
			QCOMPARE(secureStorage.getSignatureAlgorithmsPsk().first().second, QCryptographicHash::Algorithm::Sha512);
			QCOMPARE(secureStorage.getSignatureAlgorithmsPsk().last().first, QSsl::KeyAlgorithm::Rsa);
			QCOMPARE(secureStorage.getSignatureAlgorithmsPsk().last().second, QCryptographicHash::Algorithm::Sha224);
		}


		void orderOfCiphers()
		{
			const auto& ciphersForwardSecrecy = secureStorage.getCiphersWithForwardSecrecy();
			QCOMPARE(ciphersForwardSecrecy.count(), 16);
			QCOMPARE(ciphersForwardSecrecy.first(), QStringLiteral("ECDHE-ECDSA-AES256-GCM-SHA384"));
			QCOMPARE(ciphersForwardSecrecy.last(), QStringLiteral("DHE-RSA-AES128-SHA256"));

			const auto& ciphersPsk = secureStorage.getCiphersWithPsk();
			QCOMPARE(ciphersPsk.count(), 5);
			QCOMPARE(ciphersPsk.first(), QStringLiteral("RSA-PSK-AES256-GCM-SHA384"));
			QCOMPARE(ciphersPsk.last(), QStringLiteral("RSA-PSK-AES256-CBC-SHA"));

			const auto& ciphersEc = secureStorage.getAllowedSslEllipticCurves();
			QCOMPARE(ciphersEc.count(), 6);
			QCOMPARE(ciphersEc.first(), QStringLiteral("brainpoolP512r1"));
			QCOMPARE(ciphersEc.last(), QStringLiteral("secp224r1"));
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
			QCOMPARE(secureStorage.getSslProtocolVersion(), QSsl::SslProtocol::TlsV1_0OrLater);
		}


		void getSslProtocolVersionPsk()
		{
			QCOMPARE(secureStorage.getSslProtocolVersionPsk(), QSsl::SslProtocol::TlsV1_1OrLater);
		}


};

QTEST_GUILESS_MAIN(test_SecureStorage)
#include "test_SecureStorage.moc"
