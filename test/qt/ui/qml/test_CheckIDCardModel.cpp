/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

#include "CheckIDCardModel.h"

#include "ReaderInfo.h"
#include "ReaderManager.h"

#include <QtTest>

using namespace governikus;

class MockReaderManager
	: public ReaderManager
{
	Q_OBJECT

	private:
		QList<ReaderInfo> mReaderInfos;

	public:
		void setReaderInfos(const QList<ReaderInfo>& pReaderInfos)
		{
			mReaderInfos = pReaderInfos;
			for (const auto& reader : std::as_const(mReaderInfos))
			{
				Q_EMIT fireReaderAdded(reader);
			}
		}


		QList<ReaderInfo> getReaderInfos(const ReaderFilter& pFilter = ReaderFilter()) const override
		{
			Q_UNUSED(pFilter)
			return mReaderInfos;
		}


};


class test_CheckIDCardModel
	: public QObject
{
	Q_OBJECT

	static constexpr auto pcsc = ReaderManagerPluginType::PCSC;
	MockReaderManager mMockReaderManager;
	QList<ReaderInfo> mUnknownCard = {
		ReaderInfo {QStringLiteral("TestReader"), pcsc, CardInfo {CardType::UNKNOWN}
		}
	};
	QList<ReaderInfo> mEidCard = {
		ReaderInfo {QStringLiteral("TestReader"), pcsc, CardInfo {CardType::EID_CARD}
		}
	};
	QList<ReaderInfo> mEidAndUnknownCard = {
		ReaderInfo {QStringLiteral("TestReaderA"), pcsc, CardInfo {CardType::EID_CARD}
		},
		ReaderInfo {QStringLiteral("TestReaderB"), pcsc, CardInfo {CardType::UNKNOWN}
		}
	};
	QList<ReaderInfo> mUnknownAndEidCard = {
		ReaderInfo {QStringLiteral("TestReaderA"), pcsc, CardInfo {CardType::UNKNOWN}
		},
		ReaderInfo {QStringLiteral("TestReaderB"), pcsc, CardInfo {CardType::EID_CARD}
		}
	};

	private Q_SLOTS:
		void initTestCase()
		{
			Env::set(ReaderManager::staticMetaObject, &mMockReaderManager);
		}


		void cleanup()
		{
			mMockReaderManager.setReaderInfos({});
		}


		void cleanupTestCase()
		{
			Env::clear();
		}


		void test_onCardInserted()
		{
			CheckIDCardModel model;
			QSignalSpy spyResultChanged(&model, &CheckIDCardModel::fireResultChanged);
			QSignalSpy spyScanCompleted(&model, &CheckIDCardModel::fireScanCompleted);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::UNKNOWN);

			ReaderInfo readerInfo(QStringLiteral("TestReader"));
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 0);
			QCOMPARE(spyScanCompleted.count(), 0);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::UNKNOWN);

			QTest::ignoreMessage(QtWarningMsg, "Detected conflicting readers with cards");
			const ReaderInfo wrongReaderInfo(QStringLiteral("OtherName"));
			model.onCardInserted(wrongReaderInfo);
			QCOMPARE(spyResultChanged.count(), 0);
			QCOMPARE(spyScanCompleted.count(), 0);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::UNKNOWN);

			readerInfo.setCardInfo(CardInfo(CardType::UNKNOWN));
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 1);
			QCOMPARE(spyScanCompleted.count(), 1);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::UNKNOWN_CARD_DETECTED);

			readerInfo.setCardInfo(CardInfo(CardType::EID_CARD));
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 2);
			QCOMPARE(spyScanCompleted.count(), 2);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::SUCCESS);

			readerInfo.setCardInfo(CardInfo(CardType::EID_CARD, FileRef::appEId(), nullptr, 0));
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 3);
			QCOMPARE(spyScanCompleted.count(), 3);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::PIN_BLOCKED);

			readerInfo.setCardInfo(CardInfo(CardType::EID_CARD, FileRef::appEId(), nullptr, 1));
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 4);
			QCOMPARE(spyScanCompleted.count(), 4);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::PIN_SUSPENDED);

			readerInfo.setCardInfo(CardInfo(CardType::EID_CARD, FileRef::appEId(), nullptr, 3, true));
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 5);
			QCOMPARE(spyScanCompleted.count(), 5);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::PIN_DEACTIVATED);

			readerInfo.setMaxApduLength(20);
			model.onCardInserted(readerInfo);
			QCOMPARE(spyResultChanged.count(), 6);
			QCOMPARE(spyScanCompleted.count(), 6);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::INSUFFICIENT_APDU_LENGTH);
		}


		void test_onCardRemoved_data()
		{
			QTest::addColumn<ReaderInfo>("addCard");
			QTest::addColumn<ReaderInfo>("removeCard");
			QTest::addColumn<int>("expectedSignalCount");
			QTest::addColumn<CheckIDCardModel::Result>("result");

			QTest::newRow("Add card A remove A") << ReaderInfo {QStringLiteral("A")} << ReaderInfo {QStringLiteral("A")} << 1 << CheckIDCardModel::Result::CARD_ACCESS_FAILED;
			QTest::newRow("Add card A remove B") << ReaderInfo {QStringLiteral("A")} << ReaderInfo {QStringLiteral("B")} << 0 << CheckIDCardModel::Result::UNKNOWN;
		}


		void test_onCardRemoved()
		{
			QFETCH(ReaderInfo, addCard);
			QFETCH(ReaderInfo, removeCard);
			QFETCH(int, expectedSignalCount);
			QFETCH(CheckIDCardModel::Result, result);

			CheckIDCardModel model;
			QSignalSpy spyResultChanged(&model, &CheckIDCardModel::fireResultChanged);
			model.onCardInserted(addCard);
			QCOMPARE(spyResultChanged.count(), 0);
			model.onCardRemoved(removeCard);
			QCOMPARE(spyResultChanged.count(), expectedSignalCount);
			QCOMPARE(model.getResult(), result);
		}


		void test_onReaderAdded_data()
		{
			QTest::addColumn<ReaderInfo>("readerInfo");
			QTest::addColumn<CheckIDCardModel::Result>("result");

			QTest::newRow("No Card") << ReaderInfo {QStringLiteral("TestReader")} << CheckIDCardModel::Result::UNKNOWN;
			QTest::newRow("Unknown Card") << ReaderInfo {QStringLiteral("TestReader"), pcsc, CardInfo {CardType::UNKNOWN}
				} << CheckIDCardModel::Result::UNKNOWN;
			QTest::newRow("eID Card") << ReaderInfo {QStringLiteral("TestReader"), pcsc, CardInfo {CardType::EID_CARD}
				} << CheckIDCardModel::Result::SUCCESS;
		}


		void test_onReaderAdded()
		{
			QFETCH(ReaderInfo, readerInfo);
			QFETCH(CheckIDCardModel::Result, result);

			CheckIDCardModel model;
			QSignalSpy spyResultChanged(&model, &CheckIDCardModel::fireResultChanged);
			model.onReaderAdded(readerInfo);
			const bool expectSuccess = result == CheckIDCardModel::Result::SUCCESS;
			QCOMPARE(spyResultChanged.count(), expectSuccess ? 1 : 0);
			QCOMPARE(model.getResult(), result);
		}


		void test_onReaderRemoved_data()
		{
			QTest::addColumn<ReaderInfo>("addReader");
			QTest::addColumn<ReaderInfo>("removeReader");
			QTest::addColumn<int>("expectedSignalCount");
			QTest::addColumn<CheckIDCardModel::Result>("result");

			QTest::newRow("Add reader A remove A") << ReaderInfo {QStringLiteral("A")} << ReaderInfo {QStringLiteral("A")} << 1 << CheckIDCardModel::Result::CARD_ACCESS_FAILED;
			QTest::newRow("Add reader A remove B") << ReaderInfo {QStringLiteral("A")} << ReaderInfo {QStringLiteral("B")} << 0 << CheckIDCardModel::Result::UNKNOWN;
		}


		void test_onReaderRemoved()
		{
			QFETCH(ReaderInfo, addReader);
			QFETCH(ReaderInfo, removeReader);
			QFETCH(int, expectedSignalCount);
			QFETCH(CheckIDCardModel::Result, result);

			CheckIDCardModel model;
			QSignalSpy spyResultChanged(&model, &CheckIDCardModel::fireResultChanged);
			model.onCardInserted(addReader);
			QCOMPARE(spyResultChanged.count(), 0);
			model.onReaderRemoved(removeReader);
			QCOMPARE(spyResultChanged.count(), expectedSignalCount);
			QCOMPARE(model.getResult(), result);
		}


		void test_onReaderPropertiesUpdated()
		{
			auto readerNoCard = ReaderInfo {QStringLiteral("TestReader")};
			readerNoCard.setMaxApduLength(1);
			CheckIDCardModel model;
			QSignalSpy spyResultChanged(&model, &CheckIDCardModel::fireResultChanged);
			model.onReaderPropertiesUpdated(readerNoCard);
			QCOMPARE(spyResultChanged.count(), 1);
			QCOMPARE(model.getResult(), CheckIDCardModel::Result::INSUFFICIENT_APDU_LENGTH);
		}


		void test_onReaderConnectionWaitEnded_data()
		{
			QTest::addColumn<QList<ReaderInfo>>("readerInfos");
			QTest::addColumn<CheckIDCardModel::Result>("result");

			QTest::newRow("Unknown Card") << mUnknownCard << CheckIDCardModel::Result::UNKNOWN_CARD_DETECTED;
			QTest::newRow("eID Card") << mEidCard << CheckIDCardModel::Result::SUCCESS;
			QTest::newRow("eID Card and Unknown Card") << mEidAndUnknownCard << CheckIDCardModel::Result::SUCCESS;
			QTest::newRow("Unknown Card and eID Card") << mUnknownAndEidCard << CheckIDCardModel::Result::UNKNOWN_CARD_DETECTED;
		}


		void test_onReaderConnectionWaitEnded()
		{
			QFETCH(QList<ReaderInfo>, readerInfos);
			QFETCH(CheckIDCardModel::Result, result);

			CheckIDCardModel model;
			QSignalSpy spy(&model, &CheckIDCardModel::fireResultChanged);
			mMockReaderManager.setReaderInfos(readerInfos);
			model.onReaderConnectionWaitEnded();
			QTRY_COMPARE(spy.count(), 1);
			QCOMPARE(model.getResult(), result);
		}


		void test_onReaderConnectionWaitEndedWithScan_data()
		{
			QTest::addColumn<bool>("addReaderAfterStart");
			QTest::addColumn<QList<ReaderInfo>>("readerInfos");
			QTest::addColumn<int>("expectedTimeoutCount");
			QTest::addColumn<CheckIDCardModel::Result>("result");

			QTest::newRow("Unknown Card After Start") << true << mUnknownCard << 1 << CheckIDCardModel::Result::UNKNOWN_CARD_DETECTED;
			QTest::newRow("Unknown Card Before Start") << false << mUnknownCard << 1 << CheckIDCardModel::Result::UNKNOWN_CARD_DETECTED;
			QTest::newRow("eID Card After Start") << true << mEidCard << 0 << CheckIDCardModel::Result::SUCCESS;
			QTest::newRow("eID Card Before Start") << false << mEidCard << 0 << CheckIDCardModel::Result::SUCCESS;
			QTest::newRow("eID and Unknown Card After Start") << true << mEidAndUnknownCard << 0 << CheckIDCardModel::Result::SUCCESS;
			QTest::newRow("eID and Unknown Card Before Start") << false << mEidAndUnknownCard << 0 << CheckIDCardModel::Result::SUCCESS;
			QTest::newRow("Unknown and eID CardAfterStart") << true << mUnknownAndEidCard << 0 << CheckIDCardModel::Result::SUCCESS;
			QTest::newRow("Unknown and eID CardBeforeStart") << false << mUnknownAndEidCard << 0 << CheckIDCardModel::Result::SUCCESS;
		}


		void test_onReaderConnectionWaitEndedWithScan()
		{
			QFETCH(bool, addReaderAfterStart);
			QFETCH(QList<ReaderInfo>, readerInfos);
			QFETCH(int, expectedTimeoutCount);
			QFETCH(CheckIDCardModel::Result, result);

			const int connectionTimeout = 100;
			CheckIDCardModel model(nullptr, connectionTimeout);
			QSignalSpy spyTimeout(&model.mReaderConnectionWait, &QTimer::timeout);

			if (addReaderAfterStart)
			{
				model.startScan(pcsc);
				mMockReaderManager.setReaderInfos(readerInfos);
			}
			else
			{
				mMockReaderManager.setReaderInfos(readerInfos);
				model.startScan(pcsc);
			}

			if (result == CheckIDCardModel::Result::SUCCESS)
			{
				//We expect the timer to be stopped by any present Eid.
				QVERIFY(!model.mReaderConnectionWait.isActive());
			}

			QTRY_COMPARE_EQ_WITH_TIMEOUT(spyTimeout.count(), expectedTimeoutCount, connectionTimeout * 2);
			QCOMPARE(model.getResult(), result);
			QVERIFY(!model.mIsRunning);
		}


};

QTEST_MAIN(test_CheckIDCardModel)
#include "test_CheckIDCardModel.moc"
