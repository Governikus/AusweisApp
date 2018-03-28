/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

namespace governikus
{
defineEnumType(CardType,
		NONE,
		UNKNOWN,
		EID_CARD)

defineTypedEnumType(PACE_PASSWORD_ID, char,
		PACE_MRZ = 0x01,
		PACE_CAN = 0x02,
		PACE_PIN = 0x03,
		PACE_PUK = 0x04)

}
