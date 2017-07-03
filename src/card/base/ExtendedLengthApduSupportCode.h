/*!
 * Definition of enum ExtendedLengthApduSupportCode.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"

namespace governikus
{
defineEnumType(ExtendedLengthApduSupportCode,
		UNKNOWN = -1,
		NOT_SUPPORTED = 0,
		SUPPORTED = 1,
		INVALID = 2)
}
