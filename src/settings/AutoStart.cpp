/**
 * Copyright (c) 2022-2026 Governikus Service GmbH, Germany
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
