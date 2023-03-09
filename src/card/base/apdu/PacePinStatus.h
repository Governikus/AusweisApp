/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ResponseApdu.h"

namespace governikus
{

class PacePinStatus
{
	public:
		[[nodiscard]] static int getRetryCounter(StatusCode pStatusCode);

		[[nodiscard]] static bool isInitial(StatusCode pStatusCode);

		[[nodiscard]] static bool isDeactivated(StatusCode pStatusCode);
};

} // namespace governikus
