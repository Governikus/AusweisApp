/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Phone call cost representation
 */

#pragma once

#include <QDebug>
#include <QJsonValue>


namespace governikus
{

class CallCost
{
	friend bool operator==(const CallCost& pLeft, const CallCost& pRight);

	private:
		int mFreeSeconds;
		double mLandlineCentsPerMinute;
		double mLandlineCentsPerCall;
		double mMobileCentsPerMinute;
		double mMobileCentsPerCall;

	public:
		explicit CallCost(int pFreeSeconds = 0, double pLandlineCentsPerMinute = 0.0, double pLandlineCentsPerCall = 0.0, double pMobileCentsPerMinute = 0.0, double pMobileCentsPerCall = 0.0);
		explicit CallCost(const QJsonValue& pJson);

		void load();
		void save();

		[[nodiscard]] bool isNull() const;
		[[nodiscard]] int getFreeSeconds() const;
		[[nodiscard]] double getLandlineCentsPerMinute() const;
		[[nodiscard]] double getLandlineCentsPerCall() const;
		[[nodiscard]] double getMobileCentsPerMinute() const;
		[[nodiscard]] double getMobileCentsPerCall() const;
};


inline bool operator==(const CallCost& pLeft, const CallCost& pRight)
{
	return &pLeft == &pRight || (
		pLeft.mFreeSeconds == pRight.mFreeSeconds &&
		pLeft.mLandlineCentsPerMinute == pRight.mLandlineCentsPerMinute &&
		pLeft.mLandlineCentsPerCall == pRight.mLandlineCentsPerCall &&
		pLeft.mMobileCentsPerMinute == pRight.mMobileCentsPerMinute &&
		pLeft.mMobileCentsPerCall == pRight.mMobileCentsPerCall);
}


} // namespace governikus

QDebug operator<<(QDebug pDbg, const governikus::CallCost& pCallCost);
