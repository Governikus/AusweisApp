/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/KnownOIDs.h"
#include "pace/KeyDerivationFunction.h"

#include "LogHandler.h"
#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;

/**
 * Test data generated with ocard_client
 */
class test_KeyDerivationFunction
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void unknownAlgorithm()
		{
			KeyDerivationFunction kdf("unknown-algorithm");

			QByteArray key = kdf.pi("123456");

			QVERIFY(!kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray());
		}


		void desKey()
		{
			QSignalSpy spyLog(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			KeyDerivationFunction kdf = toByteArray(KnownOIDs::id_PACE::ECDH::GM_3DES_CBC_CBC);

			QCOMPARE(spyLog.count(), 1);
			QVERIFY(TestFileHelper::containsLog(spyLog, QLatin1String("3DES not supported")));
			QVERIFY(!kdf.isInitialized());
		}


		void aes128Key()
		{
			KeyDerivationFunction kdf = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_128);

			QByteArray key = kdf.pi("123456");

			QVERIFY(kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray("591468cda83d65219cccb8560233600f"));
		}


		void aes196Key()
		{
			KeyDerivationFunction kdf = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_192);

			QByteArray key = kdf.pi("123456");

			QVERIFY(kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray("8df3278fb32026e66277357fcd6c826dbeb3de32088b2531"));
		}


		void aes256Key()
		{
			KeyDerivationFunction kdf = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_256);

			QByteArray key = kdf.pi("123456");

			QVERIFY(kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray("8df3278fb32026e66277357fcd6c826dbeb3de32088b2531757d753940185923"));
		}


};

QTEST_GUILESS_MAIN(test_KeyDerivationFunction)
#include "test_KeyDerivationFunction.moc"
