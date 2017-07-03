/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"

namespace governikus
{
defineEnumType(ReaderType,
		UNKNOWN,
		REINER_cyberJack_RFID_komfort,
		REINER_cyberJack_RFID_standard,
		REINER_cyberJack_RFID_basis,
		REINER_cyberJack_wave,
		KOBIL_IDToken,
		SCM_SDI010,
		SCM_SDI011,
		SCM_SCL011_Contactless_Reader,
		ACS_ACR1281_PICC_Reader,
		OMNIKEY_4121_CL,
		OMNIKEY_CardMan_5x21_CL,
		FEIG_OBID_myAXXESS_basic,
		Gemalto_Prox_SU,
		Gemalto_Prox_DU)

defineEnumType(CardType,
		NONE,
		UNKNOWN,
		EID_CARD)

defineEnumType(PACE_PIN_ID,
		PACE_MRZ = 0x01,
		PACE_CAN = 0x02,
		PACE_PIN = 0x03,
		PACE_PUK = 0x04)

}
