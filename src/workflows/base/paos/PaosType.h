/*!
 * \brief All possible paos types
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

namespace governikus
{

defineEnumType(PaosType,
		UNKNOWN,
		STARTPAOS,
		STARTPAOS_RESPONSE,
		INITIALIZE_FRAMEWORK,
		INITIALIZE_FRAMEWORK_RESPONSE,
		DID_AUTHENTICATE_EAC1,
		DID_AUTHENTICATE_RESPONSE_EAC1,
		DID_AUTHENTICATE_EAC2,
		DID_AUTHENTICATE_RESPONSE_EAC2,
		DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE,
		TRANSMIT,
		TRANSMIT_RESPONSE
		)

} // namespace governikus
