/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

namespace governikus
{
defineEnumType(CardType,
		NONE,
		UNKNOWN,
		EID_CARD,
		SMART_EID)

defineTypedEnumType(PacePasswordId, char,
		UNKNOWN = 0x00,
		PACE_MRZ = 0x01,
		PACE_CAN = 0x02,
		PACE_PIN = 0x03,
		PACE_PUK = 0x04)

// TR03159-4
defineEnumType(AcceptedEidType,
		CARD_CERTIFIED,
		SE_CERTIFIED,
		SE_ENDORSED,
		HW_KEYSTORE)

// TR 03110 Smart-eID Amendment
defineEnumType(MobileEidType,
		UNKNOWN,
		SE_CERTIFIED,
		SE_ENDORSED,
		HW_KEYSTORE)

} // namespace governikus
