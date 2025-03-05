/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardPosition.h"

#include <QList>
#include <QObject>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

class test_CardPositionModel;

namespace governikus
{

class CardPositionModel
	: public QObject
{
	Q_OBJECT
	QML_ELEMENT

	friend class ::test_CardPositionModel;

	Q_PROPERTY(int count READ getCardPositionCount CONSTANT)
	Q_PROPERTY(int cyclingClock READ getCyclingClock WRITE setCyclingClock NOTIFY fireCyclingClockChanged)
	Q_PROPERTY(bool running READ getIsRunning WRITE setIsRunning NOTIFY fireIsRunningChanged)

	private:
		int mCyclingClock;
		qsizetype mCurrentIndex;
		QTimer mCyclingTimer;
		const QList<CardPosition> mCardPositions;

		void startTimer();
		void stopTimer();

	public:
		CardPositionModel();

		[[nodiscard]] int getCyclingClock() const;
		void setCyclingClock(int pCyclingClock);
		[[nodiscard]] Q_INVOKABLE QVariant getCardPosition() const;
		[[nodiscard]] bool getIsRunning() const;
		void setIsRunning(bool pRunning);
		[[nodiscard]] int getCardPositionCount() const;

	Q_SIGNALS:
		void fireCyclingClockChanged();
		void fireCardPositionChanged();
		void fireIsRunningChanged();

	private Q_SLOTS:
		void onTimerTimeout();
};


} // namespace governikus
