/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

#include "command/DidAuthenticateEAC2Command.h"

#include "asn1/ASN1Struct.h"
#include "asn1/ASN1Util.h"
#include "asn1/CVCertificateChainBuilder.h"

#include "CertificateHelper.h"
#include "MockCardConnectionWorker.h"
#include "TestFileHelper.h"

#include <QByteArrayList>
#include <QtTest>
#include <openssl/cms.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_DidAuthenticateEAC2Command
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<MockCardConnectionWorker> mWorker;
		QSharedPointer<EVP_PKEY> mKey;
		QSharedPointer<X509> mCert;

		static QByteArray readFile(const QString& pFileName)
		{
			return TestFileHelper::readFile(QStringLiteral(":/card/").append(pFileName));
		}


		static CVCertificateChain createCVCertificateChain()
		{
			QList<QSharedPointer<const CVCertificate>> list;
			list << CertificateHelper::fromHex(readFile("cvca-DETESTeID00001.hex"_L1));
			list << CertificateHelper::fromHex(readFile("cvca-DETESTeID00002_DETESTeID00001.hex"_L1));
			list << CertificateHelper::fromHex(readFile("cvca-DETESTeID00002.hex"_L1));
			list << CertificateHelper::fromHex(readFile("cvca-DETESTeID00004_DETESTeID00002.hex"_L1));
			list << CertificateHelper::fromHex(readFile("cvca-DETESTeID00004.hex"_L1));
			list << CertificateHelper::fromHex(readFile("cvdv-DEDVeIDDPST00035.hex"_L1));
			list << CertificateHelper::fromHex(readFile("cvat-DEDEMODEV00038.hex"_L1));
			CVCertificateChainBuilder builder(list, false);
			return builder.getChainStartingWith(list.first());
		}


		QByteArray generateEfCardSecurity(const QByteArrayList& pSecurityInfos)
		{
			const auto& data = Asn1Util::encode(V_ASN1_UNIVERSAL, ASN1Struct::UNI_SET, pSecurityInfos.join(), true);
			QSharedPointer<BIO> bio(BIO_new(BIO_s_mem()), &BIO_free);
			BIO_write(bio.data(), data.data(), static_cast<int>(data.size()));

			QSharedPointer<CMS_ContentInfo> cms(CMS_sign(mCert.data(), mKey.data(), nullptr, nullptr, CMS_PARTIAL | CMS_NOSMIMECAP), &CMS_ContentInfo_free);
			CMS_set1_eContentType(cms.data(), OBJ_txt2obj("0.4.0.127.0.7.3.2.1", 1));
			CMS_final(cms.data(), bio.data(), nullptr, CMS_BINARY);

			uchar* rawContent = nullptr;
			const int contentSize = i2d_CMS_ContentInfo(cms.data(), &rawContent);
			const auto contentGuard = qScopeGuard([rawContent] {OPENSSL_free(rawContent);});

			return QByteArray(reinterpret_cast<char*>(rawContent), contentSize) + QByteArray::fromHex("9000");
		}

	private Q_SLOTS:
		void initTestCase()
		{
			QSharedPointer<EVP_PKEY_CTX> keyCtx(EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr), &EVP_PKEY_CTX_free);
			QVERIFY(keyCtx);
			QVERIFY(EVP_PKEY_keygen_init(keyCtx.data()) > 0);
			QVERIFY(EVP_PKEY_CTX_set_rsa_keygen_bits(keyCtx.data(), 512) > 0);

			EVP_PKEY* key = nullptr;
			QVERIFY(EVP_PKEY_keygen(keyCtx.data(), &key) > 0);
			mKey = QSharedPointer<EVP_PKEY>(key, &EVP_PKEY_free);
			QVERIFY(mKey);

			mCert = QSharedPointer<X509>(X509_new(), &X509_free);
			QVERIFY(mCert);
			ASN1_INTEGER_set(X509_get_serialNumber(mCert.data()), 1);
			X509_gmtime_adj(X509_getm_notBefore(mCert.data()), 0);
			X509_gmtime_adj(X509_getm_notAfter(mCert.data()), 24 * 3600);
			X509_set_pubkey(mCert.data(), key);
			X509_sign(mCert.data(), key, EVP_sha256());
		}


		void init()
		{
			mWorker.reset(new MockCardConnectionWorker());
		}


		void cleanup()
		{
			mWorker.clear();
		}


		void test_PerformChipAuthentication()
		{
			QByteArray input("0000");
			DidAuthenticateEAC2Command command(mWorker, CVCertificateChain(false), QByteArray(), QByteArray(), input, QByteArray());
			QByteArray bytes = QByteArray::fromHex("30 12"
												   "            06 0A 04007F00070202030202"
												   "            02 01 02"
												   "            02 01 08");
			auto chipAuthenticationInfo = ChipAuthenticationInfo::decode(bytes);

			mWorker->addResponse(CardReturnCode::COMMAND_FAILED);
			QTest::ignoreMessage(QtDebugMsg, "Performing CA MSE:Set AT");
			QCOMPARE(command.performChipAuthentication(chipAuthenticationInfo, input), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(command.getNonceAsHex(), QByteArray());
			QCOMPARE(command.getAuthTokenAsHex(), QByteArray());

			mWorker->addResponse(CardReturnCode::OK);
			QTest::ignoreMessage(QtDebugMsg, "Performing CA MSE:Set AT");
			QTest::ignoreMessage(QtWarningMsg, "CA MSE:Set AT failed: UNKNOWN");
			QCOMPARE(command.performChipAuthentication(chipAuthenticationInfo, input), CardReturnCode::PROTOCOL_ERROR);
			QCOMPARE(command.getNonceAsHex(), QByteArray());
			QCOMPARE(command.getAuthTokenAsHex(), QByteArray());

			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			mWorker->addResponse(CardReturnCode::CANCELLATION_BY_USER);
			QTest::ignoreMessage(QtDebugMsg, "Performing CA MSE:Set AT");
			QTest::ignoreMessage(QtDebugMsg, "Performing CA General Authenticate");
			QCOMPARE(command.performChipAuthentication(chipAuthenticationInfo, input), CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(command.getNonceAsHex(), QByteArray());
			QCOMPARE(command.getAuthTokenAsHex(), QByteArray());

			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			mWorker->addResponse(CardReturnCode::OK);
			QTest::ignoreMessage(QtDebugMsg, "Performing CA MSE:Set AT");
			QTest::ignoreMessage(QtDebugMsg, "Performing CA General Authenticate");
			QTest::ignoreMessage(QtWarningMsg, "CA General Authenticate failed: UNKNOWN");
			QCOMPARE(command.performChipAuthentication(chipAuthenticationInfo, input), CardReturnCode::PROTOCOL_ERROR);
			QCOMPARE(command.getNonceAsHex(), QByteArray());
			QCOMPARE(command.getAuthTokenAsHex(), QByteArray());

			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			QTest::ignoreMessage(QtDebugMsg, "Performing CA MSE:Set AT");
			QTest::ignoreMessage(QtDebugMsg, "Performing CA General Authenticate");
			QTest::ignoreMessage(QtCriticalMsg, "Cannot parse chip authentication response");
			QCOMPARE(command.performChipAuthentication(chipAuthenticationInfo, input), CardReturnCode::PROTOCOL_ERROR);
			QCOMPARE(command.getNonceAsHex(), QByteArray());
			QCOMPARE(command.getAuthTokenAsHex(), QByteArray());

			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("7C1481085B5B32C5B15D012C8208AAA14CFBA15994D39000"));
			QTest::ignoreMessage(QtDebugMsg, "Performing CA MSE:Set AT");
			QTest::ignoreMessage(QtDebugMsg, "Performing CA General Authenticate");
			QCOMPARE(command.performChipAuthentication(chipAuthenticationInfo, input), CardReturnCode::OK);
			QCOMPARE(command.getNonceAsHex(), QByteArray("5b5b32c5b15d012c"));
			QCOMPARE(command.getAuthTokenAsHex(), QByteArray("aaa14cfba15994d3"));

			const auto& commands = mWorker->getCommands();
			QCOMPARE(commands.size(), 10);
			QCOMPARE(commands.at(8), QByteArray::fromHex("002241a40f800a04007f00070202030202840108"));
			QCOMPARE(commands.at(9), QByteArray::fromHex("00860000087c0680043030303000"));
		}


		void test_PerformChipAuthenticationWithoutKeyId()
		{
			QByteArray input("0000");
			DidAuthenticateEAC2Command command(mWorker, CVCertificateChain(false), QByteArray(), QByteArray(), input, QByteArray());
			QByteArray bytes = QByteArray::fromHex("30 0F"
												   "            06 0A 04007F00070202030202"
												   "            02 01 02");
			auto chipAuthenticationInfo = ChipAuthenticationInfo::decode(bytes);

			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("7C1481085B5B32C5B15D012C8208AAA14CFBA15994D39000"));
			QTest::ignoreMessage(QtDebugMsg, "Performing CA MSE:Set AT");
			QTest::ignoreMessage(QtDebugMsg, "Performing CA General Authenticate");
			QCOMPARE(command.performChipAuthentication(chipAuthenticationInfo, input), CardReturnCode::OK);
			QCOMPARE(command.getNonceAsHex(), QByteArray("5b5b32c5b15d012c"));
			QCOMPARE(command.getAuthTokenAsHex(), QByteArray("aaa14cfba15994d3"));

			const auto& commands = mWorker->getCommands();
			QCOMPARE(commands.size(), 2);
			QCOMPARE(commands.at(0), QByteArray::fromHex("002241a40c800a04007f00070202030202"));
			QCOMPARE(commands.at(1), QByteArray::fromHex("00860000087c0680043030303000"));
		}


		void test_SelectChipAuthentication_data()
		{
			const QByteArray ta = QByteArray::fromHex("300D 060804007F0007020202 020102");
			const QByteArray eidType = QByteArray::fromHex("300E 060A04007F00070302030201 0500");
			const QByteArray ca1 = QByteArray::fromHex("3012 060A04007F00070202030202 020102 020145");
			const QByteArray ca2 = QByteArray::fromHex("3012 060A04007F00070202030202 020102 020146");
			const QByteArray ca3 = QByteArray::fromHex("3012 060A04007F00070202030202 020103 020147");
			const QByteArray pace = QByteArray::fromHex("3012 060A04007F00070202040202 020102 02010D");
			const QByteArray ri1 = QByteArray::fromHex("3017 060A04007F00070202050203 3009 020101 020143 0101FF");
			const QByteArray ri2 = QByteArray::fromHex("3017 060A04007F00070202050203 3009 020101 020144 010100");
			const QByteArray ri = QByteArray::fromHex("3019 060904007F000702020502 300C 060704007F00070102 02010D");
			const QByteArray ca = QByteArray::fromHex("301C 060904007F000702020302 300C 060704007F00070102 02010D 020145");
			const QByteArray pk = QByteArray::fromHex("3062 060904007F000702020102 3052 300C 060704007F00070102 02010D"
													  "03420004086C9B1F08D2ED1258D5EF006418003333EC5826CF8CBAC231CE3EA162774676136A85CC681B5262180190988E12CFB5B164E37BECB4C493F69CB7BCE5E970F8"
													  "020145");

			QTest::addColumn<CardReturnCode>("cardReturnCode");
			QTest::addColumn<QByteArray>("efCardSecurity");
			QTest::addColumn<int>("error");

			QTest::newRow("read_fail") << CardReturnCode::COMMAND_FAILED << QByteArray::fromHex("6A82") << 1;
			QTest::newRow("parse_fail") << CardReturnCode::OK << QByteArray::fromHex("01239000") << 2;
			QTest::newRow("version_none") << CardReturnCode::OK << generateEfCardSecurity({ta, eidType, pace, ri1, ri2, ri, ca, pk}) << 3;
			QTest::newRow("version_2") << CardReturnCode::OK << generateEfCardSecurity({ta, eidType, ca1, pace, ri1, ri2, ri, ca, pk}) << 0;
			QTest::newRow("version_22") << CardReturnCode::OK << generateEfCardSecurity({ta, eidType, ca1, ca2, pace, ri1, ri2, ri, ca, pk}) << 0;
			QTest::newRow("version_223") << CardReturnCode::OK << generateEfCardSecurity({ta, eidType, ca1, ca2, ca3, pace, ri1, ri2, ri, ca, pk}) << 0;
			QTest::newRow("version_232") << CardReturnCode::OK << generateEfCardSecurity({ta, eidType, ca1, ca3, ca2, pace, ri1, ri2, ri, ca, pk}) << 0;
			QTest::newRow("version_322") << CardReturnCode::OK << generateEfCardSecurity({ta, eidType, ca3, ca1, ca2, pace, ri1, ri2, ri, ca, pk}) << 0;
			QTest::newRow("version_3") << CardReturnCode::OK << generateEfCardSecurity({ta, eidType, ca3, pace, ri1, ri2, ri, ca, pk}) << 4;
		}


		void test_SelectChipAuthentication()
		{
			QFETCH(CardReturnCode, cardReturnCode);
			QFETCH(QByteArray, efCardSecurity);
			QFETCH(int, error);

			const CVCertificateChain cvcChain = createCVCertificateChain();
			const QByteArray auxData = QByteArray::fromHex("670F0102030405060708090A0B0C0D0E0F");
			DidAuthenticateEAC2Command command(mWorker, cvcChain, QByteArray("040506"), QByteArray(), auxData, QByteArray());

			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing TA MSE:Set DST with CAR "DETESTeID00001"
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing TA PSO:Verify Certificate with CVC(type=CVCA, car="DETESTeID00001", chr="DETESTeID00001", valid=["2010-08-13","2013-08-13"]=false)
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing TA MSE:Set DST with CAR "DETESTeID00001"
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing TA PSO:Verify Certificate with CVC(type=CVCA, car="DETESTeID00001", chr="DETESTeID00002", valid=["2010-09-21","2013-09-21"]=false)
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing TA MSE:Set DST with CAR "DETESTeID00002"
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing TA PSO:Verify Certificate with CVC(type=CVCA, car="DETESTeID00002", chr="DETESTeID00004", valid=["2012-05-11","2015-05-11"]=false)
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing TA MSE:Set DST with CAR "DETESTeID00004"
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing TA PSO:Verify Certificate with CVC(type=DV_no_f, car="DETESTeID00004", chr="DEDVeIDDPST00035", valid=["2014-02-11","2014-05-12"]=false)
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing TA MSE:Set DST with CAR "DEDVeIDDPST00035"
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing TA PSO:Verify Certificate with CVC(type=AT, car="DEDVeIDDPST00035", chr="DEDEMODEV00038", valid=["2014-03-13","2014-04-11"]=false)
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing TA MSE:Set AT
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing TA External Authenticate
			mWorker->addResponse(cardReturnCode, efCardSecurity);                  // Performing Read EF.CardSecurity
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000")); // Performing CA MSE:Set AT
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("7C1481085B5B32C5B15D012C8208AAA14CFBA15994D39000")); // Performing CA General Authenticate

			if (error == 0)
			{
				QTest::ignoreMessage(QtDebugMsg, "Choose ChipAuthenticationInfo: 0.4.0.127.0.7.2.2.3.2.2 (id-CA-ECDH-AES-CBC-CMAC-128), version: 2, keyId: 69");
			}
			else if (error == 1)
			{
				QTest::ignoreMessage(QtCriticalMsg, "Cannot read EF.CardSecurity");
			}
			else if (error == 2)
			{
				QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Cannot decode ASN.1 object: .*"_L1));
				QTest::ignoreMessage(QtCriticalMsg, "No contentInfo");
				QTest::ignoreMessage(QtCriticalMsg, "Cannot parse EF.CardSecurity");
			}
			else if (error == 3)
			{
				QTest::ignoreMessage(QtCriticalMsg, "No ChipAuthenticationInfo found in EF.CardSecurity");
			}
			else
			{
				QTest::ignoreMessage(QtCriticalMsg, "No ChipAuthenticationInfo with version 2 found in EF.CardSecurity");
			}

			command.internalExecute();

			QCOMPARE(command.getEfCardSecurityAsHex(), efCardSecurity.chopped(2).toHex());
			if (error == 0)
			{
				QCOMPARE(command.getReturnCode(), CardReturnCode::OK);
				QCOMPARE(command.getNonceAsHex(), QByteArray("5b5b32c5b15d012c"));
				QCOMPARE(command.getAuthTokenAsHex(), QByteArray("aaa14cfba15994d3"));
			}
			else
			{
				QCOMPARE(command.getReturnCode(), error == 1 ? CardReturnCode::COMMAND_FAILED : CardReturnCode::PROTOCOL_ERROR);
				QCOMPARE(command.getNonceAsHex(), QByteArray());
				QCOMPARE(command.getAuthTokenAsHex(), QByteArray());
			}
		}


};

QTEST_GUILESS_MAIN(test_DidAuthenticateEAC2Command)
#include "test_DidAuthenticateEAC2Command.moc"
