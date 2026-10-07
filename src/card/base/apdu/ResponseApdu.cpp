/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "ResponseApdu.h"

#include <QLoggingCategory>
#include <QtEndian>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


constexpr quint16 EMPTY = 0;


ResponseApdu::ResponseApdu(StatusCode pStatusCode, const QByteArray& pData)
	: mStatusCode(Enum<StatusCode>::getValue(pStatusCode))
	, mData(pData)
{
	Q_ASSERT(pStatusCode != StatusCode::UNKNOWN);
}


ResponseApdu::ResponseApdu(const QByteArray& pBuffer)
	: mStatusCode(EMPTY)
	, mData()
{
	if (pBuffer.isEmpty())
	{
		return;
	}

	static const int STATUS_CODE_LENGTH = 2;
	QByteArray statusCode = pBuffer.right(STATUS_CODE_LENGTH);
	if (statusCode.size() < STATUS_CODE_LENGTH)
	{
		qCCritical(card) << "One byte status, assuming" << statusCode.toHex() << "is SW2";
		statusCode.prepend(static_cast<char>(0x00));
	}
	mStatusCode = qFromBigEndian<quint16>(statusCode.data());

	if (pBuffer.size() > STATUS_CODE_LENGTH)
	{
		mData = pBuffer.chopped(STATUS_CODE_LENGTH);
	}
}


bool ResponseApdu::isEmpty() const
{
	return mStatusCode == EMPTY && mData.isEmpty();
}


const QByteArray& ResponseApdu::getData() const
{
	return mData;
}


StatusCode ResponseApdu::getStatusCode() const
{
	if (Enum<StatusCode>::isValue(mStatusCode))
	{
		return StatusCode(mStatusCode);
	}

	qCCritical(card) << "Unknown StatusCode value, returning UNKNOWN, value:" << QString::number(mStatusCode, 16);
	return StatusCode::UNKNOWN;
}


QByteArray ResponseApdu::getStatusBytes() const
{
	if (mStatusCode == EMPTY)
	{
		return QByteArray();
	}

	QByteArray statusCode(2, 0);
	qToBigEndian<quint16>(mStatusCode, statusCode.data());
	return statusCode;
}


int ResponseApdu::getRetryCounter() const
{
	switch (getStatusCode())
	{
		case StatusCode::SUCCESS:
			return 3;

		case StatusCode::PIN_RETRY_COUNT_2:
			return 2;

		case StatusCode::PIN_SUSPENDED:
			return 1;

		case StatusCode::PIN_BLOCKED:
		case StatusCode::PIN_DEACTIVATED:
			return 0;

		default:
			return -1;
	}
}


ResponseApdu::operator QByteArray() const
{
	if (mStatusCode == EMPTY)
	{
		return QByteArray();
	}

	return mData + getStatusBytes();
}


#include "moc_ResponseApdu.cpp"
