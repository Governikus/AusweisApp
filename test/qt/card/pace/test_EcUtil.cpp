/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "pace/ec/EcUtil.h"

#include <QSharedPointer>

#include <QtTest>
#include <openssl/ec.h>


using namespace governikus;


class test_EcUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
		}


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
			QVERIFY(EcUtil::generateKey(nullptr).isNull());

			const auto& curve = EcUtil::createCurve(NID_brainpoolP256r1);
			QVERIFY(!EcUtil::generateKey(curve).isNull());
		}


		void getEncodedPublicKey()
		{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
			QVERIFY(EcUtil::getEncodedPublicKey(nullptr).isNull());
#endif
		}


		void getPrivateKey()
		{
#if OPENSSL_VERSION_NUMBER >= 0x30000000L
			QVERIFY(EcUtil::getPrivateKey(nullptr).isNull());
#endif
		}


};

QTEST_GUILESS_MAIN(test_EcUtil)
#include "test_EcUtil.moc"
