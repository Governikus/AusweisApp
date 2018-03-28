/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "PinModifyOutput.h"

#include <QLoggingCategory>
#include <QtEndian>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


PinModifyOutput::PinModifyOutput()
	: mResponseApdu()
{
}


PinModifyOutput::PinModifyOutput(const ResponseApdu& pResponseApdu)
	: mResponseApdu(pResponseApdu)
{
}


CardReturnCode PinModifyOutput::statusCodeToCardReturnCode(const QByteArray& pData)
{
	if (pData.size() != 2)
	{
		return CardReturnCode::UNKNOWN;
	}

	const quint16 statusCode = qFromBigEndian<quint16>(pData.data());
	const StatusCode errorCode = Enum<StatusCode>::isValue(statusCode) ? StatusCode(statusCode) : StatusCode::INVALID;
	switch (errorCode)
	{
		case StatusCode::INPUT_TIMEOUT:
			return CardReturnCode::INPUT_TIME_OUT;

		case StatusCode::INPUT_CANCELLED:
			return CardReturnCode::CANCELLATION_BY_USER;

		case StatusCode::PASSWORDS_DIFFER:
			return CardReturnCode::NEW_PIN_MISMATCH;

		case StatusCode::PASSWORD_OUTOF_RANGE:
			return CardReturnCode::NEW_PIN_INVALID_LENGTH;

		case StatusCode::INVALID_DATAFIELD:
			return CardReturnCode::COMMAND_FAILED;

		case StatusCode::ACCESS_DENIED:
			return CardReturnCode::UNKNOWN;

		case StatusCode::SUCCESS:
			return CardReturnCode::OK;

		default:
			qCDebug(card) << "unknown error:" << pData.toHex();
			return CardReturnCode::UNKNOWN;
	}
}


QByteArray PinModifyOutput::toCcid() const
{
	return mResponseApdu.getBuffer();
}


CardReturnCode PinModifyOutput::getReturnCode() const
{
	return statusCodeToCardReturnCode(mResponseApdu.getBuffer());
}


const ResponseApdu& PinModifyOutput::getResponseApdu() const
{
	return mResponseApdu;
}
