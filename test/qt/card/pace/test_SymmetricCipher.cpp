/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/KnownOIDs.h"
#include "pace/KeyDerivationFunction.h"
#include "pace/SymmetricCipher.h"

#include <openssl/evp.h>
#include <QtTest>

using namespace governikus;

class test_SymmetricCipher
	: public QObject
{
	Q_OBJECT

	private:
		const QByteArray PIN = QByteArrayLiteral("123456");
		const QByteArray DATA = QByteArray::fromRawData("Moin Moin Anton!", 16);

	private Q_SLOTS:
		void unknownAlgorithm()
		{
			QByteArray paceAlgo("unknown-algorithm");
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(paceAlgo, key);

			QVERIFY(!sc.isInitialized());
			QVERIFY(sc.encrypt(QByteArray()).isEmpty());
			QVERIFY(sc.decrypt(QByteArray()).isEmpty());
		}


		void tripleDes()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_3DES_CBC_CBC);
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(paceAlgo, key);

			QVERIFY(!sc.isInitialized());
			QVERIFY(sc.encrypt(QByteArray()).isEmpty());
			QVERIFY(sc.decrypt(QByteArray()).isEmpty());
		}


		void wrongKeySize()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_128);
			SymmetricCipher sc(paceAlgo, "dummy");

			QVERIFY(!sc.isInitialized());
			QVERIFY(sc.encrypt(QByteArray()).isEmpty());
			QVERIFY(sc.decrypt(QByteArray()).isEmpty());
		}


		void noData()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_128);
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(paceAlgo, key);

			QVERIFY(sc.encrypt(QByteArray()).isEmpty());
			QVERIFY(sc.decrypt(QByteArray()).isEmpty());
		}


		void aes128()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_128);
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(paceAlgo, key);

			QByteArray encryptedData = sc.encrypt(DATA);
			QByteArray decryptedData = sc.decrypt(encryptedData);

			QVERIFY(sc.isInitialized());
			QCOMPARE(decryptedData, DATA);
		}


		void aes196()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_192);
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(paceAlgo, key);

			QByteArray encryptedData = sc.encrypt(DATA);
			QByteArray decryptedData = sc.decrypt(encryptedData);

			QVERIFY(sc.isInitialized());
			QCOMPARE(decryptedData, DATA);
		}


		void aes256()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_256);
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(paceAlgo, key);

			QByteArray encryptedData = sc.encrypt(DATA);
			QByteArray decryptedData = sc.decrypt(encryptedData);

			QVERIFY(sc.isInitialized());
			QCOMPARE(decryptedData, DATA);
		}


		void multipleuse()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_256);
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(paceAlgo, key);

			QCOMPARE(sc.decrypt(sc.encrypt(DATA)), DATA);
			QCOMPARE(sc.decrypt(sc.encrypt(DATA)), DATA);
			QCOMPARE(sc.decrypt(sc.encrypt(DATA)), DATA);
			QCOMPARE(sc.decrypt(sc.encrypt(DATA)), DATA);
			QCOMPARE(sc.decrypt(sc.encrypt(DATA)), DATA);
		}


		void setIv()
		{
			QByteArray paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_256);
			KeyDerivationFunction kdf(paceAlgo);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(paceAlgo, key);

			QByteArray encryptedData = sc.encrypt(DATA);

			sc.setIv(QByteArray().fill(1, 16));
			QByteArray decryptedData_withIv1 = sc.decrypt(encryptedData);

			sc.setIv(QByteArray().fill(0, 16));
			QByteArray decryptedData_withIv0 = sc.decrypt(encryptedData);

			QCOMPARE(decryptedData_withIv0, DATA);
			QVERIFY(decryptedData_withIv1 != DATA);
		}


};

QTEST_GUILESS_MAIN(test_SymmetricCipher)
#include "test_SymmetricCipher.moc"
