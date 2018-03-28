/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "Apdu.h"

#include "Commands.h"

#include <QLoggingCategory>
#include <QtEndian>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


Apdu::Apdu(const QByteArray& pBuffer)
	: mBuffer(pBuffer)
{
}


Apdu::~Apdu()
{
}


int Apdu::length() const
{
	return mBuffer.size();
}


const QByteArray& Apdu::getBuffer() const
{
	return mBuffer;
}


CommandApdu::CommandApdu(const QByteArray& pBuffer, bool pUpdateRetryCounter)
	: Apdu(pBuffer)
	, mUpdateRetryCounter(pUpdateRetryCounter)
{
}


bool CommandApdu::isExtendedLength() const
{
	// no data, no le: size == 4
	// no data, short le: size == 5
	// no data, extended le: size == 7, high order le byte == 0
	// data, with/without le: high order size byte == 0 <=> extended data length
	return length() > 5 && mBuffer.at(4) == '\0';
}


bool CommandApdu::isExtendedLength(const QByteArray& pData, int pLe)
{
	return pData.size() > SHORT_MAX_LC || pLe > SHORT_MAX_LE;
}


bool CommandApdu::isSecureMessaging(const QByteArray& pCommandBuffer)
{
	return !pCommandBuffer.isEmpty() && ((pCommandBuffer.at(0) & 0x0F) == CLA_SECURE_MESSAGING);
}


CommandApdu::CommandApdu(const QByteArray& pHeader, const QByteArray& pData, int pLe)
	: CommandApdu(pHeader.at(0), pHeader.at(1), pHeader.at(2), pHeader.at(3), pData, pLe)
{
}


CommandApdu::CommandApdu(char pCla, char pIns, char pP1, char pP2, const QByteArray& pData, int pLe)
	: Apdu(QByteArray())
	, mUpdateRetryCounter(false)
{
	if (pData.size() > EXTENDED_MAX_LC)
	{
		qCCritical(card) << "Command data exceeds maximum of 0xFFFF bytes";
		return;
	}
	if (pLe > EXTENDED_MAX_LE)
	{
		qCCritical(card) << "Expected length exceeds maximum value of 0x010000";
		return;
	}

	mBuffer += pCla;
	mBuffer += pIns;
	mBuffer += pP1;
	mBuffer += pP2;

	//
	// according to ISO 7816 Part 4, chapter 5.1
	//
	if (pData.size() > 0)  // withPayload
	{
		if (CommandApdu::isExtendedLength(pData, pLe))
		{
			mBuffer += '\0';
			mBuffer += static_cast<char>(pData.size() >> 8 & 0xff);
			mBuffer += static_cast<char>(pData.size() & 0xff);
		}
		else
		{
			mBuffer += static_cast<char>(pData.size() & 0xff);
		}

		mBuffer += pData;
	}

	if (pLe > 0)
	{
		if (CommandApdu::isExtendedLength(pData, pLe))
		{
			if (pData.isEmpty())
			{
				mBuffer += '\0';
			}
			mBuffer += static_cast<char>(pLe >> 8 & 0xff);
		}
		mBuffer += static_cast<char>(pLe & 0xff);
	}
}


CommandApdu::~CommandApdu()
{
}


char CommandApdu::getCLA() const
{
	return length() > 0 ? mBuffer.at(0) : '\0';
}


char CommandApdu::getINS() const
{
	return length() > 1 ? mBuffer.at(1) : '\0';
}


char CommandApdu::getP1() const
{
	return length() > 2 ? mBuffer.at(2) : '\0';
}


char CommandApdu::getP2() const
{
	return mBuffer.size() > 3 ? mBuffer.at(3) : '\0';
}


static inline int readLength(const QByteArray& pByteArray, int pOffset)
{
	return static_cast<int>(static_cast<uchar>(pByteArray.at(pOffset)) << 8) | static_cast<int>(static_cast<uchar>(pByteArray.at(pOffset + 1)));
}


int CommandApdu::getLc() const
{
	if (length() <= 5)
	{
		return 0;
	}
	if (!isExtendedLength())
	{
		// short command apdu
		return static_cast<uchar>(mBuffer.at(4));
	}
	// extended length command apdu
	if (length() <= 6)
	{
		qCCritical(card) << "Cannot determine Lc, returning 0";
		return 0;
	}
	return readLength(mBuffer, 5);
}


int CommandApdu::getLe() const
{
	int lc = getLc();
	if (isExtendedLength())
	{
		// no data (so lc==0): we have 4 bytes header and the le field is prefixed with 0 byte
		// with data: we have 4 bytes header, lc field encoded in 3 bytes and the data field
		int offset = lc == 0 ? 5 : 7 + lc;
		if (length() < offset + 2)
		{
			return 0;
		}
		int le = readLength(mBuffer, offset);
		return le == 0 ? EXTENDED_MAX_LE : le;
	}
	// no data (so lc==0): we have 4 bytes header
	// with data: we have 4 bytes header, lc field encoded in 1 byte and the data field
	int offset = lc == 0 ? 4 : 5 + lc;
	if (length() < offset + 1)
	{
		return 0;
	}
	int le = static_cast<uchar>(mBuffer.at(offset));
	return le == 0 ? SHORT_MAX_LE : le;
}


QByteArray CommandApdu::getData() const
{
	int lc = getLc();
	if (lc == 0)
	{
		return QByteArray();
	}
	if (isExtendedLength())
	{
		// 4 bytes header + 3 bytes length
		return mBuffer.mid(7, lc);
	}
	// 4 bytes header + 1 bytes length
	return mBuffer.mid(5, lc);
}


bool CommandApdu::isUpdateRetryCounter() const
{
	return mUpdateRetryCounter;
}


ResponseApdu::ResponseApdu(StatusCode pStatusCode)
	: Apdu(QByteArray())
{
	char buffer[2];
	qToBigEndian<quint16>(Enum<StatusCode>::getValue(pStatusCode), buffer);
	setBuffer(QByteArray(buffer, 2));
}


ResponseApdu::ResponseApdu(const QByteArray& pBuffer)
	: Apdu(pBuffer)
{
}


ResponseApdu::~ResponseApdu()
{
}


void ResponseApdu::setBuffer(const QByteArray& pBuffer)
{
	mBuffer = pBuffer;
}


QByteArray ResponseApdu::getData() const
{
	if (length() < RETURN_CODE_LENGTH)
	{
		return QByteArray();
	}

	return mBuffer.left(getDataLength());
}


int ResponseApdu::getDataLength() const
{
	return length() - RETURN_CODE_LENGTH;
}


StatusCode ResponseApdu::getReturnCode() const
{
	if (mBuffer.isEmpty())
	{
		return StatusCode::EMPTY;
	}

	const int returnCodeAsInt = getReturnCodeAsHex().toInt(nullptr, 16);
	return Enum<StatusCode>::isValue(returnCodeAsInt) ? StatusCode(returnCodeAsInt) : StatusCode::INVALID;
}


QByteArray ResponseApdu::getReturnCodeAsHex() const
{
	return mBuffer.right(RETURN_CODE_LENGTH).toHex();
}


int ResponseApdu::getRetryCounter() const
{
	StatusCode statusCode = getReturnCode();
	if (statusCode == StatusCode::SUCCESS)
	{
		return 3;
	}
	if (statusCode == StatusCode::PIN_RETRY_COUNT_2)
	{
		return 2;
	}
	if (statusCode == StatusCode::PIN_SUSPENDED)
	{
		return 1;
	}
	if (statusCode == StatusCode::PIN_BLOCKED || statusCode == StatusCode::PIN_DEACTIVATED)
	{
		return 0;
	}
	return -1;
}


SW1 ResponseApdu::getSW1() const
{
	if (length() < RETURN_CODE_LENGTH)
	{
		qCCritical(card) << "Buffer too short, returning 0";
		return SW1::INVALID;
	}
	return SW1(mBuffer.at(length() - RETURN_CODE_LENGTH));
}


char ResponseApdu::getSW2() const
{
	if (length() < 1)
	{
		qCCritical(card) << "Buffer too short, returning 0";
		return 0;
	}
	return mBuffer.at(length() - 1);
}


CardReturnCode ResponseApdu::getCardReturnCode() const
{
	switch (getReturnCode())
	{
		case StatusCode::SUCCESS:
			return CardReturnCode::OK;

		case StatusCode::INPUT_TIMEOUT:
			return CardReturnCode::INPUT_TIME_OUT;

		case StatusCode::INPUT_CANCELLED:
			return CardReturnCode::CANCELLATION_BY_USER;

		case StatusCode::PASSWORDS_DIFFER:
			return CardReturnCode::NEW_PIN_MISMATCH;

		case StatusCode::PASSWORD_OUTOF_RANGE:
			return CardReturnCode::NEW_PIN_INVALID_LENGTH;

		case StatusCode::PIN_BLOCKED:
			return CardReturnCode::PIN_BLOCKED;

		default:
			return CardReturnCode::PROTOCOL_ERROR;
	}

	Q_UNREACHABLE();
}


#include "moc_Apdu.cpp"
