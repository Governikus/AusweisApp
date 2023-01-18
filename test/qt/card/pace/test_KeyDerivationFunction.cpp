/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "pace/KeyDerivationFunction.h"

#include "LogHandler.h"
#include "SecurityProtocol.h"
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
			SecurityProtocol securityProtocol(Oid(QByteArray("unknown-algorithm")));
			KeyDerivationFunction kdf(securityProtocol);

			QByteArray key = kdf.pi("123456");

			QVERIFY(!kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray());
		}


		void desKey()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_3DES_CBC_CBC);
			KeyDerivationFunction kdf(securityProtocol);

			QVERIFY(logSpy.count() > 0);
			QVERIFY(TestFileHelper::containsLog(logSpy, QLatin1String("3DES not supported")));
			QVERIFY(!kdf.isInitialized());
		}


		void aes128Key()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128);
			KeyDerivationFunction kdf(securityProtocol);

			QByteArray key = kdf.pi("123456");

			QVERIFY(kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray("591468cda83d65219cccb8560233600f"));
		}


		void aes196Key()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_192);
			KeyDerivationFunction kdf(securityProtocol);

			QByteArray key = kdf.pi("123456");

			QVERIFY(kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray("8df3278fb32026e66277357fcd6c826dbeb3de32088b2531"));
		}


		void aes256Key()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_256);
			KeyDerivationFunction kdf(securityProtocol);

			QByteArray key = kdf.pi("123456");

			QVERIFY(kdf.isInitialized());
			QCOMPARE(key.toHex(), QByteArray("8df3278fb32026e66277357fcd6c826dbeb3de32088b2531757d753940185923"));
		}


};

QTEST_GUILESS_MAIN(test_KeyDerivationFunction)
#include "test_KeyDerivationFunction.moc"
