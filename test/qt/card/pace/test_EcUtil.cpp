/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "pace/ec/EcUtil.h"
#include "pace/ec/EllipticCurveFactory.h"

#include <openssl/ec.h>
#include <QSharedPointer>
#include <QtTest>

using namespace governikus;

class test_EcUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
		}


		void createAndFreeEmptyCurve()
		{
			QSharedPointer<EC_GROUP> curve = EcUtil::create(static_cast<EC_GROUP*>(nullptr));
			curve.clear();
		}


		void createAndFreeCurve()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(8);
			curve.clear();
		}


		void createAndFreeEmptyKey()
		{
			QSharedPointer<EC_KEY> key = EcUtil::create(static_cast<EC_KEY*>(nullptr));
			key.clear();
		}


		void createAndFreeKey()
		{
			QSharedPointer<EC_KEY> key = EcUtil::create(EC_KEY_new());
			key.clear();
		}


		void createAndFreeEmptyPoint()
		{
			QSharedPointer<EC_POINT> key = EcUtil::create(static_cast<EC_POINT*>(nullptr));
			key.clear();
		}


		void createAndFreePoint()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(8);
			QSharedPointer<EC_POINT> key = EcUtil::create(EC_POINT_new(curve.data()));
			key.clear();
		}


		void createAndFreeEmptyBigNumber()
		{
			QSharedPointer<BIGNUM> key = EcUtil::create(static_cast<BIGNUM*>(nullptr));
			key.clear();
		}


		void createAndFreeBigNumber()
		{
			BIGNUM* bigNum = BN_new();
			BN_bin2bn(reinterpret_cast<const uchar*>("k37"), 3, bigNum);
			QSharedPointer<BIGNUM> key = EcUtil::create(bigNum);
			key.clear();
		}


};

QTEST_GUILESS_MAIN(test_EcUtil)
#include "test_EcUtil.moc"
