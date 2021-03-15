/*!
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

#include "CardPosition.h"

using namespace governikus;

CardPosition::CardPosition(double pXPosition, double pYPosition, int pZPosition, double pRotation)
	: QObject()
	, mXPosition(pXPosition)
	, mYPosition(pYPosition)
	, mZPosition(pZPosition)
	, mRotation(pRotation)
{
	Q_ASSERT(mXPosition >= 0.0 && mXPosition <= 1.0);
	Q_ASSERT(mYPosition >= 0.0 && mYPosition <= 1.0);
	Q_ASSERT(mZPosition == -1 || mZPosition == 1);
	Q_ASSERT(mRotation >= 0 && mRotation <= 360);
}


CardPosition::CardPosition(const CardPosition& pOther)
	: QObject()
	, mXPosition(pOther.mXPosition)
	, mYPosition(pOther.mYPosition)
	, mZPosition(pOther.mZPosition)
	, mRotation(pOther.mRotation)
{

}


CardPosition& CardPosition::operator=(const CardPosition& pOther)
{
	mXPosition = pOther.mXPosition;
	mYPosition = pOther.mYPosition;
	mZPosition = pOther.mZPosition;
	mRotation = pOther.mRotation;
	return *this;
}
