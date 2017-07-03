/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */
#include "CallCost.h"

#include <QDebugStateSaver>
#include <QJsonObject>


using namespace governikus;


CallCost::CallCost(int pFreeSeconds, double pLandlineCentsPerMinute, double pLandlineCentsPerCall, double pMobileCentsPerMinute, double pMobileCentsPerCall)
	: mFreeSeconds(pFreeSeconds)
	, mLandlineCentsPerMinute(pLandlineCentsPerMinute)
	, mLandlineCentsPerCall(pLandlineCentsPerCall)
	, mMobileCentsPerMinute(pMobileCentsPerMinute)
	, mMobileCentsPerCall(pMobileCentsPerCall)
{
}


CallCost::CallCost(const QJsonValue& pJson)
	: mFreeSeconds(pJson.toObject()["free-seconds"].toInt())
	, mLandlineCentsPerMinute(pJson.toObject()["landline"].toObject()["per-minute"].toDouble())
	, mLandlineCentsPerCall(pJson.toObject()["landline"].toObject()["per-call"].toDouble())
	, mMobileCentsPerMinute(pJson.toObject()["mobile"].toObject()["per-minute"].toDouble())
	, mMobileCentsPerCall(pJson.toObject()["mobile"].toObject()["per-call"].toDouble())
{
}


bool CallCost::isNull() const
{
	return mFreeSeconds == 0 && mLandlineCentsPerMinute == 0.0 && mLandlineCentsPerCall == 0.0 && mMobileCentsPerMinute == 0.0 && mMobileCentsPerCall == 0.0;
}


int CallCost::getFreeSeconds() const
{
	return mFreeSeconds;
}


double CallCost::getLandlineCentsPerMinute() const
{
	return mLandlineCentsPerMinute;
}


double CallCost::getLandlineCentsPerCall() const
{
	return mLandlineCentsPerCall;
}


double CallCost::getMobileCentsPerMinute() const
{
	return mMobileCentsPerMinute;
}


double CallCost::getMobileCentsPerCall() const
{
	return mMobileCentsPerCall;
}


QDebug operator<<(QDebug pDbg, const CallCost& pCallCost)
{
	QDebugStateSaver saver(pDbg);
	if (pCallCost.isNull())
	{
		pDbg.nospace() << "CallCost(null)";
	}
	else
	{
		pDbg.nospace() << "CallCost(free=" << pCallCost.getFreeSeconds()
					   << ", ll-call=" << pCallCost.getLandlineCentsPerCall()
					   << ", ll-min=" << pCallCost.getLandlineCentsPerMinute()
					   << ", mob-call=" << pCallCost.getMobileCentsPerCall()
					   << ", mob-min=" << pCallCost.getMobileCentsPerMinute() << ")";
	}
	return pDbg;
}
