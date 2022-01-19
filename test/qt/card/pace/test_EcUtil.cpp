/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
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
			EcUtil::create(static_cast<EC_GROUP*>(nullptr));
		}


		void createAndFreeCurve()
		{
			EllipticCurveFactory::create(8);
		}


		void createAndFreeEmptyKey()
		{
			EcUtil::create(static_cast<EC_KEY*>(nullptr));
		}


		void createAndFreeKey()
		{
			EcUtil::create(EC_KEY_new());
		}


		void createAndFreeEmptyPoint()
		{
			EcUtil::create(static_cast<EC_POINT*>(nullptr));
		}


		void createAndFreePoint()
		{
			QSharedPointer<EC_GROUP> curve = EllipticCurveFactory::create(8);
			EcUtil::create(EC_POINT_new(curve.data()));
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


};

QTEST_GUILESS_MAIN(test_EcUtil)
#include "test_EcUtil.moc"
