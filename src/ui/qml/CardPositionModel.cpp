/*!
 * \copyright Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */

#include "CardPositionModel.h"

#include <QQmlEngine>

using namespace governikus;

CardPositionModel::CardPositionModel()
	: QObject()
	, mCyclingClock(5000)
	, mCurrentIndex(0)
#if defined(Q_OS_IOS)
	, mCardPositions({CardPosition(0.5, 0.0, -1)})
#else
	, mCardPositions({
				CardPosition(0.5, 0.5, -1, 90),
				CardPosition(0.5, 0.5, -1),
				CardPosition(0.5, 0.25, -1),
				CardPosition(0.5, 0.75, -1)
			})
#endif
{
	connect(&mCyclingTimer, &QTimer::timeout, this, &CardPositionModel::onTimerTimeout);
}


void CardPositionModel::startTimer()
{
	mCyclingTimer.start(mCyclingClock);

	mCurrentIndex = 0;
	Q_EMIT fireCardPositionChanged();
}


void CardPositionModel::stopTimer()
{
	mCyclingTimer.stop();
}


int CardPositionModel::getCyclingClock() const
{
	return mCyclingClock;
}


void CardPositionModel::setCyclingClock(int pCyclingClock)
{
	if (pCyclingClock != mCyclingClock)
	{
		mCyclingClock = pCyclingClock;

		if (getIsRunning())
		{
			startTimer();
		}

		Q_EMIT fireCyclingClockChanged();
	}
}


QVariant CardPositionModel::getCardPosition() const
{
	Q_ASSERT(mCurrentIndex >= 0 && mCurrentIndex < mCardPositions.size());

	auto* cardPosition = new CardPosition(mCardPositions[mCurrentIndex]);
	QQmlEngine::setObjectOwnership(cardPosition, QQmlEngine::JavaScriptOwnership);
	return QVariant::fromValue<QObject*>(cardPosition);
}


bool CardPositionModel::getIsRunning() const
{
	return mCyclingTimer.isActive();
}


void CardPositionModel::setIsRunning(bool pRunning)
{
	if (pRunning == getIsRunning())
	{
		return;
	}

	pRunning ? startTimer() : stopTimer();

	Q_EMIT fireIsRunningChanged();
}


int CardPositionModel::getCardPositionCount() const
{
	return mCardPositions.count();
}


void CardPositionModel::onTimerTimeout()
{
	mCurrentIndex = (mCurrentIndex + 1) % mCardPositions.size();
	Q_EMIT fireCardPositionChanged();
}
