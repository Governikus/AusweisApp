/*!
 * \brief Tests for \ref ReaderInfo.
 *
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderInfo.h"


#include <QtTest>

using namespace governikus;

struct Dummy
{
	int mCount;
	QString mData;
};
Q_DECLARE_METATYPE(Dummy)

class test_ReaderInfo
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_CardType_data()
		{
			QTest::addColumn<CardType>("type");
			QTest::addColumn<bool>("hasCard");
			QTest::addColumn<bool>("hasEid");
			QTest::addColumn<bool>("physicalCard");

			QTest::newRow("none") << CardType::NONE << false << false << false;
			QTest::newRow("unknown") << CardType::UNKNOWN << true << false << false;
			QTest::newRow("eid-card") << CardType::EID_CARD << true << true << true;
			QTest::newRow("smart-eid") << CardType::SMART_EID << true << true << false;
		}


		void test_CardType()
		{
			QFETCH(CardType, type);
			QFETCH(bool, hasCard);
			QFETCH(bool, hasEid);
			QFETCH(bool, physicalCard);

			const ReaderInfo info(QStringLiteral("Reader"), ReaderManagerPlugInType::UNKNOWN, CardInfo(type, nullptr, 3, false, false));

			QCOMPARE(info.getCardType(), type);
			QCOMPARE(info.hasCard(), hasCard);
			QCOMPARE(info.hasEid(), hasEid);
			QCOMPARE(info.isPhysicalCard(), physicalCard);
		}


		void test_ApduLength_data()
		{
			QTest::addColumn<int>("length");
			QTest::addColumn<bool>("insufficient");

			QTest::newRow("-10") << -10 << false;
			QTest::newRow("-2") << -2 << false;
			QTest::newRow("-1") << -1 << false;
			QTest::newRow("0") << 0 << true;
			QTest::newRow("1") << 2 << true;
			QTest::newRow("10") << 10 << true;
			QTest::newRow("499") << 499 << true;
			QTest::newRow("500") << 500 << false;
			QTest::newRow("501") << 501 << false;
		}


		void test_ApduLength()
		{
			QFETCH(int, length);
			QFETCH(bool, insufficient);

			ReaderInfo info(QStringLiteral("Reader"), ReaderManagerPlugInType::UNKNOWN, CardInfo(CardType::UNKNOWN));
			QCOMPARE(info.getMaxApduLength(), 500);
			QVERIFY(!info.insufficientApduLength());

			info.setMaxApduLength(length);
			QCOMPARE(info.getMaxApduLength(), length);
			QCOMPARE(info.insufficientApduLength(), insufficient);
		}


};

QTEST_GUILESS_MAIN(test_ReaderInfo)
#include "test_ReaderInfo.moc"
