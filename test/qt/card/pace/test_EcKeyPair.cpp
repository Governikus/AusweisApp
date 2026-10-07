/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "pace/ec/EcKeyPair.h"

#include "pace/ec/EcUtil.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_EcKeyPair
	: public QObject
{
	Q_OBJECT

	private:
		const QByteArray mPrivateKey = QByteArray::fromHex(
			"308202050201003081EC06072A8648CE3D02013081E0020101302C06072A8648CE3D0101022100A9FB57DBA1EEA9BC3E"
			"660A909D838D726E3BF623D52620282013481D1F6E5377304404207D5A0975FC2C3057EEF67530417AFFE7FB8055C126"
			"DC5C6CE94A4B44F330B5D9042026DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B6044104"
			"8BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9"
			"C27745132DED8E545C1D54C72F046997022100A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E8297"
			"4856A70201010482010F3082010B0201010420A07EB62E891DAA84643E0AFCC1AF006891B669B8F51E379477DBEAB8C9"
			"87A610A081E33081E0020101302C06072A8648CE3D0101022100A9FB57DBA1EEA9BC3E660A909D838D726E3BF623D526"
			"20282013481D1F6E5377304404207D5A0975FC2C3057EEF67530417AFFE7FB8055C126DC5C6CE94A4B44F330B5D90420"
			"26DC5C6CE94A4B44F330B5D9BBD77CBF958416295CF7E1CE6BCCDC18FF8C07B60441048BD2AEB9CB7E57CB2C4B482FFC"
			"81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F"
			"046997022100A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7020101");

		const QByteArray mPublicKey = QByteArray::fromHex(
			"04"
			"19d4b7447788b0e1993db35500999627e739a4e5e35f02d8fb07d6122e76567f"
			"17758d7a3aa6943ef23e5e2909b3e8b31bfaa4544c2cbf1fb487f31ff239c8f8");

	private Q_SLOTS:
		void null()
		{
			EcKeyPair keyPair;

			QCOMPARE(keyPair.getPublicKey(false), QByteArray());

			QTest::ignoreMessage(QtCriticalMsg, "Unable to apply compression on point: \"\"");
			QCOMPARE(keyPair.getPublicKey(true), QByteArray());

			QTest::ignoreMessage(QtCriticalMsg, "Missing private key");
			QTest::ignoreMessage(QtCriticalMsg, "Unable to apply compression on point: \"\"");
			QCOMPARE(keyPair.getSharedSecret(QByteArray()), QByteArray());

			QTest::ignoreMessage(QtCriticalMsg, "Missing private key");
			QTest::ignoreMessage(QtCriticalMsg, "Unable to apply compression on point: \"\"");
			QCOMPARE(keyPair.getSharedSecret(QByteArray("trash")), QByteArray());

			QTest::ignoreMessage(QtCriticalMsg, "Missing private key");
			QTest::ignoreMessage(QtCriticalMsg, "Unable to apply compression on point: \"\"");
			QCOMPARE(keyPair.getSharedSecret(mPublicKey), QByteArray());
		}


		void mapping()
		{
#if OPENSSL_VERSION_NUMBER < 0x30000000L
			QTest::ignoreMessage(QtCriticalMsg, "Error EC_KEY_generate_key");
#endif
			EcKeyPair keyPairNull(EcdhGenericMapping(nullptr));

			QTest::ignoreMessage(QtDebugMsg, "Create elliptic curve: brainpoolP256r1");
			EcdhGenericMapping mapping(EcUtil::createCurve(NID_brainpoolP256r1));
			QVERIFY(!mapping.generateLocalMappingData().isEmpty());
			QVERIFY(mapping.generateEphemeralDomainParameters(mPublicKey, QByteArray("0123456789ABCDEF")));

			EcKeyPair keyPair(mapping);
			QVERIFY(!keyPair.getPublicKey(false).isEmpty());
			QVERIFY(!keyPair.getPublicKey(true).isEmpty());

			QTest::ignoreMessage(QtCriticalMsg, "Cannot decode elliptic curve point");
			QTest::ignoreMessage(QtCriticalMsg, "Interpreting the EC point failed");
			QTest::ignoreMessage(QtCriticalMsg, "Unable to apply compression on point: \"\"");
			QCOMPARE(keyPair.getSharedSecret(QByteArray()), QByteArray());

			QTest::ignoreMessage(QtCriticalMsg, "Cannot decode elliptic curve point");
			QTest::ignoreMessage(QtCriticalMsg, "Interpreting the EC point failed");
			QTest::ignoreMessage(QtCriticalMsg, "Unable to apply compression on point: \"\"");
			QCOMPARE(keyPair.getSharedSecret(QByteArray("trash")), QByteArray());

			QVERIFY(!keyPair.getSharedSecret(mPublicKey).isEmpty());
		}


		void simple()
		{
			EcKeyPair keyPairNull((QByteArray()));

			QTest::ignoreMessage(QtCriticalMsg, QRegularExpression("Interpreting private key failed: \"error:"_L1));
			EcKeyPair keyPairTrash((QByteArray("trash")));

			EcKeyPair keyPair(mPrivateKey);
			QCOMPARE(keyPair.getPublicKey(false), mPublicKey);
			QCOMPARE(keyPair.getPublicKey(true), EcUtil::compressPoint(mPublicKey));

			QTest::ignoreMessage(QtCriticalMsg, "Cannot decode elliptic curve point");
			QTest::ignoreMessage(QtCriticalMsg, "Interpreting the EC point failed");
			QTest::ignoreMessage(QtCriticalMsg, "Unable to apply compression on point: \"\"");
			QCOMPARE(keyPair.getSharedSecret(QByteArray()), QByteArray());

			QTest::ignoreMessage(QtCriticalMsg, "Cannot decode elliptic curve point");
			QTest::ignoreMessage(QtCriticalMsg, "Interpreting the EC point failed");
			QTest::ignoreMessage(QtCriticalMsg, "Unable to apply compression on point: \"\"");
			QCOMPARE(keyPair.getSharedSecret(QByteArray("trash")), QByteArray());

			QCOMPARE(keyPair.getSharedSecret(mPublicKey), QByteArray::fromHex("228b11e64a8e8357caeb49b209b8f16426054f2906e8a45b0e4cffd996699a4b"));
		}


		void explicitCurve()
		{
			QByteArray privateKey = mPrivateKey;
			// Change generator by double it to ensure the point is still on the curve
			privateKey.replace(
					QByteArray::fromHex("8bd2aeb9cb7e57cb2c4b482ffc81b7afb9de27e1e3bd23c23a4453bd9ace3262547ef835c3dac4fd97f8461a14611dc9c27745132ded8e545c1d54c72f046997"),
					QByteArray::fromHex("743cf1b8b5cd4f2eb55f8aa369593ac436ef044166699e37d51a14c2ce13ea0e36ed163337deba9c946fe0bb776529da38df059f69249406892ada097eeb7cd4"));

#if OPENSSL_VERSION_NUMBER >= 0x40000000L
			QTest::ignoreMessage(QtCriticalMsg, QRegularExpression("Interpreting private key failed: \"error:"_L1));
			EcKeyPair keyPair(privateKey);
#else
			EcKeyPair keyPair(privateKey);
	#if OPENSSL_VERSION_NUMBER >= 0x30000000L
			if (keyPair.mPrivateKey)
			{
				QTest::ignoreMessage(QtCriticalMsg, "Missing group name");
				QTest::ignoreMessage(QtCriticalMsg, "Unable to apply compression on point: \"\"");
				QCOMPARE(keyPair.getSharedSecret(mPublicKey), QByteArray());
			}
	#else
			QCOMPARE(keyPair.getSharedSecret(mPublicKey), QByteArray::fromHex("228b11e64a8e8357caeb49b209b8f16426054f2906e8a45b0e4cffd996699a4b"));
	#endif
#endif
		}


};

QTEST_GUILESS_MAIN(test_EcKeyPair)
#include "test_EcKeyPair.moc"
