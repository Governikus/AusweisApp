/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ResponseApdu.h"

#include <QLoggingCategory>
#include <QtEndian>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)

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
	const char value = mBuffer.at(length() - RETURN_CODE_LENGTH);
	if (!Enum<SW1>::isValue(value))
	{
		qCCritical(card) << "Unknown SW1 value, returning INVALID, value:" << QString::number(value, 16);
		return SW1::INVALID;
	}
	return SW1(value);
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


#include "moc_ResponseApdu.cpp"
