/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */

#include "PacePinStatus.h"

using namespace governikus;

int PacePinStatus::getRetryCounter(StatusCode pStatusCode)
{
	switch (pStatusCode)
	{
		case StatusCode::SUCCESS:
			return 3;

		case StatusCode::PIN_RETRY_COUNT_2:
			return 2;

		case StatusCode::PIN_SUSPENDED:
			return 1;

		case StatusCode::PIN_BLOCKED:
		case StatusCode::PIN_DEACTIVATED:
			return 0;

		default:
			return -1;
	}
}


bool PacePinStatus::isDeactivated(StatusCode pStatusCode)
{
	return pStatusCode == StatusCode::PIN_DEACTIVATED;
}
