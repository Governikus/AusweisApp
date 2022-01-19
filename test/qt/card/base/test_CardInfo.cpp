/*!
 * \brief Tests for \ref CardInfo.
 *
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "CardInfo.h"


#include <QtTest>

using namespace governikus;

class test_CardInfo
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_CardTypeString_data()
		{
			QTest::addColumn<CardType>("type");
			QTest::addColumn<QString>("output");

			QTest::newRow("none") << CardType::NONE << QString("not inserted");
			QTest::newRow("unknown") << CardType::UNKNOWN << QString("unknown type");
			QTest::newRow("eid") << CardType::EID_CARD << QString("ID card (PA/eAT/eID)");
		}


		void test_CardTypeString()
		{
			QFETCH(CardType, type);
			QFETCH(QString, output);

			const CardInfo info(type, QSharedPointer<EFCardAccess>(), 3, false, false);

			QCOMPARE(info.getCardTypeString(), output);
		}


		void test_RetryCounterDeterminated()
		{
			const CardInfo info1(CardType::EID_CARD);
			QVERIFY(!info1.isRetryCounterDetermined());

			const CardInfo info2(CardType::EID_CARD, QSharedPointer<EFCardAccess>(), 3, false, false);
			QVERIFY(info2.isRetryCounterDetermined());
		}


};

QTEST_GUILESS_MAIN(test_CardInfo)
#include "test_CardInfo.moc"
