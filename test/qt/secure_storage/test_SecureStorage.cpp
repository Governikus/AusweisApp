/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref SecureStorage
 */

#include "SecureStorage.h"

#include "ResourceLoader.h"
#include "asn1/CVCertificate.h"

#include <QCoreApplication>
#include <QFile>
#include <QSslCertificateExtension>
#include <QSslKey>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_METATYPE(SecureStorage::TlsSuite)
Q_DECLARE_METATYPE(QSsl::SslProtocol)
Q_DECLARE_METATYPE(QSsl::KeyAlgorithm)

class test_SecureStorage
	: public QObject
{
	Q_OBJECT

	private:
		QStringList loadCommentList(const QString& pCommentName)
		{
			auto* storage = Env::getSingleton<SecureStorage>();
			const auto& config = storage->loadFile({storage->getEmbeddedConfig()});
			if (config.isEmpty())
			{
				return QStringList();
			}

			const QJsonValue commentValues = config.value(pCommentName);
			if (commentValues == QJsonValue::Undefined || !commentValues.isArray())
			{
				qCritical() << "Wasn't able to find the array" << pCommentName;
				return QStringList();
			}

			QStringList comments;
			const auto& commentValueArray = commentValues.toArray();
			for (const QJsonValueConstRef comment : commentValueArray)
			{
				if (comment.isString())
				{
					comments += comment.toString();
				}
				else
				{
					qCritical() << "At least one entry of" << pCommentName << "is not a string";
					return QStringList();
				}
			}

			return comments;
		}

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void testGetCVRootCertificatesUnique()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();
			static const int EXPECTED_CERTIFICATE_COUNT = 19;

			QList<QSharedPointer<const CVCertificate>> cvcs = CVCertificate::fromRaw(secureStorage->getCVRootCertificates(true))
					+ CVCertificate::fromRaw(secureStorage->getCVRootCertificates(false));
			const auto count = cvcs.count();
			QCOMPARE(count, EXPECTED_CERTIFICATE_COUNT);

			const QStringList comments = loadCommentList("_comment_2"_L1) + loadCommentList("_comment_4"_L1);
			QCOMPARE(comments.size(), count);

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
						qWarning() << "Found 2 certificates with the same name with comments:" << comments[j] << comments[i];
					}

					QVERIFY(carI != carJ || chrI != chrJ);
				}
			}
		}


		void testGetCVRootCertificateNames_data()
		{
			QTest::addColumn<int>("certificateCount");
			QTest::addColumn<bool>("isProductive");
			QTest::addColumn<QString>("commentName");

			QTest::newRow("production") << 6 << true << "_comment_2";
			QTest::newRow("test") << 13 << false << "_comment_4";
		}


		void testGetCVRootCertificateNames()
		{
			QFETCH(int, certificateCount);
			QFETCH(bool, isProductive);
			QFETCH(QString, commentName);

			const auto secureStorage = Env::getSingleton<SecureStorage>();

			QList<QSharedPointer<const CVCertificate>> cvcs = CVCertificate::fromRaw(secureStorage->getCVRootCertificates(isProductive));
			QCOMPARE(cvcs.count(), certificateCount);

			const QStringList& comments = loadCommentList(commentName);
			QCOMPARE(comments.count(), certificateCount);

			for (int index = 0; index < certificateCount; ++index)
			{
				const QString commentLine = comments[index];

				const QStringList carAndChr = commentLine.split(QStringLiteral("_"));
				QVERIFY(carAndChr.size() == 1 || carAndChr.size() == 2);

				const QByteArray carInComment = carAndChr.first().toUtf8();
				const QByteArray carInCertificate = cvcs.at(index)->getBody().getCertificationAuthorityReference();
				const QByteArray chrInComment = carAndChr.size() == 1 ? carAndChr.first().toUtf8() : carAndChr.last().toUtf8();
				const QByteArray chrInCertificate = cvcs.at(index)->getBody().getCertificateHolderReference();

				if (carInComment != carInCertificate || chrInComment != chrInCertificate)
				{
					qWarning() << "Wrong certificate information at index" << index << "with comment" << commentLine;
				}

				QCOMPARE(carInComment, carInCertificate);
				QCOMPARE(chrInComment, chrInCertificate);
			}
		}


		void testGetUpdateCertificate_data()
		{
			QTest::addColumn<int>("index");
			QTest::addColumn<QString>("subjectInfo");
			QTest::addColumn<QString>("issuerInfo");
			QTest::addColumn<QString>("effectiveDate");
			QTest::addColumn<QString>("expiryDate");
			QTest::addColumn<int>("length");
			QTest::addColumn<QSsl::KeyAlgorithm>("algorithm");
			QTest::addColumn<QString>("ocsp");

			int index = 0;

			QTest::newRow("production_2027") << index++
											 << "updates.autentapp.de"
											 << "Governikus CA 9:PN"
											 << "2021-12-06T10:18:22Z"
											 << "2027-12-31T23:59:00Z"
											 << 384
											 << QSsl::Ec
											 << "http://pki.governikus.com/ejbca/publicweb/status/ocsp";

			QTest::newRow("production_2031") << index++
											 << "updates.autentapp.de"
											 << "Governikus CA 9:PN"
											 << "2027-01-01T00:00:00Z"
											 << "2031-12-31T23:59:00Z"
											 << 384
											 << QSsl::Ec
											 << "http://pki.governikus.com/ejbca/publicweb/status/ocsp";

			QTest::newRow("production_ca") << index++
										   << "Governikus CA 9:PN"
										   << "Governikus Root CA 4:PN"
										   << "2019-09-19T14:13:51Z"
										   << "2034-09-15T14:13:51Z"
										   << 4096
										   << QSsl::Rsa
										   << "http://pki.governikus.com/publicweb/status/ocsp";

			QTest::newRow("production_ca_root") << index++
												<< "Governikus Root CA 4:PN"
												<< "Governikus Root CA 4:PN"
												<< "2019-09-16T09:45:19Z"
												<< "2039-09-11T09:45:19Z"
												<< 4096
												<< QSsl::Rsa
												<< "";
		}


		void testGetUpdateCertificate()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();
			const auto& certificates = secureStorage->getUpdateCertificates();
			QCOMPARE(certificates.count(), 4);

			QFETCH(int, index);
			QFETCH(QString, subjectInfo);
			QFETCH(QString, issuerInfo);
			QFETCH(QString, effectiveDate);
			QFETCH(QString, expiryDate);
			QFETCH(int, length);
			QFETCH(QSsl::KeyAlgorithm, algorithm);
			QFETCH(QString, ocsp);

			QVERIFY(certificates.count() - index > 0);

			const auto& cert = certificates.at(index);
			QCOMPARE(cert.subjectInfo(QSslCertificate::CommonName).at(0), subjectInfo);
			QCOMPARE(cert.issuerInfo(QSslCertificate::CommonName).at(0), issuerInfo);
			QCOMPARE(cert.effectiveDate(), QDateTime::fromString(effectiveDate, Qt::ISODate));
			QCOMPARE(cert.expiryDate(), QDateTime::fromString(expiryDate, Qt::ISODate));
			QCOMPARE(cert.publicKey().length(), length);
			QCOMPARE(cert.publicKey().algorithm(), algorithm);
			QCOMPARE(cert.publicKey().type(), QSsl::PublicKey);

			const auto& ext = cert.extensions();
			const bool foundOcspResponder = std::any_of(ext.cbegin(), ext.cend(), [&ocsp](const auto& entry){
					return entry.name() == QLatin1String("authorityInfoAccess") &&
						   entry.value().toMap().value(QStringLiteral("OCSP")).toString() == ocsp;
				});
			QCOMPARE(foundOcspResponder, !ocsp.isEmpty());
		}


		void testGetSelfAuthentication()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();
			QVERIFY(secureStorage->getSelfAuthenticationUrl(false).isValid());
			QVERIFY(secureStorage->getSelfAuthenticationUrl(true).isValid());
		}


		void testGetUpdateServerBaseUrl()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();
			QVERIFY(secureStorage->getUpdateServerBaseUrl().isValid());
		}


		void testWhitelistServerBaseUrl()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();
			QVERIFY(secureStorage->getWhitelistServerBaseUrl().isValid());
		}


		void testSmartPersonalizationUrl()
		{
			const auto& secureStorage = Env::getSingleton<SecureStorage>();

			if (secureStorage->getVendor().contains(QLatin1String("Governikus")))
			{
				QVERIFY(secureStorage->getSmartPersonalizationUrl().contains(QLatin1String("/%1")));
				QVERIFY(secureStorage->getSmartPersonalizationUrl(true).contains(QLatin1String("/%1")));
			}
			else
			{
				QVERIFY(secureStorage->getSmartPersonalizationUrl().isEmpty());
				QVERIFY(secureStorage->getSmartPersonalizationUrl(true).isEmpty());
			}
		}


		void testSmartService()
		{
			const auto& secureStorage = Env::getSingleton<SecureStorage>();

			if (secureStorage->getVendor().contains(QLatin1String("Governikus")))
			{
				QVERIFY(!secureStorage->getSmartServiceId().isEmpty());
				QVERIFY(!secureStorage->getSmartSsdAid().isEmpty());
			}
			else
			{
				QVERIFY(secureStorage->getSmartServiceId().isEmpty());
				QVERIFY(secureStorage->getSmartSsdAid().isEmpty());
			}
		}


		void testLocalIfdConfig()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();
			QCOMPARE(secureStorage->getLocalIfdPackageName(), "com.governikus.ausweisapp2.dev"_L1);
			QCOMPARE(secureStorage->getLocalIfdMinVersion(), "1.100.0"_L1);
			QCOMPARE(secureStorage->getLocalIfdMinPskSize(), 256);

			const auto& certificateHashes = secureStorage->getLocalIfdAllowedCertificateHashes();
			QCOMPARE(certificateHashes.size(), 3);
			QVERIFY(certificateHashes.contains(QByteArray::fromHex(QByteArrayLiteral("B02AC76B50A497AE810AEAC22598187B3D4290277D0851A7FA8E1AEA5A979870"))));
			QVERIFY(certificateHashes.contains(QByteArray::fromHex(QByteArrayLiteral("F4A4D85A22103EBB5F4D35AEDE5117F40E591AB5DDF43DF39C953D08E3895138"))));
			QVERIFY(certificateHashes.contains(QByteArray::fromHex(QByteArrayLiteral("F96FD6BBA899845E06D3E6522F0843217681D473B6B09F1E313DEA1A21D6B8E7"))));
		}


		void testAppcast()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();
			QCOMPARE(secureStorage->getAppcastUpdateUrl(), QUrl("https://updates.autentapp.de/AppcastInfo.json"_L1));
			QCOMPARE(secureStorage->getAppcastBetaUpdateUrl(), QUrl("https://updates.autentapp.de/beta/AppcastInfo.json"_L1));
		}


		void testMinKeySizes()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();
			QCOMPARE(secureStorage->getMinimumKeySize(QSsl::KeyAlgorithm::Rsa), 2000);
			QCOMPARE(secureStorage->getMinimumKeySize(QSsl::KeyAlgorithm::Dsa), 2000);
			QCOMPARE(secureStorage->getMinimumKeySize(QSsl::KeyAlgorithm::Dh), 2000);
			QCOMPARE(secureStorage->getMinimumKeySize(QSsl::KeyAlgorithm::Ec), 250);
		}


		void testSignatureAlgorithms()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();

			const auto& tlsSettings = secureStorage->getTlsConfig();
			QCOMPARE(tlsSettings.getSignatureAlgorithms().size(), 9);
			QCOMPARE(tlsSettings.getSignatureAlgorithms().constFirst(), QByteArray("RSA+SHA512"));
			QCOMPARE(tlsSettings.getSignatureAlgorithms().constLast(), QByteArray("ECDSA+SHA256"));
		}


		void testSignatureAlgorithmsPsk()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();

			const auto& tlsSettings = secureStorage->getTlsConfig(SecureStorage::TlsSuite::PSK);
			QCOMPARE(tlsSettings.getSignatureAlgorithms().size(), 3);
			QCOMPARE(tlsSettings.getSignatureAlgorithms().constFirst(), QByteArray("RSA+SHA512"));
			QCOMPARE(tlsSettings.getSignatureAlgorithms().constLast(), QByteArray("RSA+SHA256"));
		}


		void testSignatureAlgorithmsRemoteReader()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();

			const auto& config = secureStorage->getTlsConfigRemoteIfd();
			QCOMPARE(config.getSignatureAlgorithms().size(), 3);

			const auto& configPairing = secureStorage->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK);
			QCOMPARE(configPairing.getSignatureAlgorithms().size(), 3);

			const auto& configLocalIfd = secureStorage->getTlsConfigLocalIfd();
			QCOMPARE(configLocalIfd.getSignatureAlgorithms().size(), 3);
		}


		void orderOfCiphers()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();
			const auto& ciphersForwardSecrecy = secureStorage->getTlsConfig().getCiphers();
			QCOMPARE(ciphersForwardSecrecy.first(), QSslCipher("ECDHE-ECDSA-AES256-GCM-SHA384"_L1));
			QCOMPARE(ciphersForwardSecrecy.last(), QSslCipher("DHE-RSA-AES128-SHA256"_L1));

			const auto& ciphersPsk = secureStorage->getTlsConfig(SecureStorage::TlsSuite::PSK).getCiphers();
			QVERIFY(ciphersPsk.count() > 0);
			QCOMPARE(ciphersPsk.first(), QSslCipher("RSA-PSK-AES256-GCM-SHA384"_L1));
			QCOMPARE(ciphersPsk.last(), QSslCipher("RSA-PSK-AES256-CBC-SHA"_L1));

			const auto& ciphersEc = secureStorage->getTlsConfig().getEllipticCurves();
			QCOMPARE(ciphersEc.count(), 5);
			QCOMPARE(ciphersEc.first(), QSslEllipticCurve::fromLongName("brainpoolP512r1"_L1));
			QCOMPARE(ciphersEc.last(), QSslEllipticCurve::fromLongName("prime256v1"_L1));

			const auto& ciphersEcRemoteReader = secureStorage->getTlsConfigRemoteIfd().getEllipticCurves();
			QCOMPARE(ciphersEcRemoteReader.count(), 5);
			QCOMPARE(ciphersEcRemoteReader.first(), QSslEllipticCurve::fromLongName("brainpoolP512r1"_L1));
			QCOMPARE(ciphersEcRemoteReader.last(), QSslEllipticCurve::fromLongName("prime256v1"_L1));

			const auto& ciphersEcRemoteReaderPairing = secureStorage->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getEllipticCurves();
			QCOMPARE(ciphersEcRemoteReaderPairing.count(), 0);

			const auto& ciphersRemoteReader = secureStorage->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getCiphers();
			QVERIFY(ciphersRemoteReader.count() > 0);
			QCOMPARE(ciphersRemoteReader.first(), QSslCipher("RSA-PSK-AES256-GCM-SHA384"_L1));
			QCOMPARE(ciphersRemoteReader.last(), QSslCipher("RSA-PSK-AES256-CBC-SHA"_L1));

			const auto& localIfdConfig = secureStorage->getTlsConfigLocalIfd();
			const auto& ciphersEcLocalIfd = localIfdConfig.getEllipticCurves();
			QCOMPARE(ciphersEcLocalIfd.count(), 5);
			QCOMPARE(ciphersEcLocalIfd.first(), QSslEllipticCurve::fromLongName("brainpoolP512r1"_L1));
			QCOMPARE(ciphersEcLocalIfd.last(), QSslEllipticCurve::fromLongName("prime256v1"_L1));
			const auto& ciphersLocalIfd = localIfdConfig.getCiphers();
			QVERIFY(ciphersLocalIfd.count() == 2);
			QCOMPARE(ciphersLocalIfd.first(), QSslCipher("ECDHE-PSK-AES128-CBC-SHA256"_L1));
			QCOMPARE(ciphersLocalIfd.last(), QSslCipher("ECDHE-PSK-AES256-CBC-SHA384"_L1));
		}


		void getSslProtocolVersion_data()
		{
			QTest::addColumn<SecureStorage::TlsSuite>("suite");
			QTest::addColumn<QSsl::SslProtocol>("protocol");

			QTest::newRow("default") << SecureStorage::TlsSuite::DEFAULT << QSsl::SslProtocol::TlsV1_2;
			QTest::newRow("PSK") << SecureStorage::TlsSuite::PSK << QSsl::SslProtocol::TlsV1_2;
		}


		void getSslProtocolVersion()
		{
			QFETCH(SecureStorage::TlsSuite, suite);
			QFETCH(QSsl::SslProtocol, protocol);

			const auto secureStorage = Env::getSingleton<SecureStorage>();

			QCOMPARE(secureStorage->getTlsConfig(suite).getProtocolVersion(), protocol);
		}


		void getConfiguration_data()
		{
			QTest::addColumn<QSslConfiguration>("configuration");
			QTest::addColumn<int>("cipherSize");

			const auto secureStorage = Env::getSingleton<SecureStorage>();

			QTest::newRow("ciphers non PSK") << secureStorage->getTlsConfig().getConfiguration() << 12;
			QTest::newRow("ciphers for PSK") << secureStorage->getTlsConfig(SecureStorage::TlsSuite::PSK).getConfiguration() << 5;
			QTest::newRow("remote ifd") << secureStorage->getTlsConfigRemoteIfd().getConfiguration() << 7;
			QTest::newRow("remote ifd pairing") << secureStorage->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getConfiguration() << 5;
			QTest::newRow("local ifd") << secureStorage->getTlsConfigLocalIfd().getConfiguration() << 2;
		}


		void getConfiguration()
		{
			QFETCH(QSslConfiguration, configuration);
			QFETCH(int, cipherSize);

			QCOMPARE(configuration.ciphers().size(), cipherSize);
		}


		void testSizesIfd()
		{
			const auto secureStorage = Env::getSingleton<SecureStorage>();

			QCOMPARE(secureStorage->getIfdCreateSize(), 2048);
			QCOMPARE(secureStorage->getMinimumIfdKeySize(QSsl::KeyAlgorithm::Rsa), 2000);
			QCOMPARE(secureStorage->getMinimumIfdKeySize(QSsl::KeyAlgorithm::Dsa), 2000);
			QCOMPARE(secureStorage->getMinimumIfdKeySize(QSsl::KeyAlgorithm::Dh), 2000);
			QCOMPARE(secureStorage->getMinimumIfdKeySize(QSsl::KeyAlgorithm::Ec), 250);

		}


};

QTEST_GUILESS_MAIN(test_SecureStorage)
#include "test_SecureStorage.moc"
