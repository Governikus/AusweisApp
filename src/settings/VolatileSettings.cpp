/*!
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

#include "VolatileSettings.h"

#include "SingletonHelper.h"

using namespace governikus;

defineSingleton(VolatileSettings)


VolatileSettings::VolatileSettings()
	: mUsedAsSdk(true)
{
}


bool VolatileSettings::isUsedAsSDK() const
{
	return mUsedAsSdk;
}


void VolatileSettings::setUsedAsSDK(bool pSdk)
{
	mUsedAsSdk = pSdk;
}
