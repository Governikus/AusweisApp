/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#include "TlsChecker.h"

#include "LogHandler.h"
#include "ResourceLoader.h"
#include "SecureStorage.h"

#include "MockNetworkReply.h"
#include "TestFileHelper.h"

#include <QtTest>
#include <openssl/pem.h>

Q_DECLARE_LOGGING_CATEGORY(network)

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_METATYPE(QSsl::KeyAlgorithm)

class test_TlsChecker
	: public QObject
{
	Q_OBJECT

	QList<QSslCertificate> certs;

	static QSslKey createQSslKeyWithHandle(const QByteArray& pPemEncodedEvpPKey)
	{
		BIO* bio = BIO_new(BIO_s_mem());
		BIO_write(bio, pPemEncodedEvpPKey.constData(), static_cast<int>(pPemEncodedEvpPKey.length()));
		QSslKey key(PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr), QSsl::PublicKey);
		BIO_free(bio);
		return key;
	}

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
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
			for (const auto& cert : std::as_const(certs))
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
			QVERIFY(!TlsChecker::checkCertificate(certs.at(0), QCryptographicHash::Algorithm::Sha256, QSet<QString>() << "dummy"_L1 << "bla bla"_L1));
			const QString hash = QStringLiteral("9B:87:54:6D:28:D9:A5:CF:49:C8:B1:AB:3F:C6:0D:EA:63:4B:77:64:44:3A:A8:B1:87:9B:51:44:7E:97:D2:CA").remove(':'_L1);
			QVERIFY(TlsChecker::checkCertificate(certs.at(0), QCryptographicHash::Algorithm::Sha256, QSet<QString>() << "dummy"_L1 << hash << "bla bla"_L1));
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
			QTest::addColumn<FailureCode::FailureInfoMap>("infoMap");

			/*
			 * openssl ecparam -name secp112r2 -out secp112r2_param.pem
			 * openssl ecparam -in secp112r2_param.pem -genkey -noout -out secp112r2_key.pem
			 * openssl ec -in secp112r2_key.pem -pubout -out secp112r2_pubkey.pem
			 */
			QByteArray ec112("-----BEGIN PUBLIC KEY-----\n"
							 "MDIwEAYHKoZIzj0CAQYFK4EEAAcDHgAEWo89aCax3oUWJho7rFZ1u70WqghvA7Tf\n"
							 "SXXiZw==\n"
							 "-----END PUBLIC KEY-----");
			QTest::newRow("ec112") << ec112 << QSsl::KeyAlgorithm::Ec << false << FailureCode::FailureInfoMap {};

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
			QTest::newRow("ec521") << ec521 << QSsl::KeyAlgorithm::Ec << true << FailureCode::FailureInfoMap {
				{FailureCode::Info::Ephemeral_Server_Key_Algorithm, "Ec"_L1},
				{FailureCode::Info::Ephemeral_Server_Key_Length, "521"_L1}
				};

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
			QTest::newRow("dsa768") << dsa768 << QSsl::KeyAlgorithm::Dsa << false << FailureCode::FailureInfoMap {
				{FailureCode::Info::Ephemeral_Server_Key_Algorithm, "Dsa"_L1},
				{FailureCode::Info::Ephemeral_Server_Key_Length, "768"_L1}
				};

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
			QTest::newRow("dsa1024") << dsa1024 << QSsl::KeyAlgorithm::Dsa << false << FailureCode::FailureInfoMap {
				{FailureCode::Info::Ephemeral_Server_Key_Algorithm, "Dsa"_L1},
				{FailureCode::Info::Ephemeral_Server_Key_Length, "1024"_L1}
				};

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

			QTest::newRow("dsa2048") << dsa2048 << QSsl::KeyAlgorithm::Dsa << false << FailureCode::FailureInfoMap {
				{FailureCode::Info::Ephemeral_Server_Key_Algorithm, "Dsa"_L1},
				{FailureCode::Info::Ephemeral_Server_Key_Length, "2048"_L1}
				};

			/*
			 * openssl dsaparam -out dsa3072_param.pem 3072
			 * openssl gendsa -out dsa3072_key.pem dsa3072_param.pem
			 * openssl dsa -in dsa3072_key.pem -pubout -out dsa3072_pubkey.pem
			 */
			QByteArray dsa3072("-----BEGIN PUBLIC KEY-----\n"
							   "MIIEwjCCAzUGByqGSM44BAEwggMoAoIBgQCSrktmNWqtkuTQKbZnPkpqVq56/YTD\n"
							   "Fc5EHJmqWoWWyobwChfWn9bsanUGKYUdSR4Y+PfsmNHkHTytaMPixXC3F9wnA14O\n"
							   "RaI34Ivf+1CUsGbQH0aVX0GfQHODNsqqLaxS6/sI+Kd4Xye6GPQYdxaetLwo9TEA\n"
							   "jQYHQd3gClQXdSWgqG5bDCU4ILkUNJ5coP8UOkFJjk/z7EcxcuFOu9OPCExxI9G4\n"
							   "U3bFBIHjUcFk5cbe0i1YehwAo2HGTUmc/K1XGr50THuMSgcWH/MvuXmY7/mhlnlh\n"
							   "7udLkT1CsBezbcpA2uG7nkDKBzjGKnOnLkLy0elhJs+OYUjR4PibaR2Mat52tM/K\n"
							   "lUl5duIhxbb/TWLGvVZYBYNm96O2d8P9XVknlqbH1lrukrouF/yTH9yH/s32npR/\n"
							   "0R7vjzXmokyj0+4AT33NxzWX1ANzI7H4bALKx4G/hpmyBo+dbWrmaUqANVg3blsf\n"
							   "fgKUleKK5b7lk8Rj1NHazHk3NbXC1GqKIKsCHQD1k8RPbnqQ0itWdC+kTI0lKIas\n"
							   "92tw8KVbEq/XAoIBgEMfD1FJgMLzS8p7vrw9G2PvaLB5KHrPDu/9uzsdjWDndyzN\n"
							   "bJCAdDxWB5hPwVS3S5Hx06weT8xNRNYGomlG3iW+AbWNBxpDyWLKeVl4FvGGBYK+\n"
							   "j7o3vOEKqfVXNxY8ALQUUWPprEpm6UH/mtqMa6AE26bunx/dDDOCdyoLtP1VcYCG\n"
							   "Un1QhdZ0jo7Aj5gZDsa09Iluxq4I73WE71/3TFgM0RFInR1mNEFRZkLpn1q+YZ4d\n"
							   "HJllhVmSZFN1LDJ3SEbTBi6nEy3PvWFFUAsYIJPtoZjFWRLoLkNAwLUTdEijOkXG\n"
							   "F5okmnjmCUC/tLy3jpwKq8X4PCcU29sV0tDINIF0bf182/P9kmfs5GIVeVubkmlX\n"
							   "/cGb5A+HfRVALAPPP0NYp0RM28OYKmEdPPs2hrbnh49GVaPEdwSI+Ta75zafTXp0\n"
							   "wH9s3tzChuVfQT0U+aApz4/Zo4RfjjtxvxM4x0PF7+PvNrX0DXYGnetE1pqaOwpW\n"
							   "cNXwgNTTCOY5LjXreQOCAYUAAoIBgBoAsSoEmKbDONThiRSFP0edi7Rt+gx2NgRr\n"
							   "oAVtCi0nqCG3yNOA2IbDRINgWxgw+HHvzazQQQMsJA6UPPkxSLBXHnWCSY8EM3iG\n"
							   "Uu0qVu8eaq7OFmqeSJdg04LmXGMIqA5L79XMKT06gAuOaMQt8NbB8HXZFTlGgLZZ\n"
							   "76jdrOTjqoChr9jYUbUNd2v9d+YWNWWP9A7YFbiNkmwbWqc2SbecziD3AUuasrZU\n"
							   "m6+NJjVo2W44mKmXvYyY/wPBV80qGqAXkosef/Xh1c4kHifGID5sKtWRMT881a0Z\n"
							   "4UT0LgoNpcLlr6FDUWqfUHlHz6h3EdINueHsdvHoRyfK16HfQzcycJ1wePyDP010\n"
							   "h2BpFQkvsbB4GlEpg2EpA+8pME1pKOdziNR6OPUY/O7CtSl4NzuKbm0xRSRe8Y0l\n"
							   "WYgK8nUY1xGKBVmGoUfFhCPgZtz2n7g2EXGZuu80qfiGmmKOyo2QmPWvXeErEPop\n"
							   "onJVM+lxe8gp7okaGGmRcBgho199Bg==\n"
							   "-----END PUBLIC KEY-----");

			QTest::newRow("dsa3072") << dsa3072 << QSsl::KeyAlgorithm::Dsa << true << FailureCode::FailureInfoMap {
				{FailureCode::Info::Ephemeral_Server_Key_Algorithm, "Dsa"_L1},
				{FailureCode::Info::Ephemeral_Server_Key_Length, "3072"_L1}
				};

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
			QTest::newRow("rsa1024") << rsa1024 << QSsl::KeyAlgorithm::Rsa << false << FailureCode::FailureInfoMap {
				{FailureCode::Info::Ephemeral_Server_Key_Algorithm, "Rsa"_L1},
				{FailureCode::Info::Ephemeral_Server_Key_Length, "1024"_L1}
				};

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
			QTest::newRow("rsa2048") << rsa2048 << QSsl::KeyAlgorithm::Rsa << false << FailureCode::FailureInfoMap {
				{FailureCode::Info::Ephemeral_Server_Key_Algorithm, "Rsa"_L1},
				{FailureCode::Info::Ephemeral_Server_Key_Length, "2048"_L1}
				};

			/*
			 * openssl genrsa -out rsa3072_key.pem 3072
			 * openssl rsa -in rsa3072_key.pem -out rsa3072_pubkey.pem -pubout
			 */
			QByteArray rsa3072("-----BEGIN PUBLIC KEY-----\n"
							   "MIIBojANBgkqhkiG9w0BAQEFAAOCAY8AMIIBigKCAYEAml5hQRUb0j/3+bnBCoBE\n"
							   "U75t43Mp+XOS6nwOy/KJgvbpT+oPjKcNOmM0fXAnByCcpbc0avrT9+jNGduzNWU0\n"
							   "/TAHBJxfWV3+rC/vQ9MFdOEmcmGB6j2pCbiuxJLQX4jAFE0ilSXouisPhDrU1dvJ\n"
							   "2NlSV2IarjeywfzN77ik3eA69pBKT9B9Vior5TtWwpqIkeC/41vnysFusbWTfn7M\n"
							   "3BilReDC1k9KBlm5hHQy3GpswnFRABLvsy/hz3yAeeCQyZxUXCKcSahqE+DB/PTy\n"
							   "nPwOWU9mL7d61WZ6mZjSo+iHHgeVkAsTnuLOr3yo5FG+utMsNxgZeI352ylMeJdj\n"
							   "bMlbpdqHBtn5sr3OvOt3Hi+vvzDxI1lX0b71vTVriGfgr+o6VUbvfCRy0b8J2/6f\n"
							   "HSYiIDh/Wf2tz5mKRfpV61k3AIM7aiw5+evFtU7cT/mBH2e5K2rhcywlUiEmiwzP\n"
							   "8P3+H+HvrctpZoO3J9kIqOoGBz7Q7ocl1HNYMKFh7oy7AgMBAAE=\n"
							   "-----END PUBLIC KEY-----");
			QTest::newRow("rsa3072") << rsa3072 << QSsl::KeyAlgorithm::Rsa << true << FailureCode::FailureInfoMap {
				{FailureCode::Info::Ephemeral_Server_Key_Algorithm, "Rsa"_L1},
				{FailureCode::Info::Ephemeral_Server_Key_Length, "3072"_L1}
				};
		}


		void hasValidEphemeralKeyLength()
		{
			QFETCH(QByteArray, pem);
			QFETCH(QSsl::KeyAlgorithm, algorithm);
			QFETCH(bool, sufficient);
			QFETCH(FailureCode::FailureInfoMap, infoMap);

			QSslKey key = createQSslKeyWithHandle(pem);
			QVERIFY(!key.isNull());
			QCOMPARE(key.algorithm(), algorithm);
			QCOMPARE(TlsChecker::hasValidEphemeralKeyLength(key), sufficient);
			if (!infoMap.isEmpty())
			{
				QCOMPARE(TlsChecker::getEphemeralKeyInfoMap(key), infoMap);
			}
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
			QVERIFY(logSpy.at(0).at(0).toString().contains("Used session cipher QSslCipher(name=, bits=0, proto=)"_L1));
#if (QT_VERSION < QT_VERSION_CHECK(6, 7, 0))
			QVERIFY(logSpy.at(1).at(0).toString().contains("Used session protocol: \"UnknownProtocol\""_L1));
#else
			QVERIFY(logSpy.at(1).at(0).toString().contains("Used session protocol: QSsl::UnknownProtocol"_L1));
#endif
			QVERIFY(logSpy.at(2).at(0).toString().contains("Used ephemeral server key:"_L1));
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 0))
			QVERIFY(logSpy.at(3).at(0).toString().contains("Used peer certificate: QSslCertificate(\"\", \"\", \"1B2M2Y8AsgTpgAmY7PhCfg==\""_L1));
#else
			QVERIFY(logSpy.at(3).at(0).toString().contains(R"(Used peer certificate: QSslCertificate(Version="", SerialNumber="", Digest="1B2M2Y8AsgTpgAmY7PhCfg==", Issuer="", Subject="", AlternativeSubjectNames=QMultiMap(), EffectiveDate=QDateTime(Invalid), ExpiryDate=QDateTime(Invalid))"_L1));
#endif
			QVERIFY(logSpy.at(4).at(0).toString().contains("Used ssl session: \"\""_L1));
			QVERIFY(logSpy.at(5).at(0).toString().contains("Handshake of tls connection done!"_L1));
		}


};

QTEST_GUILESS_MAIN(test_TlsChecker)
#include "test_TlsChecker.moc"
