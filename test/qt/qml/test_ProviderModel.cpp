/*!
 * \brief Unit tests for \ref ProviderModel
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
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
		void createAmountStringForCents()
		{
			QVERIFY(ProviderModel::createAmountString(3.9).contains(QString("3.9")));
		}


		void createAmountStringForEur()
		{
			QVERIFY(ProviderModel::createAmountString(289).contains(QString("2.89")));
		}


		void createCostStringMinute()
		{
			const auto& msg = ProviderModel::createCostString(1.9, 2.9);
			QVERIFY(msg.contains(QString("1.9")));
			QVERIFY(!msg.contains(QString("2.9")));
		}


		void createCostStringCall()
		{
			const auto& msg = ProviderModel::createCostString(0.0, 2.9);
			QVERIFY(!msg.contains(QString("0.0")));
			QVERIFY(msg.contains(QString("2.9")));
		}


		void createCostStringEmpty()
		{
			const auto& msg = ProviderModel::createCostString(0.0, 0.0);
			QVERIFY(msg.isEmpty());
		}


		void createCostStringNullCost()
		{
			const auto& msg = ProviderModel::createCostString(CallCost());
			QVERIFY(msg.isNull());
		}


		void createCostString()
		{
			const auto& msg = ProviderModel::createCostString(CallCost(0.0, 3.9, 0.0, 42.0, 0.0));
			QVERIFY(!msg.isNull());
		}


};

QTEST_GUILESS_MAIN(test_ProviderModel)
#include "test_ProviderModel.moc"
