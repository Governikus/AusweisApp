/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Apdu.h"
#include "CardReturnCode.h"
#include "EnumHelper.h"


namespace governikus
{
defineTypedEnumType(StatusCode, quint16,
		EMPTY = 0x0000,
		INVALID = 0x0001,
		SUCCESS = 0x9000,
		NO_PKCS15_APP = 0x6200,
		END_OF_FILE = 0x6282,
		PIN_DEACTIVATED = 0x6283,
		FCI_NO_ISO7816_4 = 0x6284,
		VERIFICATION_FAILED = 0x6300,
		INPUT_TIMEOUT = 0x6400,
		INPUT_CANCELLED = 0x6401,
		PASSWORDS_DIFFER = 0x6402,
		PASSWORD_OUTOF_RANGE = 0x6403,
		CARD_EJECTED_AND_REINSERTED = 0x64a2,
		EEPROM_CELL_DEFECT = 0x6581,
		SECURITY_ENVIRONMENT = 0x6600,
		WRONG_LENGTH = 0x6700,
		NO_BINARY_FILE = 0x6981,
		LAST_CHAIN_CMD_EXPECTED = 0x6883,
		ACCESS_DENIED = 0x6982,
		PASSWORD_COUNTER_EXPIRED = 0x6983,
		DIRECTORY_OR_PASSWORD_LOCKED_OR_NOT_ALLOWED = 0x6984,
		NO_PARENT_FILE = 0x6985,
		NOT_YET_INITIALIZED = 0x6985,
		NO_CURRENT_DIRECTORY_SELECTED = 0x6986,
		DATAFIELD_EXPECTED = 0x6987,
		INVALID_SM_OBJECTS = 0x6988,
		COMMAND_NOT_ALLOWED = 0x69f0,
		INVALID_DATAFIELD = 0x6a80,
		ALGORITHM_ID = 0x6a81,
		FILE_NOT_FOUND = 0x6a82,
		RECORD_NOT_FOUND = 0x6a83,
		INVALID_PARAMETER = 0x6a86,
		LC_INCONSISTANT = 0x6a87,
		PASSWORD_NOT_FOUND = 0x6a88,
		ILLEGAL_OFFSET = 0x6b00,
		UNSUPPORTED_CLA = 0x6e00,
		CANT_DISPLAY = 0x6410,
		INVALID_P1P2 = 0x6a00,
		UNSUPPORTED_INS = 0x6d00,
		PIN_BLOCKED = 0x63c0, // retries left: 0
		PIN_SUSPENDED = 0x63c1, // retries left: 1
		PIN_RETRY_COUNT_2 = 0x63c2, // retries left: 2
		)

/*
 * As defined in ISO-7816-4 Table-5
 */
defineTypedEnumType(SW1, quint8,
		INVALID = 0x00,
		MORE_DATA_AVAILABLE = 0x61,
		NONVOLATILE_MEMORY_UNCHANGED_1 = 0x62,
		NONVOLATILE_MEMORY_CHANGED_1 = 0x63,
		NONVOLATILE_MEMORY_UNCHANGED_2 = 0x64,
		NONVOLATILE_MEMORY_CHANGED_2 = 0x65,
		SECURITY_ISSUE = 0x66,
		WRONG_LENGTH = 0x67,
		FUNCTIONS_IN_CLASS_NOT_SUPPORTED = 0x68,
		ERROR_COMMAND_NOT_ALLOWED = 0x69,
		WRONG_PARAMETERS_P1_P2 = 0x6a,
		WRONG_PARAMETERS_P1_P2_NO_INFO = 0x6b,
		WRONG_LE_FIELD = 0x6c,
		INSTRUCTION_CODE_INVALID = 0x6d,
		CLASS_NOT_SUPPORTED = 0x6e,
		NO_PRECISE_DIAGNOSIS = 0x6f,
		SUCCESS = 0x90,
		)

class ResponseApdu
	: public Apdu
{
	private:
		static const int RETURN_CODE_LENGTH = 2;

	protected:
		virtual void setBuffer(const QByteArray& pBuffer);

	public:
		explicit ResponseApdu(StatusCode pStatusCode);
		explicit ResponseApdu(const QByteArray& pBuffer = QByteArray());
		virtual ~ResponseApdu() = default;

		QByteArray getData() const;
		int getDataLength() const;
		StatusCode getReturnCode() const;
		QByteArray getReturnCodeAsHex() const;
		int getRetryCounter() const;
		SW1 getSW1() const;
		char getSW2() const;
		CardReturnCode getCardReturnCode() const;
};

struct ResponseApduResult
{
	CardReturnCode mReturnCode = CardReturnCode::UNDEFINED;
	ResponseApdu mResponseApdu = ResponseApdu();
};

#ifndef QT_NO_DEBUG
inline bool operator ==(const ResponseApduResult& pLeft, const ResponseApduResult& pRight)
{
	return pLeft.mReturnCode == pRight.mReturnCode && pLeft.mResponseApdu.getBuffer() == pRight.mResponseApdu.getBuffer();
}


#endif

} // namespace governikus
