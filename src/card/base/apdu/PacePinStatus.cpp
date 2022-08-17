/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

#include "PacePinStatus.h"

using namespace governikus;

int PacePinStatus::getRetryCounter(StatusCode pStatusCode)
{
	switch (pStatusCode)
	{
		case StatusCode::SUCCESS:
		case StatusCode::INITIAL_PIN_RETRY_COUNT_3:
			return 3;

		case StatusCode::PIN_RETRY_COUNT_2:
		case StatusCode::INITIAL_PIN_RETRY_COUNT_2:
			return 2;

		case StatusCode::PIN_SUSPENDED:
		case StatusCode::INITIAL_PIN_RETRY_COUNT_1:
			return 1;

		case StatusCode::PIN_BLOCKED:
		case StatusCode::PIN_DEACTIVATED:
		case StatusCode::INITIAL_PIN_BLOCKED:
			return 0;

		default:
			return -1;
	}
}


bool PacePinStatus::isInitial(StatusCode pStatusCode)
{
	return pStatusCode == StatusCode::INITIAL_PIN_BLOCKED
		   || pStatusCode == StatusCode::INITIAL_PIN_RETRY_COUNT_1
		   || pStatusCode == StatusCode::INITIAL_PIN_RETRY_COUNT_2
		   || pStatusCode == StatusCode::INITIAL_PIN_RETRY_COUNT_3;
}


bool PacePinStatus::isDeactivated(StatusCode pStatusCode)
{
	return pStatusCode == StatusCode::PIN_DEACTIVATED;
}
