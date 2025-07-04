/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/Oid.h"

#include "LogHandler.h"
#include "TestFileHelper.h"
#include "asn1/ASN1TemplateUtil.h"

#include <QtCore>
#include <QtTest>
#include <openssl/objects.h>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_Oid
	: public QObject
{
	Q_OBJECT

	private:
		bool mTestAllKnownOids;
		QMap<KnownOid, int> mOidsToTest;

	private Q_SLOTS:
		void initTestCase()
		{
			// Check if INIT_FUNCTION in Oid.cpp only added new and unique OIDs
			QVERIFY(getOpenSslError().isEmpty());

			// Check if all known OIDs are added
			QCOMPARE(Oid::cKnownOids.size(), Enum<KnownOid>::getList().size());
			mTestAllKnownOids = false;
			mOidsToTest = Oid::cKnownOids;

			Env::getSingleton<LogHandler>()->init();
		}


		void cleanupTestCase()
		{
			QVERIFY(!mTestAllKnownOids || mOidsToTest.isEmpty());
		}


		void standard()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			Oid oid;

			QCOMPARE(oid, Oid());
			QCOMPARE(oid.operator QByteArray(), QByteArray());
			QCOMPARE(QByteArray(oid), QByteArray());

			QCOMPARE(logSpy.count(), 0);

		}


		void copyConstructor_data()
		{
			QTest::addColumn<QByteArray>("oid");
			QTest::addColumn<bool>("warnings");

			QTest::newRow("openssl") << QByteArray::fromHex("2A864886F70D010702") << false;
			QTest::newRow("known") << QByteArray::fromHex("04007F0007020204") << false;
			QTest::newRow("unknown") << QByteArray::fromHex("04007F00070200") << true;
		}


		void copyConstructor()
		{
			QFETCH(QByteArray, oid);
			QFETCH(bool, warnings);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			Oid source(oid);
			Oid target(source);
			QCOMPARE(source, target);
			QCOMPARE(logSpy.count() > 0, warnings);
		}


		void moveConstructor_data()
		{
			QTest::addColumn<QByteArray>("oid");
			QTest::addColumn<bool>("warnings");

			QTest::newRow("openssl") << QByteArray::fromHex("2A864886F70D010702") << false;
			QTest::newRow("known") << QByteArray::fromHex("04007F0007020204") << false;
			QTest::newRow("unknown") << QByteArray::fromHex("04007F00070200") << true;
		}


		void moveConstructor()
		{
			QFETCH(QByteArray, oid);
			QFETCH(bool, warnings);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			Oid source(oid);
			const auto* checkPtr = source.mObject;
			Oid target(std::move(source));
			QVERIFY(source != target);
			QVERIFY(source.mObject == nullptr);
			QVERIFY(target.mObject != nullptr);
			QVERIFY(target.mObject == checkPtr);
			QCOMPARE(logSpy.count() > 0, warnings);
		}


		void assignmentOperator_data()
		{
			QTest::addColumn<QByteArray>("oid");
			QTest::addColumn<bool>("warnings");

			QTest::newRow("openssl") << QByteArray::fromHex("2A864886F70D010702") << false;
			QTest::newRow("known") << QByteArray::fromHex("04007F0007020204") << false;
			QTest::newRow("unknown") << QByteArray::fromHex("04007F00070200") << true;
		}


		void assignmentOperator()
		{
			QFETCH(QByteArray, oid);
			QFETCH(bool, warnings);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			Oid source(oid);
			Oid target;
			target = source;
			QCOMPARE(source, target);
			QCOMPARE(logSpy.count() > 0, warnings);
		}


		void assignmentMoveOperator_data()
		{
			QTest::addColumn<QByteArray>("oid");
			QTest::addColumn<bool>("warnings");

			QTest::newRow("openssl") << QByteArray::fromHex("2A864886F70D010702") << false;
			QTest::newRow("known") << QByteArray::fromHex("04007F0007020204") << false;
			QTest::newRow("unknown") << QByteArray::fromHex("04007F00070200") << true;
		}


		void assignmentMoveOperator()
		{
			QFETCH(QByteArray, oid);
			QFETCH(bool, warnings);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			Oid source(oid);
			const auto* checkPtr = source.mObject;
			Oid target;
			target = std::move(source);
			QVERIFY(source != target);
			QVERIFY(source.mObject == nullptr);
			QVERIFY(target.mObject != nullptr);
			QVERIFY(target.mObject == checkPtr);
			QCOMPARE(logSpy.count() > 0, warnings);
		}


		void getter_data()
		{
			QTest::addColumn<QByteArray>("oid");
			QTest::addColumn<QLatin1String>("logging");
			QTest::addColumn<bool>("warnings");

			QTest::newRow("openssl") << QByteArray::fromHex("2A864886F70D010702") << QLatin1String("1.2.840.113549.1.7.2 (pkcs7-signedData)") << false;
			QTest::newRow("known") << QByteArray::fromHex("04007F0007020204") << QLatin1String("0.4.0.127.0.7.2.2.4 (id-PACE)") << false;
			QTest::newRow("unknown") << QByteArray::fromHex("04007F00070200") << QLatin1String("0.4.0.127.0.7.2.0") << true;
		}


		void getter()
		{
			QFETCH(QByteArray, oid);
			QFETCH(QLatin1String, logging);
			QFETCH(bool, warnings);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			Oid oidObject(oid);
			qDebug() << oidObject;

			QCOMPARE(QByteArray(oidObject), oid);
			QVERIFY(logSpy.count() > 0);
			QVERIFY(TestFileHelper::containsLog(logSpy, logging));
			QCOMPARE(logSpy.count() > 1, warnings);
		}


		void knownOids_data()
		{
			QTest::addColumn<KnownOid>("knownOid");
			QTest::addColumn<QByteArray>("bin");
			QTest::addColumn<QByteArray>("oid");
			QTest::addColumn<QByteArray>("sn");
			QTest::addColumn<QByteArray>("ln");

			QTest::newRow("001") << KnownOid::ID_SIGNED_DATA << QByteArray::fromHex("2A864886F70D010702") << QByteArray("1.2.840.113549.1.7.2") << QByteArray("pkcs7-signedData") << QByteArray("pkcs7-signedData");

			QTest::newRow("002") << KnownOid::EC_PS_PUBLICKEY << QByteArray::fromHex("04007F000701010203") << QByteArray("0.4.0.127.0.7.1.1.2.3") << QByteArray("ecPSPublicKey") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Static keys for Pseudonymous Signatures A.6.1");

			QTest::newRow("003") << KnownOid::STANDARDIZED_DOMAINPARAMETERS << QByteArray::fromHex("04007F00070102") << QByteArray("0.4.0.127.0.7.1.2") << QByteArray("standardizedDomainParameters") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Standardized Domain Parameters A.2.1.1");

			QTest::newRow("004") << KnownOid::BSI_DE_PROTOCOLS_SMARTCARD << QByteArray::fromHex("04007F00070202") << QByteArray("0.4.0.127.0.7.2.2") << QByteArray("bsi-de-protocols-smartcard") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Supported Protocols A.1.1");

			QTest::newRow("005") << KnownOid::ID_PK << QByteArray::fromHex("04007F0007020201") << QByteArray("0.4.0.127.0.7.2.2.1") << QByteArray("id-PK") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1");
			QTest::newRow("006") << KnownOid::ID_PK_DH << QByteArray::fromHex("04007F000702020101") << QByteArray("0.4.0.127.0.7.2.2.1.1") << QByteArray("id-PK-DH") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1.1");
			QTest::newRow("007") << KnownOid::ID_PK_ECDH << QByteArray::fromHex("04007F000702020102") << QByteArray("0.4.0.127.0.7.2.2.1.2") << QByteArray("id-PK-ECDH") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1.2");
			QTest::newRow("008") << KnownOid::ID_PS_PK << QByteArray::fromHex("04007F000702020103") << QByteArray("0.4.0.127.0.7.2.2.1.3") << QByteArray("id-PS-PK") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1.3");
			QTest::newRow("009") << KnownOid::ID_PS_PK_ECDH_ESCHNORR << QByteArray::fromHex("04007F00070202010302") << QByteArray("0.4.0.127.0.7.2.2.1.3.2") << QByteArray("id-PS-PK-ECDH-ECSchnorr") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1.3.2");

			QTest::newRow("010") << KnownOid::ID_TA << QByteArray::fromHex("04007F0007020202") << QByteArray("0.4.0.127.0.7.2.2.2") << QByteArray("id-TA") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3");
			QTest::newRow("011") << KnownOid::ID_TA_RSA << QByteArray::fromHex("04007F000702020201") << QByteArray("0.4.0.127.0.7.2.2.2.1") << QByteArray("id-TA-RSA") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1");
			QTest::newRow("012") << KnownOid::ID_TA_RSA_V1_5_SHA_1 << QByteArray::fromHex("04007F00070202020101") << QByteArray("0.4.0.127.0.7.2.2.2.1.1") << QByteArray("id-TA-RSA-v1-5-SHA-1") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.1");
			QTest::newRow("013") << KnownOid::ID_TA_RSA_V1_5_SHA_256 << QByteArray::fromHex("04007F00070202020102") << QByteArray("0.4.0.127.0.7.2.2.2.1.2") << QByteArray("id-TA-RSA-v1-5-SHA-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.2");
			QTest::newRow("014") << KnownOid::ID_TA_RSA_PSS_SHA_1 << QByteArray::fromHex("04007F00070202020103") << QByteArray("0.4.0.127.0.7.2.2.2.1.3") << QByteArray("id-TA-RSA-PSS-SHA-1") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.3");
			QTest::newRow("015") << KnownOid::ID_TA_RSA_PSS_SHA_256 << QByteArray::fromHex("04007F00070202020104") << QByteArray("0.4.0.127.0.7.2.2.2.1.4") << QByteArray("id-TA-RSA-PSS-SHA-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.4");
			QTest::newRow("016") << KnownOid::ID_TA_RSA_V1_5_SHA_512 << QByteArray::fromHex("04007F00070202020105") << QByteArray("0.4.0.127.0.7.2.2.2.1.5") << QByteArray("id-TA-RSA-v1-5-SHA-512") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.5");
			QTest::newRow("017") << KnownOid::ID_TA_RSA_PSS_SHA_512 << QByteArray::fromHex("04007F00070202020106") << QByteArray("0.4.0.127.0.7.2.2.2.1.6") << QByteArray("id-TA-RSA-PSS-SHA-512") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.6");
			QTest::newRow("018") << KnownOid::ID_TA_ECDSA << QByteArray::fromHex("04007F000702020202") << QByteArray("0.4.0.127.0.7.2.2.2.2") << QByteArray("id-TA-ECDSA") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2");
			QTest::newRow("019") << KnownOid::ID_TA_ECDSA_SHA_1 << QByteArray::fromHex("04007F00070202020201") << QByteArray("0.4.0.127.0.7.2.2.2.2.1") << QByteArray("id-TA-ECDSA-SHA-1") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.1");
			QTest::newRow("020") << KnownOid::ID_TA_ECDSA_SHA_224 << QByteArray::fromHex("04007F00070202020202") << QByteArray("0.4.0.127.0.7.2.2.2.2.2") << QByteArray("id-TA-ECDSA-SHA-224") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.2");
			QTest::newRow("021") << KnownOid::ID_TA_ECDSA_SHA_256 << QByteArray::fromHex("04007F00070202020203") << QByteArray("0.4.0.127.0.7.2.2.2.2.3") << QByteArray("id-TA-ECDSA-SHA-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.3");
			QTest::newRow("022") << KnownOid::ID_TA_ECDSA_SHA_384 << QByteArray::fromHex("04007F00070202020204") << QByteArray("0.4.0.127.0.7.2.2.2.2.4") << QByteArray("id-TA-ECDSA-SHA-384") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.4");
			QTest::newRow("023") << KnownOid::ID_TA_ECDSA_SHA_512 << QByteArray::fromHex("04007F00070202020205") << QByteArray("0.4.0.127.0.7.2.2.2.2.5") << QByteArray("id-TA-ECDSA-SHA-512") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.5");

			QTest::newRow("024") << KnownOid::ID_CA << QByteArray::fromHex("04007F0007020203") << QByteArray("0.4.0.127.0.7.2.2.3") << QByteArray("id-CA") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3");
			QTest::newRow("025") << KnownOid::ID_CA_DH << QByteArray::fromHex("04007F000702020301") << QByteArray("0.4.0.127.0.7.2.2.3.1") << QByteArray("id-CA-DH") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.1");
			QTest::newRow("026") << KnownOid::ID_CA_DH_3DES_CBC_CBC << QByteArray::fromHex("04007F00070202030101") << QByteArray("0.4.0.127.0.7.2.2.3.1.1") << QByteArray("id-CA-DH-3DES-CBC-CBC") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.1.1");
			QTest::newRow("027") << KnownOid::ID_CA_DH_AES_CBC_CMAC_128 << QByteArray::fromHex("04007F00070202030102") << QByteArray("0.4.0.127.0.7.2.2.3.1.2") << QByteArray("id-CA-DH-AES-CBC-CMAC-128") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.1.2");
			QTest::newRow("028") << KnownOid::ID_CA_DH_AES_CBC_CMAC_192 << QByteArray::fromHex("04007F00070202030103") << QByteArray("0.4.0.127.0.7.2.2.3.1.3") << QByteArray("id-CA-DH-AES-CBC-CMAC-192") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.1.3");
			QTest::newRow("029") << KnownOid::ID_CA_DH_AES_CBC_CMAC_256 << QByteArray::fromHex("04007F00070202030104") << QByteArray("0.4.0.127.0.7.2.2.3.1.4") << QByteArray("id-CA-DH-AES-CBC-CMAC-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.1.4");
			QTest::newRow("030") << KnownOid::ID_CA_ECDH << QByteArray::fromHex("04007F000702020302") << QByteArray("0.4.0.127.0.7.2.2.3.2") << QByteArray("id-CA-ECDH") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.2");
			QTest::newRow("031") << KnownOid::ID_CA_ECDH_3DES_CBC_CBC << QByteArray::fromHex("04007F00070202030201") << QByteArray("0.4.0.127.0.7.2.2.3.2.1") << QByteArray("id-CA-ECDH-3DES-CBC-CBC") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.2.1");
			QTest::newRow("032") << KnownOid::ID_CA_ECDH_AES_CBC_CMAC_128 << QByteArray::fromHex("04007F00070202030202") << QByteArray("0.4.0.127.0.7.2.2.3.2.2") << QByteArray("id-CA-ECDH-AES-CBC-CMAC-128") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.2.2");
			QTest::newRow("033") << KnownOid::ID_CA_ECDH_AES_CBC_CMAC_192 << QByteArray::fromHex("04007F00070202030203") << QByteArray("0.4.0.127.0.7.2.2.3.2.3") << QByteArray("id-CA-ECDH-AES-CBC-CMAC-192") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.2.3");
			QTest::newRow("034") << KnownOid::ID_CA_ECDH_AES_CBC_CMAC_256 << QByteArray::fromHex("04007F00070202030204") << QByteArray("0.4.0.127.0.7.2.2.3.2.4") << QByteArray("id-CA-ECDH-AES-CBC-CMAC-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 3.2.4");

			QTest::newRow("035") << KnownOid::ID_PACE << QByteArray::fromHex("04007F0007020204") << QByteArray("0.4.0.127.0.7.2.2.4") << QByteArray("id-PACE") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4");
			QTest::newRow("036") << KnownOid::ID_PACE_DH_GM << QByteArray::fromHex("04007F000702020401") << QByteArray("0.4.0.127.0.7.2.2.4.1") << QByteArray("id-PACE-DH-GM") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.1");
			QTest::newRow("037") << KnownOid::ID_PACE_DH_GM_3DES_CBC_CBC << QByteArray::fromHex("04007F00070202040101") << QByteArray("0.4.0.127.0.7.2.2.4.1.1") << QByteArray("id-PACE-DH-GM-3DES-CBC-CBC") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.1.1");
			QTest::newRow("038") << KnownOid::ID_PACE_DH_GM_AES_CBC_CMAC_128 << QByteArray::fromHex("04007F00070202040102") << QByteArray("0.4.0.127.0.7.2.2.4.1.2") << QByteArray("id-PACE-DH-GM-AES-CBC-CMAC-128") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.1.2");
			QTest::newRow("039") << KnownOid::ID_PACE_DH_GM_AES_CBC_CMAC_192 << QByteArray::fromHex("04007F00070202040103") << QByteArray("0.4.0.127.0.7.2.2.4.1.3") << QByteArray("id-PACE-DH-GM-AES-CBC-CMAC-192") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.1.3");
			QTest::newRow("040") << KnownOid::ID_PACE_DH_GM_AES_CBC_CMAC_256 << QByteArray::fromHex("04007F00070202040104") << QByteArray("0.4.0.127.0.7.2.2.4.1.4") << QByteArray("id-PACE-DH-GM-AES-CBC-CMAC-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.1.4");
			QTest::newRow("041") << KnownOid::ID_PACE_ECDH_GM << QByteArray::fromHex("04007F000702020402") << QByteArray("0.4.0.127.0.7.2.2.4.2") << QByteArray("id-PACE-ECDH-GM") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.2");
			QTest::newRow("042") << KnownOid::ID_PACE_ECDH_GM_3DES_CBC_CBC << QByteArray::fromHex("04007F00070202040201") << QByteArray("0.4.0.127.0.7.2.2.4.2.1") << QByteArray("id-PACE-ECDH-GM-3DES-CBC-CBC") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.2.1");
			QTest::newRow("043") << KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128 << QByteArray::fromHex("04007F00070202040202") << QByteArray("0.4.0.127.0.7.2.2.4.2.2") << QByteArray("id-PACE-ECDH-GM-AES-CBC-CMAC-128") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.2.2");
			QTest::newRow("044") << KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_192 << QByteArray::fromHex("04007F00070202040203") << QByteArray("0.4.0.127.0.7.2.2.4.2.3") << QByteArray("id-PACE-ECDH-GM-AES-CBC-CMAC-192") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.2.3");
			QTest::newRow("045") << KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_256 << QByteArray::fromHex("04007F00070202040204") << QByteArray("0.4.0.127.0.7.2.2.4.2.4") << QByteArray("id-PACE-ECDH-GM-AES-CBC-CMAC-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.2.4");
			QTest::newRow("046") << KnownOid::ID_PACE_DH_IM << QByteArray::fromHex("04007F000702020403") << QByteArray("0.4.0.127.0.7.2.2.4.3") << QByteArray("id-PACE-DH-IM") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.3");
			QTest::newRow("047") << KnownOid::ID_PACE_DH_IM_3DES_CBC_CBC << QByteArray::fromHex("04007F00070202040301") << QByteArray("0.4.0.127.0.7.2.2.4.3.1") << QByteArray("id-PACE-DH-IM-3DES-CBC-CBC") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.3.1");
			QTest::newRow("048") << KnownOid::ID_PACE_DH_IM_AES_CBC_CMAC_128 << QByteArray::fromHex("04007F00070202040302") << QByteArray("0.4.0.127.0.7.2.2.4.3.2") << QByteArray("id-PACE-DH-IM-AES-CBC-CMAC-128") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.3.2");
			QTest::newRow("049") << KnownOid::ID_PACE_DH_IM_AES_CBC_CMAC_192 << QByteArray::fromHex("04007F00070202040303") << QByteArray("0.4.0.127.0.7.2.2.4.3.3") << QByteArray("id-PACE-DH-IM-AES-CBC-CMAC-192") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.3.3");
			QTest::newRow("050") << KnownOid::ID_PACE_DH_IM_AES_CBC_CMAC_256 << QByteArray::fromHex("04007F00070202040304") << QByteArray("0.4.0.127.0.7.2.2.4.3.4") << QByteArray("id-PACE-DH-IM-AES-CBC-CMAC-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.3.4");
			QTest::newRow("051") << KnownOid::ID_PACE_ECDH_IM << QByteArray::fromHex("04007F000702020404") << QByteArray("0.4.0.127.0.7.2.2.4.4") << QByteArray("id-PACE-ECDH-IM") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.4");
			QTest::newRow("052") << KnownOid::ID_PACE_ECDH_IM_3DES_CBC_CBC << QByteArray::fromHex("04007F00070202040401") << QByteArray("0.4.0.127.0.7.2.2.4.4.1") << QByteArray("id-PACE-ECDH-IM-3DES-CBC-CBC") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.4.1");
			QTest::newRow("053") << KnownOid::ID_PACE_ECDH_IM_AES_CBC_CMAC_128 << QByteArray::fromHex("04007F00070202040402") << QByteArray("0.4.0.127.0.7.2.2.4.4.2") << QByteArray("id-PACE-ECDH-IM-AES-CBC-CMAC-128") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.4.2");
			QTest::newRow("054") << KnownOid::ID_PACE_ECDH_IM_AES_CBC_CMAC_192 << QByteArray::fromHex("04007F00070202040403") << QByteArray("0.4.0.127.0.7.2.2.4.4.3") << QByteArray("id-PACE-ECDH-IM-AES-CBC-CMAC-192") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.4.3");
			QTest::newRow("055") << KnownOid::ID_PACE_ECDH_IM_AES_CBC_CMAC_256 << QByteArray::fromHex("04007F00070202040404") << QByteArray("0.4.0.127.0.7.2.2.4.4.4") << QByteArray("id-PACE-ECDH-IM-AES-CBC-CMAC-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.4.4");
			QTest::newRow("056") << KnownOid::ID_PACE_ECDH_CAM << QByteArray::fromHex("04007F000702020406") << QByteArray("0.4.0.127.0.7.2.2.4.6") << QByteArray("id-PACE-ECDH-CAM") << QByteArray("BSI TR-03105 Part 5.1 Version 1.5 PACE 7.4 - 6");
			QTest::newRow("057") << KnownOid::ID_PACE_ECDH_CAM_AES_CBC_CMAC_128 << QByteArray::fromHex("04007F00070202040602") << QByteArray("0.4.0.127.0.7.2.2.4.6.2") << QByteArray("id-PACE-ECDH-CAM-AES-CBC-CMAC-128") << QByteArray("BSI TR-03105 Part 5.1 Version 1.5 PACE 7.4 - 6.2");
			QTest::newRow("058") << KnownOid::ID_PACE_ECDH_CAM_AES_CBC_CMAC_192 << QByteArray::fromHex("04007F00070202040603") << QByteArray("0.4.0.127.0.7.2.2.4.6.3") << QByteArray("id-PACE-ECDH-CAM-AES-CBC-CMAC-192") << QByteArray("BSI TR-03105 Part 5.1 Version 1.5 PACE 7.4 - 6.3");
			QTest::newRow("059") << KnownOid::ID_PACE_ECDH_CAM_AES_CBC_CMAC_256 << QByteArray::fromHex("04007F00070202040604") << QByteArray("0.4.0.127.0.7.2.2.4.6.4") << QByteArray("id-PACE-ECDH-CAM-AES-CBC-CMAC-256") << QByteArray("BSI TR-03105 Part 5.1 Version 1.5 PACE 7.4 - 6.4");

			QTest::newRow("060") << KnownOid::ID_RI << QByteArray::fromHex("04007F0007020205") << QByteArray("0.4.0.127.0.7.2.2.5") << QByteArray("id-RI") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4");
			QTest::newRow("061") << KnownOid::ID_RI_DH << QByteArray::fromHex("04007F000702020501") << QByteArray("0.4.0.127.0.7.2.2.5.1") << QByteArray("id-RI-DH") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1");
			QTest::newRow("062") << KnownOid::ID_RI_DH_SHA_1 << QByteArray::fromHex("04007F00070202050101") << QByteArray("0.4.0.127.0.7.2.2.5.1.1") << QByteArray("id-RI-DH-SHA-1") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.1");
			QTest::newRow("063") << KnownOid::ID_RI_DH_SHA_224 << QByteArray::fromHex("04007F00070202050102") << QByteArray("0.4.0.127.0.7.2.2.5.1.2") << QByteArray("id-RI-DH-SHA-224") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.2");
			QTest::newRow("064") << KnownOid::ID_RI_DH_SHA_256 << QByteArray::fromHex("04007F00070202050103") << QByteArray("0.4.0.127.0.7.2.2.5.1.3") << QByteArray("id-RI-DH-SHA-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.3");
			QTest::newRow("065") << KnownOid::ID_RI_DH_SHA_384 << QByteArray::fromHex("04007F00070202050104") << QByteArray("0.4.0.127.0.7.2.2.5.1.4") << QByteArray("id-RI-DH-SHA-384") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.4");
			QTest::newRow("066") << KnownOid::ID_RI_DH_SHA_512 << QByteArray::fromHex("04007F00070202050105") << QByteArray("0.4.0.127.0.7.2.2.5.1.5") << QByteArray("id-RI-DH-SHA-512") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.5");
			QTest::newRow("067") << KnownOid::ID_RI_ECDH << QByteArray::fromHex("04007F000702020502") << QByteArray("0.4.0.127.0.7.2.2.5.2") << QByteArray("id-RI-ECDH") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2");
			QTest::newRow("068") << KnownOid::ID_RI_ECDH_SHA_1 << QByteArray::fromHex("04007F00070202050201") << QByteArray("0.4.0.127.0.7.2.2.5.2.1") << QByteArray("id-RI-ECDH-SHA-1") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.1");
			QTest::newRow("069") << KnownOid::ID_RI_ECDH_SHA_224 << QByteArray::fromHex("04007F00070202050202") << QByteArray("0.4.0.127.0.7.2.2.5.2.2") << QByteArray("id-RI-ECDH-SHA-224") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.2");
			QTest::newRow("070") << KnownOid::ID_RI_ECDH_SHA_256 << QByteArray::fromHex("04007F00070202050203") << QByteArray("0.4.0.127.0.7.2.2.5.2.3") << QByteArray("id-RI-ECDH-SHA-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.3");
			QTest::newRow("071") << KnownOid::ID_RI_ECDH_SHA_384 << QByteArray::fromHex("04007F00070202050204") << QByteArray("0.4.0.127.0.7.2.2.5.2.4") << QByteArray("id-RI-ECDH-SHA-384") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.4");
			QTest::newRow("072") << KnownOid::ID_RI_ECDH_SHA_512 << QByteArray::fromHex("04007F00070202050205") << QByteArray("0.4.0.127.0.7.2.2.5.2.5") << QByteArray("id-RI-ECDH-SHA-512") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.5");

			QTest::newRow("073") << KnownOid::ID_CI << QByteArray::fromHex("04007F0007020206") << QByteArray("0.4.0.127.0.7.2.2.6") << QByteArray("id-CI") << QByteArray("BSI TR-03110 Part 3 Version 2.21 CardInfo (eIDAS token only) A.1.1.7");

			QTest::newRow("074") << KnownOid::ID_EID_SECURITY << QByteArray::fromHex("04007F0007020207") << QByteArray("0.4.0.127.0.7.2.2.7") << QByteArray("id-eIDSecurity") << QByteArray("BSI TR-03110 Part 3 Version 2.21 EIDSecurityInfo (eIDAS token only) A.1.1.8");

			QTest::newRow("075") << KnownOid::ID_PT << QByteArray::fromHex("04007F0007020208") << QByteArray("0.4.0.127.0.7.2.2.8") << QByteArray("id-PT") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PrivilegedTerminalInfo (eIDAS token only) A.1.1.9");

			QTest::newRow("076") << KnownOid::ID_PS << QByteArray::fromHex("04007F000702020B") << QByteArray("0.4.0.127.0.7.2.2.11") << QByteArray("id-PS") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 11");
			QTest::newRow("077") << KnownOid::ID_PSA << QByteArray::fromHex("04007F000702020B01") << QByteArray("0.4.0.127.0.7.2.2.11.1") << QByteArray("id-PSA") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 11.1");
			QTest::newRow("078") << KnownOid::ID_PSA_ECDH_ECSCHNORR << QByteArray::fromHex("04007F000702020B0102") << QByteArray("0.4.0.127.0.7.2.2.11.1.2") << QByteArray("id-PSA-ECDH-ECSchnorr") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 11.1.2");
			QTest::newRow("079") << KnownOid::ID_PSA_ECDH_ECSCHNORR_SHA_256 << QByteArray::fromHex("04007F000702020B010203") << QByteArray("0.4.0.127.0.7.2.2.11.1.2.3") << QByteArray("id-PSA-ECDH-ECSchnorr-SHA-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 11.1.2.3");
			QTest::newRow("080") << KnownOid::ID_PSA_ECDH_ECSCHNORR_SHA_384 << QByteArray::fromHex("04007F000702020B010204") << QByteArray("0.4.0.127.0.7.2.2.11.1.2.4") << QByteArray("id-PSA-ECDH-ECSchnorr-SHA-384") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 11.1.2.4");
			QTest::newRow("081") << KnownOid::ID_PSA_ECDH_ECSCHNORR_SHA_512 << QByteArray::fromHex("04007F000702020B010205") << QByteArray("0.4.0.127.0.7.2.2.11.1.2.5") << QByteArray("id-PSA-ECDH-ECSchnorr-SHA-512") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 11.1.2.5");

			QTest::newRow("082") << KnownOid::ID_PASSWORDTYPE << QByteArray::fromHex("04007F000702020C") << QByteArray("0.4.0.127.0.7.2.2.12") << QByteArray("id-PasswordType") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 12");
			QTest::newRow("083") << KnownOid::ID_PASSWORDTYPE_MRZ << QByteArray::fromHex("04007F000702020C01") << QByteArray("0.4.0.127.0.7.2.2.12.1") << QByteArray("id-PasswordType-MRZ") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 12.1");
			QTest::newRow("084") << KnownOid::ID_PASSWORDTYPE_CAN << QByteArray::fromHex("04007F000702020C02") << QByteArray("0.4.0.127.0.7.2.2.12.2") << QByteArray("id-PasswordType-CAN") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 12.2");
			QTest::newRow("085") << KnownOid::ID_PASSWORDTYPE_PIN << QByteArray::fromHex("04007F000702020C03") << QByteArray("0.4.0.127.0.7.2.2.12.3") << QByteArray("id-PasswordType-PIN") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 12.3");
			QTest::newRow("086") << KnownOid::ID_PASSWORDTYPE_PUK << QByteArray::fromHex("04007F000702020C04") << QByteArray("0.4.0.127.0.7.2.2.12.4") << QByteArray("id-PasswordType-PUK") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 12.4");

			QTest::newRow("087") << KnownOid::ID_AUXILIARY_DATA << QByteArray::fromHex("04007F0007030104") << QByteArray("0.4.0.127.0.7.3.1.4") << QByteArray("id-AuxiliaryData") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Object Identifier A.7.5.1");
			QTest::newRow("088") << KnownOid::ID_DATE_OF_BIRTH << QByteArray::fromHex("04007F000703010401") << QByteArray("0.4.0.127.0.7.3.1.4.1") << QByteArray("id-DateOfBirth") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Age Verification A.7.5.2");
			QTest::newRow("089") << KnownOid::ID_DATE_OF_EXPIRY << QByteArray::fromHex("04007F000703010402") << QByteArray("0.4.0.127.0.7.3.1.4.2") << QByteArray("id-DateOfExpiry") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Document Validity Verification A.7.5.3");
			QTest::newRow("090") << KnownOid::ID_MUNICIPALITY_ID << QByteArray::fromHex("04007F000703010403") << QByteArray("0.4.0.127.0.7.3.1.4.3") << QByteArray("id-MunicipalityID") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Municipality ID Verification A.7.5.4");
			QTest::newRow("091") << KnownOid::ID_PSM_MESSAGE << QByteArray::fromHex("04007F000703010404") << QByteArray("0.4.0.127.0.7.3.1.4.4") << QByteArray("id-PSM-Message") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Pseudonymous Signature of Messages A.7.5.5");

			QTest::newRow("092") << KnownOid::ID_SECURITY_OBJECT << QByteArray::fromHex("04007F0007030201") << QByteArray("0.4.0.127.0.7.3.2.1") << QByteArray("id-SecurityObject") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Signature Format for CardSecurity and ChipSecurity A.1.2.5");

			QTest::newRow("093") << KnownOid::ID_ROLES << QByteArray::fromHex("04007F0007030102") << QByteArray("0.4.0.127.0.7.3.1.2") << QByteArray("id-roles") << QByteArray("BSI TR-03110 Part 4 Version 2.21 Applications and Terminals 2");
			QTest::newRow("094") << KnownOid::ID_IS << QByteArray::fromHex("04007F000703010201") << QByteArray("0.4.0.127.0.7.3.1.2.1") << QByteArray("id-IS") << QByteArray("BSI TR-03110 Part 4 Version 2.21 Authorization 2.1.3.2");
			QTest::newRow("095") << KnownOid::ID_AT << QByteArray::fromHex("04007F000703010202") << QByteArray("0.4.0.127.0.7.3.1.2.2") << QByteArray("id-AT") << QByteArray("BSI TR-03110 Part 4 Version 2.21 Authorization 2.2.3.2");
			QTest::newRow("096") << KnownOid::ID_ST << QByteArray::fromHex("04007F000703010203") << QByteArray("0.4.0.127.0.7.3.1.2.3") << QByteArray("id-ST") << QByteArray("BSI TR-03110 Part 3 Version 2.11 Signature Terminals C.4.3");

			QTest::newRow("097") << KnownOid::ID_EXTENSIONS << QByteArray::fromHex("04007F0007030103") << QByteArray("0.4.0.127.0.7.3.1.3") << QByteArray("id-extensions") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Certificate Extensions for Terminal Authentication Version 2 C.3");
			QTest::newRow("098") << KnownOid::ID_DESCRIPTION << QByteArray::fromHex("04007F000703010301") << QByteArray("0.4.0.127.0.7.3.1.3.1") << QByteArray("id-description") << QByteArray("BSI TR-03110 Part 4 Version 2.21 Certificate Description Extension 2.2.6");
			QTest::newRow("099") << KnownOid::ID_PLAIN_FORMAT << QByteArray::fromHex("04007F00070301030101") << QByteArray("0.4.0.127.0.7.3.1.3.1.1") << QByteArray("id-plainFormat") << QByteArray("BSI TR-03110 Part 4 Version 2.21 Plain Text Format 2.2.6.1");
			QTest::newRow("100") << KnownOid::ID_HTML_FORMAT << QByteArray::fromHex("04007F00070301030102") << QByteArray("0.4.0.127.0.7.3.1.3.1.2") << QByteArray("id-htmlFormat") << QByteArray("BSI TR-03110 Part 4 Version 2.21 HTML Format 2.2.6.2");
			QTest::newRow("101") << KnownOid::ID_PFD_FORMAT << QByteArray::fromHex("04007F00070301030103") << QByteArray("0.4.0.127.0.7.3.1.3.1.3") << QByteArray("id-pdfFormat") << QByteArray("BSI TR-03110 Part 4 Version 2.21 PDF Format 2.2.6.3");
			QTest::newRow("102") << KnownOid::ID_SECTOR_RI << QByteArray::fromHex("04007F000703010302") << QByteArray("0.4.0.127.0.7.3.1.3.2") << QByteArray("id-sector-ri") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Sector for Restricted Identification C.3.2.1");
			QTest::newRow("103") << KnownOid::ID_SECTOR_PS << QByteArray::fromHex("04007F000703010303") << QByteArray("0.4.0.127.0.7.3.1.3.3") << QByteArray("id-sector-ps") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Sector for Pseudonymous Signatures C.3.2.2");

			QTest::newRow("104") << KnownOid::ID_EID_TYPE << QByteArray::fromHex("04007F0007030203") << QByteArray("0.4.0.127.0.7.3.2.3") << QByteArray("id-eIDType") << QByteArray("Draft Smart-eID");
			QTest::newRow("105") << KnownOid::ID_CARD_EID_TYPE << QByteArray::fromHex("04007F000703020301") << QByteArray("0.4.0.127.0.7.3.2.3.1") << QByteArray("id-cardEIDType") << QByteArray("Draft Smart-eID - 1");
			QTest::newRow("106") << KnownOid::ID_MOBILE_EID_TYPE << QByteArray::fromHex("04007F000703020302") << QByteArray("0.4.0.127.0.7.3.2.3.2") << QByteArray("id-mobileEIDType") << QByteArray("Draft Smart-eID - 2");
			QTest::newRow("107") << KnownOid::ID_MOBILE_EID_TYPE_SE_CERTIFIED << QByteArray::fromHex("04007F00070302030201") << QByteArray("0.4.0.127.0.7.3.2.3.2.1") << QByteArray("id-mobileEIDType-SECertified") << QByteArray("Draft Smart-eID - 2.1");
			QTest::newRow("108") << KnownOid::ID_MOBILE_EID_TYPE_SE_ENDORSED << QByteArray::fromHex("04007F00070302030202") << QByteArray("0.4.0.127.0.7.3.2.3.2.2") << QByteArray("id-mobileEIDType-SEEndorsed") << QByteArray("Draft Smart-eID - 2.2");
			QTest::newRow("109") << KnownOid::ID_MOBILE_EID_TYPE_HW_KEYSTORE << QByteArray::fromHex("04007F00070302030203") << QByteArray("0.4.0.127.0.7.3.2.3.2.3") << QByteArray("id-mobileEIDType-HWKeyStore") << QByteArray("Draft Smart-eID - 2.3");

			mTestAllKnownOids = true;
		}


		void knownOids()
		{
			QFETCH(KnownOid, knownOid);
			QFETCH(QByteArray, bin);
			QFETCH(QByteArray, oid);
			QFETCH(QByteArray, sn);
			QFETCH(QByteArray, ln);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			ASN1_OBJECT* objectFromOid = OBJ_txt2obj(oid.constData(), 1);
			ASN1_OBJECT* objectFromSn = OBJ_txt2obj(sn.constData(), 0);
			ASN1_OBJECT* objectFromLn = OBJ_txt2obj(ln.constData(), 0);

			const auto guard = qScopeGuard([objectFromOid, objectFromSn, objectFromLn] {
						ASN1_OBJECT_free(objectFromOid);
						ASN1_OBJECT_free(objectFromSn);
						ASN1_OBJECT_free(objectFromLn);
					});

			QVERIFY(OBJ_obj2nid(objectFromOid) > 0);
			QVERIFY(OBJ_obj2nid(objectFromSn) > 0);
			QVERIFY(OBJ_obj2nid(objectFromLn) > 0);

			QVERIFY(objectFromOid == objectFromSn);
			QVERIFY(objectFromOid == objectFromLn);

			Oid oidfromObject(objectFromOid);
			Oid oidFromEnum(knownOid);
			Oid oidFromBin = Oid(bin);

			QVERIFY(oidfromObject == oidFromEnum);
			QVERIFY(oidfromObject == oidFromBin);

			QCOMPARE(QByteArray(oidFromEnum), bin);

			QVERIFY(logSpy.count() == 0);

			mOidsToTest.remove(knownOid);
		}


		void logging()
		{
			QTest::ignoreMessage(QtCriticalMsg, QRegularExpression("Invalid NID, was not able to create a valid OID for: 9999999$"_L1));
			Oid invalidNid(static_cast<KnownOid>(9999999));

			QTest::ignoreMessage(QtCriticalMsg, QRegularExpression("Copying failed, was not able to create a valid OID$"_L1));
			Oid nullObject(nullptr);

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Unknown OID: 1.2.3.4.5.6$"_L1));
			ASN1_OBJECT* obj = OBJ_txt2obj("1.2.3.4.5.6", 0);
			Oid undefined1(obj);
			ASN1_OBJECT_free(obj);

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Unknown OID: 2.25.110.118.97.108.105.100$"_L1));
			Oid invalid(QByteArray("invalid"));

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Unknown OID: 1.9.46.50.46.51.46.52.46.53.46.54$"_L1));
			Oid undefined2(QByteArray("1.2.3.4.5.6"));
		}


};

QTEST_GUILESS_MAIN(test_Oid)
#include "test_Oid.moc"
