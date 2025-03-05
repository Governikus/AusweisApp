/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "AutoStart.h"

#include "VolatileSettings.h"


using namespace governikus;


bool AutoStart::set(bool pEnabled)
{
	if (Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		return false;
	}

	return setInternal(pEnabled);
}
