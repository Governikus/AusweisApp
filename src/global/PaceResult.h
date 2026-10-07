/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "EnumHelper.h"


namespace governikus
{

defineEnumTypeQmlExposed(PaceResult,
		UNDEFINED,
		OK_PIN,
		OK_PIN_AUTH,
		OK_CAN,
		OK_CAN_AUTH,
		OK_PUK,
		INVALID_PIN_1,
		INVALID_PIN_2,
		INVALID_PIN_3,
		INVALID_CAN,
		INVALID_PUK)


} // namespace governikus
