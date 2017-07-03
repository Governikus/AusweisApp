/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include <QApplication>
#include <QtCore/QtCore>
#include <QtTest/QtTest>

#include "asn1/KnownOIDs.h"
#include "pace/KeyDerivationFunction.h"

using namespace governikus;

/**
 * Test data generated with ocard_client
 */
class test_KeyDerivationFunction
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void unknownAlgorithm()
		{
			KeyDerivationFunction kdf("unknown-algorithm");

			QByteArray key = kdf.pi("123456");

			QVERIFY(!kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray());
		}


		void desKey()
		{
			KeyDerivationFunction kdf(KnownOIDs::id_PACE::ECDH::GM_3DES_CBC_CBC);

			QByteArray key = kdf.pi("123456");

			QVERIFY(kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray("591468cda83d6521"));
		}


		void aes128Key()
		{
			KeyDerivationFunction kdf(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_128);

			QByteArray key = kdf.pi("123456");

			QVERIFY(kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray("591468cda83d65219cccb8560233600f"));
		}


		void aes196Key()
		{
			KeyDerivationFunction kdf(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_192);

			QByteArray key = kdf.pi("123456");

			QVERIFY(kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray("8df3278fb32026e66277357fcd6c826dbeb3de32088b2531"));
		}


		void aes256Key()
		{
			KeyDerivationFunction kdf(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_256);

			QByteArray key = kdf.pi("123456");

			QVERIFY(kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray("8df3278fb32026e66277357fcd6c826dbeb3de32088b2531757d753940185923"));
		}


};

QTEST_GUILESS_MAIN(test_KeyDerivationFunction)
#include "test_KeyDerivationFunction.moc"
