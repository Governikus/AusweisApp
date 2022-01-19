/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

namespace governikus
{
defineEnumType(CardType,
		NONE,
		UNKNOWN,
		PASSPORT,
		EID_CARD)

defineTypedEnumType(PacePasswordId, char,
		UNKNOWN = 0x00,
		PACE_MRZ = 0x01,
		PACE_CAN = 0x02,
		PACE_PIN = 0x03,
		PACE_PUK = 0x04)

} // namespace governikus

Q_DECLARE_METATYPE(governikus::PacePasswordId)
