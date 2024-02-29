/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "pace/SymmetricCipher.h"

#include "SecurityProtocol.h"
#include "pace/KeyDerivationFunction.h"

#include <QtTest>
#include <openssl/evp.h>


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
			SecurityProtocol securityProtocol(Oid(QByteArray("unknown-algorithm")));
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(securityProtocol, key);

			QVERIFY(!sc.isInitialized());
			QVERIFY(sc.encrypt(QByteArray()).isEmpty());
			QVERIFY(sc.decrypt(QByteArray()).isEmpty());
		}


		void tripleDes()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_3DES_CBC_CBC);
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(securityProtocol, key);

			QVERIFY(!sc.isInitialized());
			QVERIFY(sc.encrypt(QByteArray()).isEmpty());
			QVERIFY(sc.decrypt(QByteArray()).isEmpty());
		}


		void wrongKeySize()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128);
			SymmetricCipher sc(securityProtocol, "dummy");

			QVERIFY(!sc.isInitialized());
			QVERIFY(sc.encrypt(QByteArray()).isEmpty());
			QVERIFY(sc.decrypt(QByteArray()).isEmpty());
		}


		void noData()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128);
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(securityProtocol, key);

			QVERIFY(sc.encrypt(QByteArray()).isEmpty());
			QVERIFY(sc.decrypt(QByteArray()).isEmpty());
		}


		void aes128()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128);
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(securityProtocol, key);

			QByteArray encryptedData = sc.encrypt(DATA);
			QByteArray decryptedData = sc.decrypt(encryptedData);

			QVERIFY(sc.isInitialized());
			QCOMPARE(decryptedData, DATA);
		}


		void aes196()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_192);
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(securityProtocol, key);

			QByteArray encryptedData = sc.encrypt(DATA);
			QByteArray decryptedData = sc.decrypt(encryptedData);

			QVERIFY(sc.isInitialized());
			QCOMPARE(decryptedData, DATA);
		}


		void aes256()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_256);
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(securityProtocol, key);

			QByteArray encryptedData = sc.encrypt(DATA);
			QByteArray decryptedData = sc.decrypt(encryptedData);

			QVERIFY(sc.isInitialized());
			QCOMPARE(decryptedData, DATA);
		}


		void multipleuse()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_256);
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(securityProtocol, key);

			QCOMPARE(sc.decrypt(sc.encrypt(DATA)), DATA);
			QCOMPARE(sc.decrypt(sc.encrypt(DATA)), DATA);
			QCOMPARE(sc.decrypt(sc.encrypt(DATA)), DATA);
			QCOMPARE(sc.decrypt(sc.encrypt(DATA)), DATA);
			QCOMPARE(sc.decrypt(sc.encrypt(DATA)), DATA);
		}


		void setIv()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_256);
			KeyDerivationFunction kdf(securityProtocol);
			QByteArray key = kdf.pi(PIN);
			SymmetricCipher sc(securityProtocol, key);

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
