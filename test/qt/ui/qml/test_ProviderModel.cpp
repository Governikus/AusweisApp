/*!
 * \brief Unit tests for \ref ProviderModel
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderModel.h"

#include <QDebug>
#include <QtTest>


using namespace governikus;


class test_ProviderModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void createCostStringNullCost()
		{
			const auto& msg = ProviderModel::createCostString(CallCost());
			QVERIFY(msg.isNull());
		}


		void createCostString_data()
		{
			QTest::addColumn<int>("freeSeconds");
			QTest::addColumn<double>("lLineCentsPMin");
			QTest::addColumn<double>("lLineCentsPCall");
			QTest::addColumn<double>("mobCentsPMin");
			QTest::addColumn<double>("mobCentsPCall");
			QTest::addColumn<QString>("msg");

			const QString msg1 = QStringLiteral("10 seconds free, afterwards landline costs 0.5 ct/min; mobile costs may vary.");
			const QString msg2 = QStringLiteral("landline costs 20 ct/call; mobile costs may vary.");
			const QString msg3 = QStringLiteral("landline costs 15 ct/min; mobile costs 25 ct/min");
			const QString msg4 = QStringLiteral("20 seconds free, afterwards landline costs 15 ct/min; mobile costs 1.01 EUR/call");
			const QString msg5 = QStringLiteral("60 seconds free, afterwards landline costs 2 EUR/call; mobile costs 55 ct/call");

			QTest::newRow("landlineProMin") << 10 << 0.5 << 2.4 << 0.0 << 0.0 << msg1;
			QTest::newRow("landlineProCall") << 0 << 0.0 << 20.0 << 0.0 << 0.0 << msg2;
			QTest::newRow("landlineAndMobProMin") << 0 << 15.0 << 30.0 << 25.0 << 0.0 << msg3;
			QTest::newRow("landlineProMinMobProCall") << 20 << 15.0 << 30.0 << 0.0 << 101.0 << msg4;
			QTest::newRow("landlineAndMobProCall") << 60 << 0.0 << 200.0 << 0.0 << 55.0 << msg5;
		}


		void createCostString()
		{
			QFETCH(int, freeSeconds);
			QFETCH(double, lLineCentsPMin);
			QFETCH(double, lLineCentsPCall);
			QFETCH(double, mobCentsPMin);
			QFETCH(double, mobCentsPCall);
			QFETCH(QString, msg);

			const CallCost cost(freeSeconds, lLineCentsPMin, lLineCentsPCall, mobCentsPMin, mobCentsPCall);
			QCOMPARE(ProviderModel::createCostString(cost), msg);
		}


};

QTEST_GUILESS_MAIN(test_ProviderModel)
#include "test_ProviderModel.moc"
