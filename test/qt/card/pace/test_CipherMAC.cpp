/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/KnownOIDs.h"
#include "pace/CipherMac.h"
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

	private Q_SLOTS:
		void unknownAlgorithm()
		{
			QByteArray paceAlgo("unknown-algorithm");
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.mac("123456");
			CipherMac cipherMac(paceAlgo, key);

			QByteArray mac = cipherMac.generate(DATA);

			QVERIFY(!cipherMac.isInitialized());
			QCOMPARE(mac.toHex(), QByteArray());
		}


		void wrongKeySize()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_256);
			QByteArray key("123456");
			CipherMac cipherMac(paceAlgo, key);

			QByteArray mac = cipherMac.generate(DATA);

			QVERIFY(!cipherMac.isInitialized());
			QCOMPARE(mac.toHex(), QByteArray());
		}


		void tripleDes()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_3DES_CBC_CBC);
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.mac("123456");
			CipherMac cipherMac(paceAlgo, key);

			QByteArray mac = cipherMac.generate(DATA);

			QVERIFY(!cipherMac.isInitialized());
			QCOMPARE(mac.toHex(), QByteArray());
		}


		void aes128()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_128);
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.mac("123456");
			CipherMac cipherMac(paceAlgo, key);

			QByteArray mac = cipherMac.generate(DATA);

			QVERIFY(cipherMac.isInitialized());
			QCOMPARE(mac.toHex(), QByteArray("6da545f6f0b3f5c5"));
		}


		void aes196()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_192);
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.mac("123456");
			CipherMac cipherMac(paceAlgo, key);

			QByteArray mac = cipherMac.generate(DATA);

			QVERIFY(cipherMac.isInitialized());
			QCOMPARE(mac.toHex(), QByteArray("c6a55dcfea2fc789"));
		}


		void aes256()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_256);
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.mac("123456");
			CipherMac cipherMac(paceAlgo, key);

			QByteArray mac = cipherMac.generate(DATA);

			QVERIFY(cipherMac.isInitialized());
			QCOMPARE(mac.toHex(), QByteArray("1759c6c914394042"));
		}


		void multipleuse()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_256);
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.mac("123456");
			CipherMac cipherMac(paceAlgo, key);

			QCOMPARE(cipherMac.generate(DATA).toHex(), QByteArray("1759c6c914394042"));
			QCOMPARE(cipherMac.generate(DATA).toHex(), QByteArray("1759c6c914394042"));
			QVERIFY(cipherMac.isInitialized());
			QCOMPARE(cipherMac.generate(DATA).toHex(), QByteArray("1759c6c914394042"));
		}


};

QTEST_GUILESS_MAIN(test_CipherMAC)
#include "test_CipherMAC.moc"
