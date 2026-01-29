/**
 * Copyright (c) 2020-2026 Governikus GmbH & Co. KG, Germany
 */

#include "CardPosition.h"


using namespace governikus;


CardPosition::CardPosition(const Data& pData)
	: QObject()
	, mData(pData)
{
	Q_ASSERT(mData.mXPosition >= 0.0 && mData.mXPosition <= 1.0);
	Q_ASSERT(mData.mYPosition >= 0.0 && mData.mYPosition <= 1.0);
	Q_ASSERT(mData.mRotation >= 0.0 && mData.mRotation <= 360.0);
	Q_ASSERT(mData.mZPosition == -1 || mData.mZPosition == 1);
}


double CardPosition::getXPosition() const
{
	return mData.mXPosition;
}


double CardPosition::getYPosition() const
{
	return mData.mYPosition;
}


double CardPosition::getRotation() const
{
	return mData.mRotation;
}


int CardPosition::getZPosition() const
{
	return mData.mZPosition;
}


#ifndef QT_NO_DEBUG
bool CardPosition::operator==(const CardPosition& pOther) const
{
	return mData.mXPosition == pOther.mData.mXPosition
		   && mData.mYPosition == pOther.mData.mYPosition
		   && mData.mRotation == pOther.mData.mRotation
		   && mData.mZPosition == pOther.mData.mZPosition;
}


#endif
