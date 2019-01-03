/*!
 * \brief Phone call cost representation
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QDebug>
#include <QJsonValue>


namespace governikus
{

class CallCost
{
	friend bool operator==(const CallCost& pLeft, const CallCost& pRight);

	int mFreeSeconds;
	double mLandlineCentsPerMinute, mLandlineCentsPerCall;
	double mMobileCentsPerMinute, mMobileCentsPerCall;

	public:
		CallCost(int pFreeSeconds = 0, double pLandlineCentsPerMinute = 0.0, double pLandlineCentsPerCall = 0.0, double pMobileCentsPerMinute = 0.0, double pMobileCentsPerCall = 0.0);
		CallCost(const QJsonValue& pJson);

		void load();
		void save();

		bool isNull() const;
		int getFreeSeconds() const;
		double getLandlineCentsPerMinute() const;
		double getLandlineCentsPerCall() const;
		double getMobileCentsPerMinute() const;
		double getMobileCentsPerCall() const;
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
