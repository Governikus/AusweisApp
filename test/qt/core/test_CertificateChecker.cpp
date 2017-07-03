/*!
 * \brief Unit tests for \ref CertificateChecker
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "AppSettings.h"
#include "CertificateChecker.h"
#include "LogHandler.h"
#include "SecureStorage.h"
#include "context/AuthContext.h"

#include "MockActivationContext.h"
#include "TestFileHelper.h"

#include <QSslKey>
#include <QtTest>
#include <openssl/pem.h>

using namespace governikus;

Q_DECLARE_METATYPE(QSsl::KeyAlgorithm)

class test_CertificateChecker
	: public QObject
{
	Q_OBJECT

	QVector<QSslCertificate> certs;

	static QSslKey createQSslKeyWithHandle(const QByteArray& pPemEncodedEvpPKey)
	{
		BIO* bio = BIO_new(BIO_s_mem());
		BIO_write(bio, pPemEncodedEvpPKey.constData(), pPemEncodedEvpPKey.length());
		QSslKey key(PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr), QSsl::PublicKey);
		BIO_free(bio);
		return key;
	}


	private Q_SLOTS:
		void initTestCase()
		{
			LogHandler::getInstance().init();
			AppSettings::getInstance().load();
			certs = AppSettings::getInstance().getSecureStorage().getUpdateCertificates();
			QVERIFY(certs.size() > 0);
		}


		void cleanup()
		{
			LogHandler::getInstance().resetBacklog();
		}


		void validUpdateCert()
		{
			QSharedPointer<AuthContext> model(new AuthContext(new MockActivationContext()));

			QCOMPARE(model->getCertificateList().size(), 0);
			QCOMPARE(CertificateChecker::checkAndSaveCertificate(certs.at(0), QUrl("dummy"), model), CertificateChecker::CertificateStatus::Good);
			QCOMPARE(model->getCertificateList().size(), 1);
		}


		void hasUpdateCertificateKeyLength()
		{
			for (const auto& cert : qAsConst(certs))
			{
				QVERIFY(CertificateChecker::hasValidCertificateKeyLength(cert));
			}
		}


		void hasCertificateKeyLength_data()
		{
			QTest::addColumn<QString>("filename");
			QTest::addColumn<QString>("output");

			QTest::newRow("rsa") << ":/core/invalid.keysize.rsa.der" << "Rsa key with insufficient key size found 1024";
			QTest::newRow("dsa") << ":/core/invalid.keysize.dsa.der" << "Dsa key with insufficient key size found 1024";
			QTest::newRow("ec") << ":/core/invalid.keysize.ec.der" << "Ec key with insufficient key size found 128";
		}


		void hasCertificateKeyLength()
		{
			QFETCH(QString, filename);
			QFETCH(QString, output);

			const auto& content = TestFileHelper::readFile(filename);
			QVERIFY(!content.isEmpty());
			QSslCertificate invalidCert(content);
			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);
			QVERIFY(!CertificateChecker::hasValidCertificateKeyLength(invalidCert));

			QCOMPARE(spy.count(), 3);
			auto param = spy.takeLast();
			QVERIFY(param.at(0).toString().contains(output));
		}


		void checkCertificateHash()
		{
			QVERIFY(!CertificateChecker::checkCertificate(certs.at(0), QCryptographicHash::Algorithm::Sha256, QSet<QString>() << "dummy" << "bla bla"));
			QVERIFY(CertificateChecker::checkCertificate(certs.at(0), QCryptographicHash::Algorithm::Sha256, QSet<QString>() << "dummy" << "4381E8E49218524CB6A59C26379B1234C32DDBCB04A12988DE249C098FF32ED7" << "bla bla"));
		}


		void hasEmptyEphemeralKey()
		{
			QSslKey key;
			QVERIFY(!CertificateChecker::hasValidEphemeralKeyLength(key));
		}


		void hasValidEphemeralKeyLength_data()
		{
			QTest::addColumn<QByteArray>("pem");
			QTest::addColumn<QSsl::KeyAlgorithm>("algorithm");
			QTest::addColumn<bool>("sufficient");

			/*
			 * openssl ecparam -name secp112r2 -out secp112r2_param.pem
			 * openssl ecparam -in secp112r2_param.pem -genkey -noout -out secp112r2_key.pem
			 * openssl ec -in secp112r2_key.pem -pubout -out secp112r2_pubkey.pem
			 */
			QByteArray ec112("-----BEGIN PUBLIC KEY-----\n"
							 "MDIwEAYHKoZIzj0CAQYFK4EEAAcDHgAEWo89aCax3oUWJho7rFZ1u70WqghvA7Tf\n"
							 "SXXiZw==\n"
							 "-----END PUBLIC KEY-----");

			QTest::newRow("ec112") << ec112 << QSsl::KeyAlgorithm::Ec << false;

			/*
			 * openssl ecparam -name secp521r1 -out secp521r1_param.pem
			 * openssl ecparam -in secp521r1_param.pem -genkey -noout -out secp521r1_key.pem
			 * openssl ec -in secp521r1_key.pem -pubout -out secp521r1_pubkey.pem
			 */
			QByteArray ec521("-----BEGIN PUBLIC KEY-----\n"
							 "MIGbMBAGByqGSM49AgEGBSuBBAAjA4GGAAQBi5e64Y/7EGT7Tbpe5h4cZGSpNidN\n"
							 "fiPz6F/aG1yApWkTgqVoQTUCahP851skkDI6PzHedE67CR2KPJ8VNt6WmuIAc4Cg\n"
							 "zBwxpgBC09TO/3D8fS70xVqBX6dzA4lO9MUZCqgBMt2LTFpklUabviy657kcRQ+H\n"
							 "tTAy2sDy+bhcj1UyWlE=\n"
							 "-----END PUBLIC KEY-----");
			QTest::newRow("ec521") << ec521 << QSsl::KeyAlgorithm::Ec << true;

			/*
			 * openssl dsaparam -out dsa768_param.pem 1024
			 * openssl gendsa -out dsa768_key.pem dsa768_param.pem
			 * openssl dsa -in dsa768_key.pem -pubout -out dsa768_pubkey.pem
			 */
			QByteArray dsa768("-----BEGIN PUBLIC KEY-----\n"
							  "MIIBUTCB6AYHKoZIzjgEATCB3AJhAMZ8q4KifsiCkTcL/qaqrghMK1Ah2EC6cO3b\n"
							  "a0Dq1FhA7cEQpe0YVcFhcnjd6CCEGz+Zl5nlHGKpgrjdBUZZP66JIFy/WhUb2adZ\n"
							  "HRcswrSlE/NQS+sPH3CA0WddlcLaDwIVAKsBnl++V6aRAvYR9HVxr5LajwnVAmBq\n"
							  "7heqwtm9j317cWptXSwR1jKKWPqkOXzaxSVG1PhEEGXTq6PeQgoBRivzbE+/Ome8\n"
							  "DqfcBsIOCMc47bupEUqK38fmnALGLaig7cup3njaNkxyuoQPngtb0qUiT0dy7dED\n"
							  "ZAACYQCz1c7kF720wMi9SnrpPD3mZWsC3dqOn5r8EwxanHnkf4KyPctawqGmhZsK\n"
							  "b3586zIW9Fdg/Rq4yda8GzMLVAtF540s3k5RmsJvnxPEGXlc3c+wEOKR/iHwPrg4\n"
							  "DNXOVys=\n"
							  "-----END PUBLIC KEY-----");
			QTest::newRow("dsa768") << dsa768 << QSsl::KeyAlgorithm::Dsa << false;

			/*
			 * openssl dsaparam -out dsa1024_param.pem 1024
			 * openssl gendsa -out dsa1024_key.pem dsa1024_param.pem
			 * openssl dsa -in dsa1024_key.pem -pubout -out dsa1024_pubkey.pem
			 */
			QByteArray dsa1024("-----BEGIN PUBLIC KEY-----\n"
							   "MIIBtzCCASwGByqGSM44BAEwggEfAoGBAMGEyYRk7Hj45r/1vnY4FFIPSLGDnq6g\n"
							   "60T514zhDaL3A9qXdy+U/i2zm+wscG0mP+9SuznBpTRCM063saCmfQODTCwtg/nZ\n"
							   "Hn7XGfT1dp5jyqzLDLJ+OSGkazLpUXg5gMPRtju7H8z4oPQMaZZX7bnafBbkQ/Xo\n"
							   "X9Gn5PGpWqaNAhUA1YRAaKf5eR8cC+SRWIIvP8b3pMcCgYEAmf5LMvR+m2akqKu2\n"
							   "piW7dN1PxyH0Ixx12aQVF4Hek1cHK6LZW/a/SGrG1pdvz60iq/6aA3vyt2CI8rRi\n"
							   "mjlCjM5YGGYCKn6l6cTrGQCB2QL7ThIeKFAmPy2wLWe2uGUa12texb4PGKGV7HsO\n"
							   "wnWAsuMYQ8DLM5tT1v8s/779g/wDgYQAAoGASzXSQchr3SyX5vxTtbZ3v8x/a7nZ\n"
							   "HAYEiBXfVG8iW+kaAGkyU0L0AIFXJFTUYbjI3EAO24kyFmwWI2e8BkXmibIuAlwY\n"
							   "1vSGQgpXPK6250pSrx8r8pngr9Qxrt8ElFpLtK+Of6Th/wSTe8COYbXSOP/6Yx7v\n"
							   "6cObyPo7eAmX8Sg=\n"
							   "-----END PUBLIC KEY-----");
			QTest::newRow("dsa1024") << dsa1024 << QSsl::KeyAlgorithm::Dsa << true;

			/*
			 * openssl genrsa -out rsa1024_key.pem 1024
			 * openssl rsa -in rsa1024_key.pem -out rsa1024_pubkey.pem -pubout
			 */
			QByteArray rsa1024("-----BEGIN PUBLIC KEY-----\n"
							   "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQC3cWoq9XuQI1soyaOM4hjVw4Lc\n"
							   "TA1HCypXuF/GaTmvxc82J+2F6ISxNFWnSXOs+wpgavXgpfDL7od5fo8EbnMRZLXQ\n"
							   "tHKRnCy0sQiCQqmSNmWe4qeLwXslKxm1fqj3/tvrX+0VxDVxXz5jS0HH7hHMdELQ\n"
							   "om/I1IppJFS4IkqEoQIDAQAB\n"
							   "-----END PUBLIC KEY-----");
			QTest::newRow("rsa1024") << rsa1024 << QSsl::KeyAlgorithm::Rsa << false;

			/*
			 * openssl genrsa -out rsa2048_key.pem 2048
			 * openssl rsa -in rsa2048_key.pem -out rsa2048_pubkey.pem -pubout
			 */
			QByteArray rsa2048("-----BEGIN PUBLIC KEY-----\n"
							   "MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAs1xh1ymbXRDOBxDmx2I9\n"
							   "oQbBvqE3QYK5ktPI9mHX38lwQ71AYw7pUxsGzCwwLDRBnHNc1gKtn8IoXhtujsWi\n"
							   "FvVpayI0GQkJ1Pob66Vr7DY9ZC9dfsJddZi+ce1cAdzg3Od/31y5c3Tiz0laNTkv\n"
							   "tyGhyyIovfPDJbdX2Srgg8GJAo0AtYJjVCJxZX5TDJLHmBdvK61ZXotgYIjkl7lA\n"
							   "DO9y31b/10IiLpFcgIUMI+8oF8OypQ/24XKIPVSJm4DZZkjSbUCuwT/mA+61m0no\n"
							   "q0hOP6K9hZ7pBZvqLZ0gZdVWqarIf9w/I5yS2QGN9jgJN/oJIKyljlreGuSagST/\n"
							   "5wIDAQAB\n"
							   "-----END PUBLIC KEY-----");
			QTest::newRow("rsa2048") << rsa2048 << QSsl::KeyAlgorithm::Rsa << true;
		}


		void hasValidEphemeralKeyLength()
		{
			QFETCH(QByteArray, pem);
			QFETCH(QSsl::KeyAlgorithm, algorithm);
			QFETCH(bool, sufficient);

			QSslKey key = createQSslKeyWithHandle(pem);
			QVERIFY(!key.isNull());

			#if QT_VERSION >= 0x050800
			QCOMPARE(key.algorithm(), algorithm);
			#else
			Q_UNUSED(algorithm)
			QCOMPARE(key.algorithm(), QSsl::Opaque);
			#endif

			QCOMPARE(CertificateChecker::hasValidEphemeralKeyLength(key), sufficient);
		}


};

QTEST_GUILESS_MAIN(test_CertificateChecker)
#include "test_CertificateChecker.moc"
