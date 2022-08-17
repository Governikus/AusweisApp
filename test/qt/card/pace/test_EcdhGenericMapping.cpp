/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "pace/ec/EcdhGenericMapping.h"

#include "pace/ec/EcUtil.h"

#include <QtTest>

using namespace governikus;


class test_EcdhGenericMapping
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void curve_data()
		{
			QTest::addColumn<int>("nid");

			QTest::newRow("with curve") << NID_brainpoolP256r1;
			QTest::newRow("without curve") << NID_undef;
		}


		void curve()
		{
			QFETCH(int, nid);

			auto curve = EcUtil::createCurve(nid);
			EcdhGenericMapping mapping(curve);
			QCOMPARE(mapping.getCurve(), curve);
			QCOMPARE(mapping.generateTerminalMappingData().isEmpty(), curve.isNull());
		}


};

QTEST_GUILESS_MAIN(test_EcdhGenericMapping)
#include "test_EcdhGenericMapping.moc"
