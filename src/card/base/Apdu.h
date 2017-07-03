/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"
#include <QByteArray>

namespace governikus
{

defineEnumType(StatusCode,
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
		PIN_BLOCKED = 0x63c0, //retrys left 0
		PIN_SUSPENDED = 0x63c1, //retrys left 1
		PIN_RETRY_COUNT_2 = 0x63c2, //retrys left 2
		)


/*
 * As defined in ISO-7816-4 Table-5
 */
defineEnumType(SW1,
		ERROR_COMMAND_NOT_ALLOWED = 0x69,
		)

class Apdu
{
	protected:
		QByteArray mBuffer;

		Apdu(const QByteArray& pBuffer);
		virtual ~Apdu();

	public:
		static const int NO_LE = 0;
		static const int SHORT_MAX_LC = 0xff;
		static const int SHORT_MAX_LE = 0x0100;
		static const int EXTENDED_MAX_LC = 0x00ffff;
		static const int EXTENDED_MAX_LE = 0x010000;
		static const char CLA = 0x00;
		static const char CLA_COMMAND_CHAINING = 0x10;
		static const char CLA_SECURE_MESSAGING = 0x0c;
		const QByteArray& getBuffer() const;
		size_t length() const;
};

class CommandApdu
	: public Apdu
{
	private:
		inline bool isExtendedLength() const;

	public:
		CommandApdu(const QByteArray& pBuffer);
		CommandApdu(const QByteArray& pHeader, const QByteArray& pData, int pLe);
		CommandApdu(char pCla, char pIns, char pP1, char pP2, const QByteArray& pData = QByteArray(), int pLe = NO_LE);
		virtual ~CommandApdu();

		char getCLA() const;
		char getINS() const;
		char getP1() const;
		char getP2() const;
		int getLc() const;
		int getLe() const;
		QByteArray getData() const;

		static bool isExtendedLength(const QByteArray& pData, int pLe);
		static bool isSecureMessaging(const QByteArray& pCommandBuffer);
};

class ResponseApdu
	: public Apdu
{

	public:
		ResponseApdu(const QByteArray& pBuffer = QByteArray());
		virtual ~ResponseApdu();

		virtual void setBuffer(const QByteArray& pBuffer);
		QByteArray getData() const;
		int getDataLength() const;
		StatusCode getReturnCode() const;
		int getSW1() const;
		int getSW2() const;
};

} /* namespace governikus */
