/*!
 * \brief Unit tests for \ref CertificateChecker
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "TlsChecker.h"

#include "LogHandler.h"
#include "SecureStorage.h"

#include "MockNetworkReply.h"
#include "TestFileHelper.h"

#include <openssl/pem.h>
#include <QtTest>

Q_DECLARE_LOGGING_CATEGORY(network)

using namespace governikus;

Q_DECLARE_METATYPE(QSsl::KeyAlgorithm)

class test_TlsChecker
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
			Env::getSingleton<LogHandler>()->init();
			certs = Env::getSingleton<SecureStorage>()->getUpdateCertificates();
			QVERIFY(certs.size() > 0);
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void hasUpdateCertificateKeyLength()
		{
			for (const auto& cert : qAsConst(certs))
			{
				QVERIFY(TlsChecker::hasValidCertificateKeyLength(cert));
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
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QVERIFY(!TlsChecker::hasValidCertificateKeyLength(invalidCert));

			QVERIFY(logSpy.count() > 0);
			auto param = logSpy.takeLast();
			QVERIFY(param.at(0).toString().contains(output));
		}


		void checkCertificateHash()
		{
			QVERIFY(!TlsChecker::checkCertificate(certs.at(0), QCryptographicHash::Algorithm::Sha256, QSet<QString>() << "dummy" << "bla bla"));
			const QString hash = QStringLiteral("9B:87:54:6D:28:D9:A5:CF:49:C8:B1:AB:3F:C6:0D:EA:63:4B:77:64:44:3A:A8:B1:87:9B:51:44:7E:97:D2:CA").remove(QLatin1Char(':'));
			QVERIFY(TlsChecker::checkCertificate(certs.at(0), QCryptographicHash::Algorithm::Sha256, QSet<QString>() << "dummy" << hash << "bla bla"));
		}


		void hasEmptyEphemeralKey()
		{
			QSslKey key;
			QVERIFY(!TlsChecker::hasValidEphemeralKeyLength(key));
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
			QTest::newRow("dsa1024") << dsa1024 << QSsl::KeyAlgorithm::Dsa << false;

			/*
			 * openssl dsaparam -out dsa2048_param.pem 2048
			 * openssl gendsa -out dsa2048_key.pem dsa2048_param.pem
			 * openssl dsa -in dsa2048_key.pem -pubout -out dsa2048_pubkey.pem
			 */
			QByteArray dsa2048("-----BEGIN PUBLIC KEY-----\n"
							   "MIIDSDCCAjoGByqGSM44BAEwggItAoIBAQDZiiMJ3Dmz9N/0DtlMYPDGzHv4EJBr\n"
							   "jFA7hNwK6ErO53JUnonaSIXoCf+yRuELAjr/O+VQ/kN1C10/czeNR7/zas+3MtEh\n"
							   "Y6cYHbMbce0ChxyW+GFgVixUwIkwMZVcRHjSGzfo/WXMyo08w6adWTYxMkb48OFl\n"
							   "Qfu/Z22GQ+lor7lyuOLkyOhgDxbZLuViryeKzYIZd3t5iony6X13a8lcefAac1bN\n"
							   "QQyBraRIpO8+H9bQkzDDMzYawRpFB8iVLgszGMblX0CxYjb5aUTvuYugYCO1pG83\n"
							   "5PH9f00W6FVk6sCiGM1sR/ED20fOAvhDDMaF++EB9ue8+rluskTa1EEbAiEAzuTB\n"
							   "M85eGF0KlXynqkZ3bNDg68Y2S/91KIX7I4LJ2MMCggEBANkPkUAhhIqd++GhuFmB\n"
							   "+zn3PrPArv9YqjlY+Q3gIcjWmTumDsCokjmW2xbgXTRFPQRX03b2PdQ51HYoSX2H\n"
							   "sPv65jiuOen9yLR+euMPD4EPaVG6R8v6RjXbzw/0BpMmt3504fwOPTz0diVghxFM\n"
							   "NAurfrIVupzUCU7uaxDgJDV2/6KZJjfZPYuzh1iCE9JBxdpW3yf7WMhhcgZYlVaj\n"
							   "ZTzHi9oOob0rQPlLFSXjQ0HQEqGWP7rJv18u1rnSnRLI16OoxNIDl9XC7iIPZd5z\n"
							   "Sd/tWTWzTuWSTA39njSbgIHCZgmIKzpaMNeLosnTaOcbs8GU01tErw3hJdHqgpNX\n"
							   "d3cDggEGAAKCAQEAgaqu8eNK8O2VND8xpjMmgxRTHtWQkPNXB5POpQglr1nwyzkn\n"
							   "VsvC+JM64cJiGH+6XrQguPvIduBpR013/SSYqPu6itdbZKNb7P5+tQrN3GtLeuv6\n"
							   "5zf6GMc3ZgR6Bp74EBXAnSZZgHNc/EEXuSyP1P3FqaSPsuZuiagVujDZpWKERCCl\n"
							   "+6qQXvUg41ZA3K0G0Wf6ZJVPMD6vzSrkF+rJ9cgJeFwCBQ+laFc23Mke8lvblyPS\n"
							   "+wc7yd8AmFxlcif7PKZ+NzsmA53JOFOnNSvMFXXykJnxqJbUA6Iw+Je3mW+4PsD4\n"
							   "foMYlbwM/IzefL1TD2+eBlyV0unSwMIwrvXPTg==\n"
							   "-----END PUBLIC KEY-----\n");

			QTest::newRow("dsa2048") << dsa2048 << QSsl::KeyAlgorithm::Dsa << true;

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
			QCOMPARE(key.algorithm(), algorithm);
			QCOMPARE(TlsChecker::hasValidEphemeralKeyLength(key), sufficient);
		}


		void containsFatalError()
		{
			const auto reply = QSharedPointer<MockNetworkReply>::create();
			QList<QSslError> errors;

			QVERIFY(!TlsChecker::containsFatalError(reply, errors));

			errors.append(QSslError(QSslError::SslError::SelfSignedCertificate));
			QVERIFY(!TlsChecker::containsFatalError(reply, errors));

			errors.append(QSslError(QSslError::SslError::SubjectIssuerMismatch));
			QVERIFY(TlsChecker::containsFatalError(reply, errors));
		}


		void addUnsupportedCipher()
		{
			SslCipherList list;
			QVERIFY(list.isEmpty());
			list += QStringLiteral("dummy value");
			QVERIFY(list.isEmpty());
		}


		void addSupportedCipher()
		{
			SslCipherList list;
			QVERIFY(list.isEmpty());
			list += QStringLiteral("ECDHE-ECDSA-AES256-GCM-SHA384");
			QCOMPARE(list.length(), 1);
		}


		void addUnsupportedCurve()
		{
			SslEllipticCurveVector vector;
			QVERIFY(vector.isEmpty());
			vector += QStringLiteral("dummy value");
			QVERIFY(vector.isEmpty());
		}


		void addSupportedCurve()
		{
			SslEllipticCurveVector vector;
			QVERIFY(vector.isEmpty());
			vector += QStringLiteral("brainpoolP512r1");
			QCOMPARE(vector.length(), 1);
		}


		void sslConfigLog()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);
			QSslConfiguration cfg;
			TlsChecker::logSslConfig(cfg, spawnMessageLogger(network));

			QCOMPARE(logSpy.count(), 6);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Used session cipher QSslCipher(name=, bits=0, proto=)"));
			QVERIFY(logSpy.at(1).at(0).toString().contains("Used session protocol: \"UnknownProtocol\""));
			QVERIFY(logSpy.at(2).at(0).toString().contains("Used ephemeral server key:"));
			QVERIFY(logSpy.at(3).at(0).toString().contains("Used peer certificate: QSslCertificate(\"\", \"\", \"1B2M2Y8AsgTpgAmY7PhCfg==\""));
			QVERIFY(logSpy.at(4).at(0).toString().contains("Used ssl session: \"\""));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Handshake of tls connection done!"));
		}


};

QTEST_GUILESS_MAIN(test_TlsChecker)
#include "test_TlsChecker.moc"
