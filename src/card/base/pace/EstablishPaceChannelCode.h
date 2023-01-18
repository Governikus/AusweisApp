/*!
 * \brief EstablishPaceChannel error code definitions
 *
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

namespace governikus
{

//
// EstablishPaceChannel error codes according to TR-03119, D.1.2
//
defineTypedEnumType(EstablishPaceChannelErrorCode, quint32,
		NoError
			= 0x00000000,

		// Error in input data
		InconsistentLengthsInInput
			= 0xD0000001,
		UnexpectedDataInInput
			= 0xD0000002,
		UnexpectedCombinationOfDataInInput
			= 0xD0000003,

		// Errors during protocol execution
		SyntaxErrorInTLVResponse
			= 0xE0000001,
		UnexpectedOrMissingObjectInTLVResponse
			= 0xE0000002,
		UnknownPasswordID
			= 0xE0000003,
		WrongAuthenticationToken
			= 0xE0000006,
		CertificateChainForTerminalAuthenticationCannotBeBuilt
			= 0xE0000007,
		UnexpectedDataStructureInResponseToChipAuthentication
			= 0xE0000008,
		PassiveAuthenticationFailed
			= 0xE0000009,
		IncorrectTokenForChipAuthentication
			= 0xE000000A,

		// Response APDU of the card reports error (status code SW1SW2)
		//		Select EF.CardAccess
		//		  0xF000SW1SW2
		//		Read Binary EF.CardAccess
		//		  0xF001SW1SW2
		//		MSE: Set AT for PACE
		//		  0xF002SW1SW2
		//		General Authenticate Step 1 - 4
		//		  0xF003SW1SW2 - 0xF006SW1SW2

		// A specific case with "SW1 == 0x63 == warning" and a "dummy SW2".
		GeneralAuthenticateStep1_4_Warning
			= 0xF0066300,

		// APDU created by PCD for terminal/chip authentication reports error (status code SW1SW2)
		//		MSE: Set DST (first certificate)
		//		  0xF800SW1SW2
		//		PSO: Verify Certificate (first certificate)
		//		  0xF801SW1SW2
		//		MSE: Set DST (second certificate)
		//		  0xF802SW1SW2
		//		PSO: Verify Certificate (second certificate)
		//		  0xF803SW1SW2
		//		MSE: Set DST (third certificate)
		//		  0xF804SW1SW2
		//		PSO: Verify Certificate (third certificate)
		//		  0xF805SW1SW2
		//		MSE: Set AT for terminal authentication
		//		  0xF806SW1SW2
		//		Get Challenge
		//		  0xF807SW1SW2
		//		External Authenticate
		//		  0xF808SW1SW2
		//		Select EF.CardSecurity
		//		  0xF809SW1SW2
		//		Read Binary EF.CardSecurity
		//		  0xF80ASW1SW2
		//		MSE: Set AT for chip authentication
		//		  0xF80BSW1SW2
		//		General Authenticate
		//		  0xF80CSW1SW2

		// Others
		CommunicationAbort
			= 0xF0100001,
		NoCard
			= 0xF0100002,
		Abort
			= 0xF0200001,
		Timeout
			= 0xF0200002,
		NoActivePinSet
			= 0xF0200003
			)


} // namespace governikus
