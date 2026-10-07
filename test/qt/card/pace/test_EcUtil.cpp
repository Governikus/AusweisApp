/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "pace/ec/EcUtil.h"

#include "Converter.h"

#include <QSharedPointer>

#include <QtTest>
#include <openssl/ec.h>


using namespace governikus;


class test_EcUtil
	: public QObject
{
	Q_OBJECT

	private:
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
		QSharedPointer<EVP_PKEY> readPrivateKey()
#else
		QSharedPointer<EC_KEY> readPrivateKey()
#endif
		{
			const QByteArray rawKey = QByteArray::fromHex(
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

			const auto* dataPointer = reinterpret_cast<const unsigned char*>(rawKey.constData());
			const auto& privateKey = EcUtil::create(d2i_PrivateKey(EVP_PKEY_EC, nullptr, &dataPointer, static_cast<long>(rawKey.length())));

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
			return privateKey;

#else
			return EcUtil::create(EVP_PKEY_get1_EC_KEY(privateKey.data()));

#endif
		}

	private Q_SLOTS:
		void compressPoint_data()
		{
			QTest::addColumn<QByteArray>("input");
			QTest::addColumn<QByteArray>("output");
			QTest::addColumn<bool>("warning");

			QTest::newRow("Empty") << QByteArray() << QByteArray() << true;
			QTest::newRow("Wrong first byte - no data") << QByteArray("00") << QByteArray("00") << true;
			QTest::newRow("Correct first byte - no data") << QByteArray("04") << QByteArray() << false;
			QTest::newRow("Wrong first byte - data") << QByteArray("000102") << QByteArray("000102") << true;
			QTest::newRow("Correct first byte - data") << QByteArray("040102") << QByteArray("01") << false;
		}


		void compressPoint()
		{
			QFETCH(QByteArray, input);
			QFETCH(QByteArray, output);
			QFETCH(bool, warning);

			if (warning)
			{
				QByteArray message("Unable to apply compression on point: \"DATA\" ");
				message.replace("DATA", input);
				QTest::ignoreMessage(QtCriticalMsg, message.data());
			}
			QCOMPARE(EcUtil::compressPoint(QByteArray::fromHex(input)).toHex(), output);
		}


		void createAndFreeEmptyCurve()
		{
			EcUtil::create(static_cast<EC_GROUP*>(nullptr));
		}


		void createAndFreeCurve()
		{
			EcUtil::create(EC_GROUP_new_by_curve_name(NID_brainpoolP256r1));
		}


		void createAndFreeEmptyKey()
		{
#if OPENSSL_VERSION_NUMBER < 0x30000000L
			EcUtil::create(static_cast<EC_KEY*>(nullptr));
#endif
		}


		void createAndFreeKey()
		{
#if OPENSSL_VERSION_NUMBER < 0x30000000L
			EcUtil::create(EC_KEY_new());
#endif
		}


		void createAndFreeEmptyPoint()
		{
			EcUtil::create(static_cast<EC_POINT*>(nullptr));
		}


		void createAndFreePoint()
		{
			EcUtil::create(EC_POINT_new(nullptr));
		}


		void createAndFreeEmptyBigNumber()
		{
			EcUtil::create(static_cast<BIGNUM*>(nullptr));
		}


		void createAndFreeBigNumber()
		{
			BIGNUM* bigNum = BN_new();
			BN_bin2bn(reinterpret_cast<const uchar*>("k37"), 3, bigNum);
			EcUtil::create(bigNum);
		}


		void conversion()
		{
			auto curve = EcUtil::createCurve(NID_brainpoolP256r1);

			QVERIFY(EcUtil::oct2point(curve, QByteArray("dummy")).isNull());
			QVERIFY(EcUtil::oct2point(nullptr, QByteArray("dummy")).isNull());

			const auto x = QByteArray::fromHex("8BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1E3BD23C23A4453BD9ACE3262");
			const auto y = QByteArray::fromHex("547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D54C72F046997");
			const auto point = EcUtil::oct2point(curve, QByteArray::fromHex("04") + x + y);
			QVERIFY(!point.isNull());

			const auto uncompressed = EcUtil::point2oct(curve, point.data());
			QCOMPARE(uncompressed, QByteArray::fromHex("04") + x + y);

			const auto compressed = EcUtil::point2oct(curve, point.data(), true);
			QCOMPARE(compressed, x);
		}


		void generateKey()
		{
			QVERIFY(EcUtil::generateKey(NID_undef).isNull());

			QVERIFY(!EcUtil::generateKey(NID_brainpoolP256r1).isNull());
		}


		void getEncodedPublicKey()
		{
			QVERIFY(EcUtil::getEncodedPublicKey(nullptr, true).isNull());
			QVERIFY(EcUtil::getEncodedPublicKey(nullptr, false).isNull());

			const auto& key = readPrivateKey();
			QCOMPARE(EcUtil::getEncodedPublicKey(key, true), QByteArray::fromHex("19D4B7447788B0E1993DB35500999627E739A4E5E35F02D8FB07D6122E76567F"));
			QCOMPARE(EcUtil::getEncodedPublicKey(key, false), QByteArray::fromHex("0419D4B7447788B0E1993DB35500999627E739A4E5E35F02D8FB07D6122E76567F17758D7A3AA6943EF23E5E2909B3E8B31BFAA4544C2CBF1FB487F31FF239C8F8"));
		}


		void getPrivateKey()
		{
			QVERIFY(EcUtil::getPrivateKey(nullptr).isNull());

			const auto& key = readPrivateKey();
			QCOMPARE(Converter::toHex(EcUtil::getPrivateKey(key).data()), QByteArray("A07EB62E891DAA84643E0AFCC1AF006891B669B8F51E379477DBEAB8C987A610"));
		}


};

QTEST_GUILESS_MAIN(test_EcUtil)
#include "test_EcUtil.moc"
