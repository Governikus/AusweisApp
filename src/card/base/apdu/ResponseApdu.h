/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardReturnCode.h"
#include "EnumHelper.h"


namespace governikus
{
defineTypedEnumType(StatusCode, quint16,
		UNKNOWN = 0x0000,
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
		CARD_EJECTED_AND_REINSERTED = 0x64A2,
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
		COMMAND_NOT_ALLOWED = 0x69F0,
		INVALID_DATAFIELD = 0x6A80,
		ALGORITHM_ID = 0x6A81,
		FILE_NOT_FOUND = 0x6A82,
		RECORD_NOT_FOUND = 0x6A83,
		INVALID_PARAMETER = 0x6A86,
		LC_INCONSISTANT = 0x6A87,
		PASSWORD_NOT_FOUND = 0x6A88,
		ILLEGAL_OFFSET = 0x6B00,
		UNSUPPORTED_CLA = 0x6E00,
		CANT_DISPLAY = 0x6410,
		INVALID_P1P2 = 0x6A00,
		UNSUPPORTED_INS = 0x6D00,
		PIN_BLOCKED = 0x63C0, // retries left: 0
		PIN_SUSPENDED = 0x63C1, // retries left: 1
		PIN_RETRY_COUNT_2 = 0x63C2, // retries left: 2
		INITIAL_PIN_BLOCKED = 0x63D0,
		INITIAL_PIN_RETRY_COUNT_1 = 0x63D1,
		INITIAL_PIN_RETRY_COUNT_2 = 0x63D2,
		INITIAL_PIN_RETRY_COUNT_3 = 0x63D3
		)

// According to ISO-7816-4, 5.6 Status bytes, Table 6
defineTypedEnumType(SW1, quint8,
		UNKNOWN = 0x00,
		MORE_DATA_AVAILABLE = 0x61,
		NONVOLATILE_MEMORY_UNCHANGED_1 = 0x62,
		NONVOLATILE_MEMORY_CHANGED_1 = 0x63,
		NONVOLATILE_MEMORY_UNCHANGED_2 = 0x64,
		NONVOLATILE_MEMORY_CHANGED_2 = 0x65,
		SECURITY_ISSUE = 0x66,
		WRONG_LENGTH = 0x67,
		FUNCTIONS_IN_CLASS_NOT_SUPPORTED = 0x68,
		ERROR_COMMAND_NOT_ALLOWED = 0x69,
		WRONG_PARAMETERS_P1_P2 = 0x6A,
		WRONG_PARAMETERS_P1_P2_NO_INFO = 0x6B,
		WRONG_LE_FIELD = 0x6C,
		INSTRUCTION_CODE_INVALID = 0x6D,
		CLASS_NOT_SUPPORTED = 0x6E,
		NO_PRECISE_DIAGNOSIS = 0x6F,
		SUCCESS = 0x90
		)

class ResponseApdu final
{
	private:
		quint16 mStatusCode;
		QByteArray mData;

	public:
		explicit ResponseApdu(StatusCode pStatusCode, const QByteArray& pData = QByteArray());
		explicit ResponseApdu(const QByteArray& pBuffer = QByteArray());

		[[nodiscard]] bool isEmpty() const;
		[[nodiscard]] const QByteArray& getData() const;
		[[nodiscard]] StatusCode getStatusCode() const;
		[[nodiscard]] QByteArray getStatusBytes() const;
		[[nodiscard]] SW1 getSW1() const;
		[[nodiscard]] char getSW2() const;

		operator QByteArray() const;
};


inline QDebug operator<<(QDebug pDbg, const ResponseApdu& pResponseApdu)
{
	QDebugStateSaver saver(pDbg);
	pDbg << QByteArray(pResponseApdu).toHex();
	return pDbg;
}


char* toString(const ResponseApdu& pResponseApdu);


struct ResponseApduResult
{
	CardReturnCode mReturnCode = CardReturnCode::UNDEFINED;
	ResponseApdu mResponseApdu = ResponseApdu();
};

#ifndef QT_NO_DEBUG
inline bool operator==(const ResponseApdu& pLeft, const ResponseApdu& pRight)
{
	return QByteArray(pLeft) == QByteArray(pRight);
}


inline bool operator==(const ResponseApduResult& pLeft, const ResponseApduResult& pRight)
{
	return pLeft.mReturnCode == pRight.mReturnCode && pLeft.mResponseApdu == pRight.mResponseApdu;
}


#endif

} // namespace governikus
