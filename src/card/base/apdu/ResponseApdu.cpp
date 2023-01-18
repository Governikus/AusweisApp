/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
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


SW1 ResponseApdu::getSW1() const
{
	const auto value = static_cast<quint8>(mStatusCode >> 8);
	if (value != EMPTY && Enum<SW1>::isValue(value))
	{
		return SW1(value);
	}

	qCCritical(card) << "Unknown SW1 value, returning UNKNOWN, value:" << QString::number(value, 16);
	return SW1::UNKNOWN;
}


char ResponseApdu::getSW2() const
{
	return static_cast<char>(mStatusCode & 0xFF);
}


ResponseApdu::operator QByteArray() const
{
	if (mStatusCode == EMPTY)
	{
		return QByteArray();
	}

	return mData + getStatusBytes();
}


#if !defined(QT_NO_DEBUG) && __has_include(<QTest>)
	#include <QTest>
char* governikus::toString(const ResponseApdu& pResponseApdu)
{
	return QTest::toString(QByteArray(pResponseApdu).toHex());
}


#endif


#include "moc_ResponseApdu.cpp"
