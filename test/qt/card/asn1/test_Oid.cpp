/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/Oid.h"

#include "LogHandler.h"
#include "asn1/ASN1TemplateUtil.h"

#include <QtCore>
#include <QtTest>
#include <openssl/objects.h>

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
			QCOMPARE(oid.getData(), QByteArray());

			QCOMPARE(logSpy.count(), 0);

		}


		void copyContructor_data()
		{
			QTest::addColumn<QByteArray>("oid");
			QTest::addColumn<bool>("warnings");

			QTest::newRow("openssl") << QByteArray("1.2.840.113549.1.7.2") << false;
			QTest::newRow("known") << QByteArray("0.4.0.127.0.7.2.2.4") << false;
			QTest::newRow("unknown") << QByteArray("0.4.0.127.0.7.2.0") << true;
		}


		void copyContructor()
		{
			QFETCH(QByteArray, oid);
			QFETCH(bool, warnings);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			Oid source(oid);
			Oid target(source);
			QCOMPARE(source, target);
			QCOMPARE(logSpy.count() > 0, warnings);
		}


		void moveContructor_data()
		{
			QTest::addColumn<QByteArray>("oid");
			QTest::addColumn<bool>("warnings");

			QTest::newRow("openssl") << QByteArray("1.2.840.113549.1.7.2") << false;
			QTest::newRow("known") << QByteArray("0.4.0.127.0.7.2.2.4") << false;
			QTest::newRow("unknown") << QByteArray("0.4.0.127.0.7.2.0") << true;
		}


		void moveContructor()
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

			QTest::newRow("openssl") << QByteArray("1.2.840.113549.1.7.2") << false;
			QTest::newRow("known") << QByteArray("0.4.0.127.0.7.2.2.4") << false;
			QTest::newRow("unknown") << QByteArray("0.4.0.127.0.7.2.0") << true;
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

			QTest::newRow("openssl") << QByteArray("1.2.840.113549.1.7.2") << false;
			QTest::newRow("known") << QByteArray("0.4.0.127.0.7.2.2.4") << false;
			QTest::newRow("unknown") << QByteArray("0.4.0.127.0.7.2.0") << true;
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
			QTest::addColumn<QByteArray>("data");
			QTest::addColumn<QByteArray>("logging");
			QTest::addColumn<bool>("warnings");

			QTest::newRow("openssl") << QByteArray("1.2.840.113549.1.7.2") << QByteArray::fromHex("2A864886F70D010702") << QByteArray("1.2.840.113549.1.7.2 (pkcs7-signedData)") << false;
			QTest::newRow("known") << QByteArray("0.4.0.127.0.7.2.2.4") << QByteArray::fromHex("04007F0007020204") << QByteArray("0.4.0.127.0.7.2.2.4 (id-PACE)") << false;
			QTest::newRow("unknown") << QByteArray("0.4.0.127.0.7.2.0") << QByteArray::fromHex("04007F00070200") << QByteArray("0.4.0.127.0.7.2.0") << true;
		}


		void getter()
		{
			QFETCH(QByteArray, oid);
			QFETCH(QByteArray, data);
			QFETCH(QByteArray, logging);
			QFETCH(bool, warnings);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			Oid oidObject(oid);

			QCOMPARE(oidObject.getData(), data);
			QCOMPARE(oidObject.operator QByteArray(), logging);
			QCOMPARE(logSpy.count() > 0, warnings);
		}


		void knownOids_data()
		{
			QTest::addColumn<KnownOid>("knownOid");
			QTest::addColumn<QByteArray>("oid");
			QTest::addColumn<QByteArray>("sn");
			QTest::addColumn<QByteArray>("ln");

			QTest::newRow("01") << KnownOid::ID_SIGNED_DATA << QByteArray("1.2.840.113549.1.7.2") << QByteArray("pkcs7-signedData") << QByteArray("pkcs7-signedData");

			QTest::newRow("02") << KnownOid::EC_PS_PUBLICKEY << QByteArray("0.4.0.127.0.7.1.1.2.3") << QByteArray("ecPSPublicKey") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Static keys for Pseudonymous Signatures A.6.1");

			QTest::newRow("03") << KnownOid::STANDARDIZED_DOMAINPARAMETERS << QByteArray("0.4.0.127.0.7.1.2") << QByteArray("standardizedDomainParameters") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Standardized Domain Parameters A.2.1.1");

			QTest::newRow("04") << KnownOid::BSI_DE_PROTOCOLS_SMARTCARD << QByteArray("0.4.0.127.0.7.2.2") << QByteArray("bsi-de-protocols-smartcard") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Supported Protocols A.1.1");

			QTest::newRow("05") << KnownOid::ID_PK << QByteArray("0.4.0.127.0.7.2.2.1") << QByteArray("id-PK") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication Key A.1.1.2");
			QTest::newRow("06") << KnownOid::ID_PK_DH << QByteArray("0.4.0.127.0.7.2.2.1.1") << QByteArray("id-PK-DH") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication Key A.1.1.2 - 1");
			QTest::newRow("07") << KnownOid::ID_PK_ECDH << QByteArray("0.4.0.127.0.7.2.2.1.2") << QByteArray("id-PK-ECDH") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication Key A.1.1.2 - 2");

			QTest::newRow("08") << KnownOid::ID_TA << QByteArray("0.4.0.127.0.7.2.2.2") << QByteArray("id-TA") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3");
			QTest::newRow("09") << KnownOid::ID_TA_RSA << QByteArray("0.4.0.127.0.7.2.2.2.1") << QByteArray("id-TA-RSA") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1");
			QTest::newRow("10") << KnownOid::ID_TA_RSA_V1_5_SHA_1 << QByteArray("0.4.0.127.0.7.2.2.2.1.1") << QByteArray("id-TA-RSA-v1-5-SHA-1") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.1");
			QTest::newRow("11") << KnownOid::ID_TA_RSA_V1_5_SHA_256 << QByteArray("0.4.0.127.0.7.2.2.2.1.2") << QByteArray("id-TA-RSA-v1-5-SHA-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.2");
			QTest::newRow("12") << KnownOid::ID_TA_RSA_PSS_SHA_1 << QByteArray("0.4.0.127.0.7.2.2.2.1.3") << QByteArray("id-TA-RSA-PSS-SHA-1") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.3");
			QTest::newRow("13") << KnownOid::ID_TA_RSA_PSS_SHA_256 << QByteArray("0.4.0.127.0.7.2.2.2.1.4") << QByteArray("id-TA-RSA-PSS-SHA-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.4");
			QTest::newRow("14") << KnownOid::ID_TA_RSA_V1_5_SHA_512 << QByteArray("0.4.0.127.0.7.2.2.2.1.5") << QByteArray("id-TA-RSA-v1-5-SHA-512") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.5");
			QTest::newRow("15") << KnownOid::ID_TA_RSA_PSS_SHA_512 << QByteArray("0.4.0.127.0.7.2.2.2.1.6") << QByteArray("id-TA-RSA-PSS-SHA-512") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 1.6");
			QTest::newRow("16") << KnownOid::ID_TA_ECDSA << QByteArray("0.4.0.127.0.7.2.2.2.2") << QByteArray("id-TA-ECDSA") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2");
			QTest::newRow("17") << KnownOid::ID_TA_ECDSA_SHA_1 << QByteArray("0.4.0.127.0.7.2.2.2.2.1") << QByteArray("id-TA-ECDSA-SHA-1") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.1");
			QTest::newRow("18") << KnownOid::ID_TA_ECDSA_SHA_224 << QByteArray("0.4.0.127.0.7.2.2.2.2.2") << QByteArray("id-TA-ECDSA-SHA-224") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.2");
			QTest::newRow("19") << KnownOid::ID_TA_ECDSA_SHA_256 << QByteArray("0.4.0.127.0.7.2.2.2.2.3") << QByteArray("id-TA-ECDSA-SHA-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.3");
			QTest::newRow("20") << KnownOid::ID_TA_ECDSA_SHA_384 << QByteArray("0.4.0.127.0.7.2.2.2.2.4") << QByteArray("id-TA-ECDSA-SHA-384") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.4");
			QTest::newRow("21") << KnownOid::ID_TA_ECDSA_SHA_512 << QByteArray("0.4.0.127.0.7.2.2.2.2.5") << QByteArray("id-TA-ECDSA-SHA-512") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Terminal Authentication A.1.1.3 - 2.5");

			QTest::newRow("22") << KnownOid::ID_CA << QByteArray("0.4.0.127.0.7.2.2.3") << QByteArray("id-CA") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2");
			QTest::newRow("23") << KnownOid::ID_CA_DH << QByteArray("0.4.0.127.0.7.2.2.3.1") << QByteArray("id-CA-DH") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1");
			QTest::newRow("24") << KnownOid::ID_CA_DH_3DES_CBC_CBC << QByteArray("0.4.0.127.0.7.2.2.3.1.1") << QByteArray("id-CA-DH-3DES-CBC-CBC") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1.1");
			QTest::newRow("25") << KnownOid::ID_CA_DH_AES_CBC_CMAC_128 << QByteArray("0.4.0.127.0.7.2.2.3.1.2") << QByteArray("id-CA-DH-AES-CBC-CMAC-128") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1.2");
			QTest::newRow("26") << KnownOid::ID_CA_DH_AES_CBC_CMAC_192 << QByteArray("0.4.0.127.0.7.2.2.3.1.3") << QByteArray("id-CA-DH-AES-CBC-CMAC-192") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1.3");
			QTest::newRow("27") << KnownOid::ID_CA_DH_AES_CBC_CMAC_256 << QByteArray("0.4.0.127.0.7.2.2.3.1.4") << QByteArray("id-CA-DH-AES-CBC-CMAC-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 1.4");
			QTest::newRow("28") << KnownOid::ID_CA_ECDH << QByteArray("0.4.0.127.0.7.2.2.3.2") << QByteArray("id-CA-ECDH") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 2");
			QTest::newRow("29") << KnownOid::ID_CA_ECDH_3DES_CBC_CBC << QByteArray("0.4.0.127.0.7.2.2.3.2.1") << QByteArray("id-CA-ECDH-3DES-CBC-CBC") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 2.1");
			QTest::newRow("30") << KnownOid::ID_CA_ECDH_AES_CBC_CMAC_128 << QByteArray("0.4.0.127.0.7.2.2.3.2.2") << QByteArray("id-CA-ECDH-AES-CBC-CMAC-128") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 2.2");
			QTest::newRow("31") << KnownOid::ID_CA_ECDH_AES_CBC_CMAC_192 << QByteArray("0.4.0.127.0.7.2.2.3.2.3") << QByteArray("id-CA-ECDH-AES-CBC-CMAC-192") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 2.3");
			QTest::newRow("32") << KnownOid::ID_CA_ECDH_AES_CBC_CMAC_256 << QByteArray("0.4.0.127.0.7.2.2.3.2.4") << QByteArray("id-CA-ECDH-AES-CBC-CMAC-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Chip Authentication A.1.1.2 - 2.4");

			QTest::newRow("33") << KnownOid::ID_PACE << QByteArray("0.4.0.127.0.7.2.2.4") << QByteArray("id-PACE") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1");
			QTest::newRow("34") << KnownOid::ID_PACE_DH_GM << QByteArray("0.4.0.127.0.7.2.2.4.1") << QByteArray("id-PACE-DH-GM") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 1");
			QTest::newRow("35") << KnownOid::ID_PACE_DH_GM_3DES_CBC_CBC << QByteArray("0.4.0.127.0.7.2.2.4.1.1") << QByteArray("id-PACE-DH-GM-3DES-CBC-CBC") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 1.1");
			QTest::newRow("36") << KnownOid::ID_PACE_DH_GM_AES_CBC_CMAC_128 << QByteArray("0.4.0.127.0.7.2.2.4.1.2") << QByteArray("id-PACE-DH-GM-AES-CBC-CMAC-128") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 1.2");
			QTest::newRow("37") << KnownOid::ID_PACE_DH_GM_AES_CBC_CMAC_192 << QByteArray("0.4.0.127.0.7.2.2.4.1.3") << QByteArray("id-PACE-DH-GM-AES-CBC-CMAC-192") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 1.3");
			QTest::newRow("38") << KnownOid::ID_PACE_DH_GM_AES_CBC_CMAC_256 << QByteArray("0.4.0.127.0.7.2.2.4.1.4") << QByteArray("id-PACE-DH-GM-AES-CBC-CMAC-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 1.4");
			QTest::newRow("39") << KnownOid::ID_PACE_ECDH_GM << QByteArray("0.4.0.127.0.7.2.2.4.2") << QByteArray("id-PACE-ECDH-GM") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 2");
			QTest::newRow("40") << KnownOid::ID_PACE_ECDH_GM_3DES_CBC_CBC << QByteArray("0.4.0.127.0.7.2.2.4.2.1") << QByteArray("id-PACE-ECDH-GM-3DES-CBC-CBC") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 2.1");
			QTest::newRow("41") << KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128 << QByteArray("0.4.0.127.0.7.2.2.4.2.2") << QByteArray("id-PACE-ECDH-GM-AES-CBC-CMAC-128") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 2.2");
			QTest::newRow("42") << KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_192 << QByteArray("0.4.0.127.0.7.2.2.4.2.3") << QByteArray("id-PACE-ECDH-GM-AES-CBC-CMAC-192") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 2.3");
			QTest::newRow("43") << KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_256 << QByteArray("0.4.0.127.0.7.2.2.4.2.4") << QByteArray("id-PACE-ECDH-GM-AES-CBC-CMAC-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 2.4");
			QTest::newRow("44") << KnownOid::ID_PACE_DH_IM << QByteArray("0.4.0.127.0.7.2.2.4.3") << QByteArray("id-PACE-DH-IM") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 3");
			QTest::newRow("45") << KnownOid::ID_PACE_DH_IM_3DES_CBC_CBC << QByteArray("0.4.0.127.0.7.2.2.4.3.1") << QByteArray("id-PACE-DH-IM-3DES-CBC-CBC") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 3.1");
			QTest::newRow("46") << KnownOid::ID_PACE_DH_IM_AES_CBC_CMAC_128 << QByteArray("0.4.0.127.0.7.2.2.4.3.2") << QByteArray("id-PACE-DH-IM-AES-CBC-CMAC-128") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 3.2");
			QTest::newRow("47") << KnownOid::ID_PACE_DH_IM_AES_CBC_CMAC_192 << QByteArray("0.4.0.127.0.7.2.2.4.3.3") << QByteArray("id-PACE-DH-IM-AES-CBC-CMAC-192") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 3.3");
			QTest::newRow("48") << KnownOid::ID_PACE_DH_IM_AES_CBC_CMAC_256 << QByteArray("0.4.0.127.0.7.2.2.4.3.4") << QByteArray("id-PACE-DH-IM-AES-CBC-CMAC-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 3.4");
			QTest::newRow("49") << KnownOid::ID_PACE_ECDH_IM << QByteArray("0.4.0.127.0.7.2.2.4.4") << QByteArray("id-PACE-ECDH-IM") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4");
			QTest::newRow("50") << KnownOid::ID_PACE_ECDH_IM_3DES_CBC_CBC << QByteArray("0.4.0.127.0.7.2.2.4.4.1") << QByteArray("id-PACE-ECDH-IM-3DES-CBC-CBC") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.1");
			QTest::newRow("51") << KnownOid::ID_PACE_ECDH_IM_AES_CBC_CMAC_128 << QByteArray("0.4.0.127.0.7.2.2.4.4.2") << QByteArray("id-PACE-ECDH-IM-AES-CBC-CMAC-128") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.2");
			QTest::newRow("52") << KnownOid::ID_PACE_ECDH_IM_AES_CBC_CMAC_192 << QByteArray("0.4.0.127.0.7.2.2.4.4.3") << QByteArray("id-PACE-ECDH-IM-AES-CBC-CMAC-192") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.3");
			QTest::newRow("53") << KnownOid::ID_PACE_ECDH_IM_AES_CBC_CMAC_256 << QByteArray("0.4.0.127.0.7.2.2.4.4.4") << QByteArray("id-PACE-ECDH-IM-AES-CBC-CMAC-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PACE A.1.1.1 - 4.4");

			QTest::newRow("54") << KnownOid::ID_RI << QByteArray("0.4.0.127.0.7.2.2.5") << QByteArray("id-RI") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4");
			QTest::newRow("55") << KnownOid::ID_RI_DH << QByteArray("0.4.0.127.0.7.2.2.5.1") << QByteArray("id-RI-DH") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1");
			QTest::newRow("56") << KnownOid::ID_RI_DH_SHA_1 << QByteArray("0.4.0.127.0.7.2.2.5.1.1") << QByteArray("id-RI-DH-SHA-1") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.1");
			QTest::newRow("57") << KnownOid::ID_RI_DH_SHA_224 << QByteArray("0.4.0.127.0.7.2.2.5.1.2") << QByteArray("id-RI-DH-SHA-224") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.2");
			QTest::newRow("58") << KnownOid::ID_RI_DH_SHA_256 << QByteArray("0.4.0.127.0.7.2.2.5.1.3") << QByteArray("id-RI-DH-SHA-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.3");
			QTest::newRow("59") << KnownOid::ID_RI_DH_SHA_384 << QByteArray("0.4.0.127.0.7.2.2.5.1.4") << QByteArray("id-RI-DH-SHA-384") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.4");
			QTest::newRow("60") << KnownOid::ID_RI_DH_SHA_512 << QByteArray("0.4.0.127.0.7.2.2.5.1.5") << QByteArray("id-RI-DH-SHA-512") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 1.5");
			QTest::newRow("61") << KnownOid::ID_RI_ECDH << QByteArray("0.4.0.127.0.7.2.2.5.2") << QByteArray("id-RI-ECDH") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2");
			QTest::newRow("62") << KnownOid::ID_RI_ECDH_SHA_1 << QByteArray("0.4.0.127.0.7.2.2.5.2.1") << QByteArray("id-RI-ECDH-SHA-1") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.1");
			QTest::newRow("63") << KnownOid::ID_RI_ECDH_SHA_224 << QByteArray("0.4.0.127.0.7.2.2.5.2.2") << QByteArray("id-RI-ECDH-SHA-224") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.2");
			QTest::newRow("64") << KnownOid::ID_RI_ECDH_SHA_256 << QByteArray("0.4.0.127.0.7.2.2.5.2.3") << QByteArray("id-RI-ECDH-SHA-256") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.3");
			QTest::newRow("65") << KnownOid::ID_RI_ECDH_SHA_384 << QByteArray("0.4.0.127.0.7.2.2.5.2.4") << QByteArray("id-RI-ECDH-SHA-384") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.4");
			QTest::newRow("66") << KnownOid::ID_RI_ECDH_SHA_512 << QByteArray("0.4.0.127.0.7.2.2.5.2.5") << QByteArray("id-RI-ECDH-SHA-512") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Restricted Identification A.1.1.4 - 2.5");

			QTest::newRow("67") << KnownOid::ID_CI << QByteArray("0.4.0.127.0.7.2.2.6") << QByteArray("id-CI") << QByteArray("BSI TR-03110 Part 3 Version 2.21 CardInfo (eIDAS token only) A.1.1.7");

			QTest::newRow("68") << KnownOid::ID_EID_SECURITY << QByteArray("0.4.0.127.0.7.2.2.7") << QByteArray("id-eIDSecurity") << QByteArray("BSI TR-03110 Part 3 Version 2.21 EIDSecurityInfo (eIDAS token only) A.1.1.8");

			QTest::newRow("69") << KnownOid::ID_PT << QByteArray("0.4.0.127.0.7.2.2.8") << QByteArray("id-PT") << QByteArray("BSI TR-03110 Part 3 Version 2.21 PrivilegedTerminalInfo (eIDAS token only) A.1.1.9");

			QTest::newRow("70") << KnownOid::ID_AUXILIARY_DATA << QByteArray("0.4.0.127.0.7.3.1.4") << QByteArray("id-AuxiliaryData") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Object Identifier A.7.5.1");
			QTest::newRow("71") << KnownOid::ID_DATE_OF_BIRTH << QByteArray("0.4.0.127.0.7.3.1.4.1") << QByteArray("id-DateOfBirth") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Age Verification A.7.5.2");
			QTest::newRow("72") << KnownOid::ID_DATE_OF_EXPIRY << QByteArray("0.4.0.127.0.7.3.1.4.2") << QByteArray("id-DateOfExpiry") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Document Validity Verification A.7.5.3");
			QTest::newRow("73") << KnownOid::ID_MUNICIPALITY_ID << QByteArray("0.4.0.127.0.7.3.1.4.3") << QByteArray("id-MunicipalityID") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Municipality ID Verification A.7.5.4");
			QTest::newRow("74") << KnownOid::ID_PSM_MESSAGE << QByteArray("0.4.0.127.0.7.3.1.4.4") << QByteArray("id-PSM-Message") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Pseudonymous Signature of Messages A.7.5.5");

			QTest::newRow("75") << KnownOid::ID_SECURITY_OBJECT << QByteArray("0.4.0.127.0.7.3.2.1") << QByteArray("id-SecurityObject") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Signature Format for CardSecurity and ChipSecurity A.1.2.5");

			QTest::newRow("76") << KnownOid::ID_ROLES << QByteArray("0.4.0.127.0.7.3.1.2") << QByteArray("id-roles") << QByteArray("BSI TR-03110 Part 4 Version 2.21 Applications and Terminals 2");
			QTest::newRow("77") << KnownOid::ID_IS << QByteArray("0.4.0.127.0.7.3.1.2.1") << QByteArray("id-IS") << QByteArray("BSI TR-03110 Part 4 Version 2.21 Authorization 2.1.3.2");
			QTest::newRow("78") << KnownOid::ID_AT << QByteArray("0.4.0.127.0.7.3.1.2.2") << QByteArray("id-AT") << QByteArray("BSI TR-03110 Part 4 Version 2.21 Authorization 2.2.3.2");
			QTest::newRow("79") << KnownOid::ID_ST << QByteArray("0.4.0.127.0.7.3.1.2.3") << QByteArray("id-ST") << QByteArray("BSI TR-03110 Part 3 Version 2.11 Signature Terminals C.4.3");

			QTest::newRow("80") << KnownOid::ID_EXTENSIONS << QByteArray("0.4.0.127.0.7.3.1.3") << QByteArray("id-extensions") << QByteArray("BSI TR-03110 Part 3 Version 2.21 Certificate Extensions for Terminal Authentication Version 2 C.3");
			QTest::newRow("81") << KnownOid::ID_DESCRIPTION << QByteArray("0.4.0.127.0.7.3.1.3.1") << QByteArray("id-description") << QByteArray("BSI TR-03110 Part 4 Version 2.21 Certificate Description Extension 2.2.6");
			QTest::newRow("82") << KnownOid::ID_PLAIN_FORMAT << QByteArray("0.4.0.127.0.7.3.1.3.1.1") << QByteArray("id-plainFormat") << QByteArray("BSI TR-03110 Part 4 Version 2.21 Plain Text Format 2.2.6.1");
			QTest::newRow("83") << KnownOid::ID_HTML_FORMAT << QByteArray("0.4.0.127.0.7.3.1.3.1.2") << QByteArray("id-htmlFormat") << QByteArray("BSI TR-03110 Part 4 Version 2.21 HTML Format 2.2.6.2");
			QTest::newRow("84") << KnownOid::ID_PFD_FORMAT << QByteArray("0.4.0.127.0.7.3.1.3.1.3") << QByteArray("id-pdfFormat") << QByteArray("BSI TR-03110 Part 4 Version 2.21 PDF Format 2.2.6.3");
			QTest::newRow("85") << KnownOid::ID_SECTOR << QByteArray("0.4.0.127.0.7.3.1.3.2") << QByteArray("id-sector") << QByteArray("BSI TR-03110 Part 3 Version 2.21  Terminal Sector for Restricted Identification C.3.2.1");

			QTest::newRow("86") << KnownOid::ID_EID_TYPE << QByteArray("0.4.0.127.0.7.3.2.3") << QByteArray("id-eIDType") << QByteArray("Draft Smart-eID");
			QTest::newRow("87") << KnownOid::ID_CARD_EID_TYPE << QByteArray("0.4.0.127.0.7.3.2.3.1") << QByteArray("id-cardEIDType") << QByteArray("Draft Smart-eID - 1");
			QTest::newRow("88") << KnownOid::ID_MOBILE_EID_TYPE << QByteArray("0.4.0.127.0.7.3.2.3.2") << QByteArray("id-mobileEIDType") << QByteArray("Draft Smart-eID - 2");
			QTest::newRow("89") << KnownOid::ID_MOBILE_EID_TYPE_SE_CERTIFIED << QByteArray("0.4.0.127.0.7.3.2.3.2.1") << QByteArray("id-mobileEIDType-SECertified") << QByteArray("Draft Smart-eID - 2.1");
			QTest::newRow("90") << KnownOid::ID_MOBILE_EID_TYPE_SE_ENDORSED << QByteArray("0.4.0.127.0.7.3.2.3.2.2") << QByteArray("id-mobileEIDType-SEEndorsed") << QByteArray("Draft Smart-eID - 2.2");
			QTest::newRow("91") << KnownOid::ID_MOBILE_EID_TYPE_HW_KEYSTORE << QByteArray("0.4.0.127.0.7.3.2.3.2.3") << QByteArray("id-mobileEIDType-HWKeyStore") << QByteArray("Draft Smart-eID - 2.3");

			mTestAllKnownOids = true;
		}


		void knownOids()
		{
			QFETCH(KnownOid, knownOid);
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
			Oid oidFromOid(oid);
			Oid oidFromSn(sn);
			Oid oidFromLn(ln);

			QVERIFY(oidfromObject == oidFromEnum);
			QVERIFY(oidfromObject == oidFromOid);
			QVERIFY(oidfromObject == oidFromSn);
			QVERIFY(oidfromObject == oidFromLn);

			QVERIFY(oidFromEnum.getData().size() > 0);
			QVERIFY(oidFromEnum.operator QByteArray().size() > 0);

			QVERIFY(logSpy.count() == 0);

			mOidsToTest.remove(knownOid);
		}


		void logging()
		{
			QTest::ignoreMessage(QtCriticalMsg, QRegularExpression("Invalid NID, was not able to create a valid OID for: 9999999$"));
			Oid invalidNid(static_cast<KnownOid>(9999999));

			QTest::ignoreMessage(QtCriticalMsg, QRegularExpression("Copying failed, was not able to create a valid OID$"));
			Oid nullObject(nullptr);

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Unknown OID: 1.2.3.4.5.6$"));
			ASN1_OBJECT* obj = OBJ_txt2obj("1.2.3.4.5.6", 0);
			Oid undefined1(obj);
			ASN1_OBJECT_free(obj);

			QTest::ignoreMessage(QtCriticalMsg, QRegularExpression("Invalid text input, was not able to create a valid OID for: \"invalid\"$"));
			Oid invalid(QByteArray("invalid"));

			QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Unknown OID: 1.2.3.4.5.6$"));
			Oid undefined2(QByteArray("1.2.3.4.5.6"));
		}


};

QTEST_GUILESS_MAIN(test_Oid)
#include "test_Oid.moc"
