/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

#include "CardPositionModel.h"

#include <QScopedPointer>
#include <QtTest>

using namespace governikus;

bool operator==(const CardPosition& pLeft, const CardPosition& pRight)
{
	return pLeft.mXPosition == pRight.mXPosition
		   && pLeft.mYPosition == pRight.mYPosition
		   && pLeft.mZPosition == pRight.mZPosition
		   && pLeft.mRotation == pRight.mRotation;
}


class test_CardPositionModel
	: public QObject
{
	Q_OBJECT
	QScopedPointer<CardPositionModel> mModel;

	private Q_SLOTS:
		void init()
		{
			mModel.reset(new CardPositionModel());
		}


		void test_CyclingClock()
		{
			QCOMPARE(mModel->getCyclingClock(), 5000);
			QSignalSpy spyCyclingClockChanged(mModel.data(), &CardPositionModel::fireCyclingClockChanged);
			const bool wasPreviouslyRunning = mModel->getIsRunning();
			mModel->setCyclingClock(1000);
			QCOMPARE(spyCyclingClockChanged.count(), 1);
			QCOMPARE(mModel->getCyclingClock(), 1000);
			QCOMPARE(mModel->getIsRunning(), wasPreviouslyRunning);
		}


		void test_Running()
		{
			QCOMPARE(mModel->getIsRunning(), false);
			QSignalSpy spyIsRunningChanged(mModel.data(), &CardPositionModel::fireIsRunningChanged);
			QSignalSpy spyCardPositionChanged(mModel.data(), &CardPositionModel::fireCardPositionChanged);
			mModel->setIsRunning(true);
			QCOMPARE(spyCardPositionChanged.count(), 1);
			QCOMPARE(spyIsRunningChanged.count(), 1);
			QCOMPARE(mModel->getIsRunning(), true);
			mModel->setIsRunning(true);
			QCOMPARE(spyIsRunningChanged.count(), 1);
			mModel->setIsRunning(false);
			QCOMPARE(spyIsRunningChanged.count(), 2);
			QCOMPARE(spyCardPositionChanged.count(), 1);
		}


		void test_CardPositionCount()
		{
#if defined(Q_OS_IOS)
			QCOMPARE(mModel->getCardPositionCount(), 1);
#else
			QCOMPARE(mModel->getCardPositionCount(), 4);
#endif
		}


		void test_InitialCardPosition()
		{
			QCOMPARE(mModel->getIsRunning(), false);
			QScopedPointer<CardPosition> cardPosition(qvariant_cast<CardPosition*>(mModel->getCardPosition()));
#if defined(Q_OS_IOS)
			QVERIFY(CardPosition(0.5, 0.0, -1) == *cardPosition);
#else
			QVERIFY(CardPosition(0.5, 0.5, -1, 90) == *cardPosition);
#endif
		}


		void test_CardPositionRotation()
		{
#if defined(Q_OS_IOS)
			QSKIP("Model has only one CardPosition on iOS, don't run test.")
#endif
			QSignalSpy spyCardPositionChanged(mModel.data(), &CardPositionModel::fireCardPositionChanged);
			mModel->setIsRunning(true);
			QCOMPARE(mModel->getIsRunning(), true);
			QCOMPARE(spyCardPositionChanged.count(), 1);
			QScopedPointer<CardPosition> cardPosition(qvariant_cast<CardPosition*>(mModel->getCardPosition()));
			QVERIFY(CardPosition(0.5, 0.5, -1, 90) == *cardPosition);

			mModel->onTimerTimeout();
			QCOMPARE(spyCardPositionChanged.count(), 2);
			cardPosition.reset(qvariant_cast<CardPosition*>(mModel->getCardPosition()));
			QVERIFY(CardPosition(0.5, 0.5, -1) == *cardPosition);

			mModel->onTimerTimeout();
			QCOMPARE(spyCardPositionChanged.count(), 3);
			cardPosition.reset(qvariant_cast<CardPosition*>(mModel->getCardPosition()));
			QVERIFY(CardPosition(0.5, 0.25, -1) == *cardPosition);

			mModel->onTimerTimeout();
			QCOMPARE(spyCardPositionChanged.count(), 4);
			cardPosition.reset(qvariant_cast<CardPosition*>(mModel->getCardPosition()));
			QVERIFY(CardPosition(0.5, 0.75, -1) == *cardPosition);

			mModel->onTimerTimeout();
			QCOMPARE(spyCardPositionChanged.count(), 5);
			cardPosition.reset(qvariant_cast<CardPosition*>(mModel->getCardPosition()));
			QVERIFY(CardPosition(0.5, 0.5, -1, 90) == *cardPosition);

			mModel->onTimerTimeout();
			QCOMPARE(spyCardPositionChanged.count(), 6);
			cardPosition.reset(qvariant_cast<CardPosition*>(mModel->getCardPosition()));
			QVERIFY(CardPosition(0.5, 0.5, -1) == *cardPosition);

			mModel->setIsRunning(false);
			mModel->setIsRunning(true);
			QCOMPARE(spyCardPositionChanged.count(), 7);
			cardPosition.reset(qvariant_cast<CardPosition*>(mModel->getCardPosition()));
			QVERIFY(CardPosition(0.5, 0.5, -1, 90) == *cardPosition);
		}


};

QTEST_MAIN(test_CardPositionModel)
#include "test_CardPositionModel.moc"
