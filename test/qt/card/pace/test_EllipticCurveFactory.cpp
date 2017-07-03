/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */


#include <QApplication>
#include <QSignalSpy>
#include <QtCore/QtCore>
#include <QtTest/QtTest>
#include <openssl/obj_mac.h>

#include "pace/ec/EllipticCurveFactory.h"


using namespace governikus;


class test_EllipticCurveFactory
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
		}


		void create_secp192r1()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(8);

			Q_ASSERT(curve != nullptr);
			QCOMPARE(EC_GROUP_get_curve_name(curve.data()), NID_X9_62_prime192v1);
		}


		void create_brainpoolP192r11()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(9);

			Q_ASSERT(curve != nullptr);
			QCOMPARE(EC_GROUP_get_curve_name(curve.data()), NID_brainpoolP192r1);
		}


		void create_secp224r1()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(10);

			Q_ASSERT(curve != nullptr);
			QCOMPARE(EC_GROUP_get_curve_name(curve.data()), NID_secp224r1);
		}


		void create_brainpoolP224r1()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(11);

			Q_ASSERT(curve != nullptr);
			QCOMPARE(EC_GROUP_get_curve_name(curve.data()), NID_brainpoolP224r1);
		}


		void create_X9_62_prime256v1()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(12);

			Q_ASSERT(curve != nullptr);
			QCOMPARE(EC_GROUP_get_curve_name(curve.data()), NID_X9_62_prime256v1);
		}


		void create_brainpoolP256r1()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(13);

			Q_ASSERT(curve != nullptr);
			QCOMPARE(EC_GROUP_get_curve_name(curve.data()), NID_brainpoolP256r1);
		}


		void create_brainpoolP320r1()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(14);

			Q_ASSERT(curve != nullptr);
			QCOMPARE(EC_GROUP_get_curve_name(curve.data()), NID_brainpoolP320r1);
		}


		void create_secp384r1()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(15);

			Q_ASSERT(curve != nullptr);
			QCOMPARE(EC_GROUP_get_curve_name(curve.data()), NID_secp384r1);
		}


		void create_brainpoolP384r1()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(16);

			Q_ASSERT(curve != nullptr);
			QCOMPARE(EC_GROUP_get_curve_name(curve.data()), NID_brainpoolP384r1);
		}


		void create_brainpoolP512r1()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(17);

			Q_ASSERT(curve != nullptr);
			QCOMPARE(EC_GROUP_get_curve_name(curve.data()), NID_brainpoolP512r1);
		}


		void create_secp521r1()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(18);

			Q_ASSERT(curve != nullptr);
			QCOMPARE(EC_GROUP_get_curve_name(curve.data()), NID_secp521r1);
		}


		void create_onknown()
		{
			Q_ASSERT(nullptr == EllipticCurveFactory::create(19));
			Q_ASSERT(nullptr == EllipticCurveFactory::create(7));
		}


};

QTEST_GUILESS_MAIN(test_EllipticCurveFactory)
#include "test_EllipticCurveFactory.moc"
