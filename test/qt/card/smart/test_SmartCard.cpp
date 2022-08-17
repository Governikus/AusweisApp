/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include <QtTest>

#include "SmartCard.h"


using namespace governikus;


class test_SmartCard
	: public QObject
{
	Q_OBJECT

	private:
		SmartCard mCard;

	private Q_SLOTS:
		void connecting()
		{
			QCOMPARE(mCard.isConnected(), false);

			QCOMPARE(mCard.establishConnection(), CardReturnCode::OK);
			QCOMPARE(mCard.isConnected(), true);

			QCOMPARE(mCard.establishConnection(), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(mCard.isConnected(), true);

			QCOMPARE(mCard.releaseConnection(), CardReturnCode::OK);
			QCOMPARE(mCard.isConnected(), false);

			QCOMPARE(mCard.releaseConnection(), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(mCard.isConnected(), false);
		}


};

QTEST_GUILESS_MAIN(test_SmartCard)
#include "test_SmartCard.moc"
