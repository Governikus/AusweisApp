/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "pace/CipherMac.h"

#include "SecurityProtocol.h"
#include "pace/KeyDerivationFunction.h"

#include <QtTest>


using namespace governikus;


/**
 * Test data generated with ocard_client
 */
class test_CipherMAC
	: public QObject
{
	Q_OBJECT

	private:
		const char* DATA = "jvnjksdhjkfkladj";
		const char* DATA2 = "xyzjksdhjkfklxyz";

	private Q_SLOTS:
		void unknownAlgorithm()
		{
			SecurityProtocol securityProtocol(Oid(QByteArray("unknown-algorithm")));
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.mac("123456");
			CipherMac cipherMac(securityProtocol, key);

			QByteArray mac = cipherMac.generate(DATA);

			QVERIFY(!cipherMac.isInitialized());
			QCOMPARE(mac.toHex(), QByteArray());
		}


		void wrongKeySize()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_256);
			QByteArray key("123456");
			CipherMac cipherMac(securityProtocol, key);

			QByteArray mac = cipherMac.generate(DATA);

			QVERIFY(!cipherMac.isInitialized());
			QCOMPARE(mac.toHex(), QByteArray());
		}


		void tripleDes()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_3DES_CBC_CBC);
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.mac("123456");
			CipherMac cipherMac(securityProtocol, key);

			QByteArray mac = cipherMac.generate(DATA);

			QVERIFY(!cipherMac.isInitialized());
			QCOMPARE(mac.toHex(), QByteArray());
		}


		void aes128()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128);
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.mac("123456");
			CipherMac cipherMac(securityProtocol, key);

			QByteArray mac = cipherMac.generate(DATA);

			QVERIFY(cipherMac.isInitialized());
			QCOMPARE(mac.toHex(), QByteArray("6da545f6f0b3f5c5"));
		}


		void aes196()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_192);
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.mac("123456");
			CipherMac cipherMac(securityProtocol, key);

			QByteArray mac = cipherMac.generate(DATA);

			QVERIFY(cipherMac.isInitialized());
			QCOMPARE(mac.toHex(), QByteArray("c6a55dcfea2fc789"));
		}


		void aes256()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_256);
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.mac("123456");
			CipherMac cipherMac(securityProtocol, key);

			QByteArray mac = cipherMac.generate(DATA);

			QVERIFY(cipherMac.isInitialized());
			QCOMPARE(mac.toHex(), QByteArray("1759c6c914394042"));
		}


		void multipleUse()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_256);
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.mac("123456");
			CipherMac cipherMac(securityProtocol, key);

			QVERIFY(cipherMac.isInitialized());
			QCOMPARE(cipherMac.generate(DATA).toHex(), QByteArray("1759c6c914394042"));
			QCOMPARE(cipherMac.generate(DATA2).toHex(), QByteArray("70ce88944532f37e"));
			QCOMPARE(cipherMac.generate(DATA).toHex(), QByteArray("1759c6c914394042"));
		}


};

QTEST_GUILESS_MAIN(test_CipherMAC)
#include "test_CipherMAC.moc"
