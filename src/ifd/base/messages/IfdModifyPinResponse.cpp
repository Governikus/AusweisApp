/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "IfdModifyPinResponse.h"
#include "apdu/ResponseApdu.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(OUTPUT_DATA, "OutputData")
} // namespace


void IfdModifyPinResponse::ensureOutputData(ECardApiResult::Minor pMinor)
{
	ResponseApdu response;
	switch (pMinor)
	{
		case ECardApiResult::Minor::IFDL_IO_RepeatedDataMismatch:
			response = ResponseApdu(StatusCode::PASSWORDS_DIFFER);
			break;

		case ECardApiResult::Minor::IFDL_IO_UnknownPINFormat:
			response = ResponseApdu(StatusCode::PASSWORD_OUTOF_RANGE);
			break;

		default:
			return;
	}

	clearError();
	if (const QByteArray& outputData = response; mOutputData != outputData)
	{
		qCCritical(ifd).noquote() << "OutputData doesn't match result minor! Expected:" << outputData.toHex() << ", given:" << mOutputData.toHex();
		mOutputData = response;
	}
}


IfdModifyPinResponse::IfdModifyPinResponse(const QString& pSlotHandle, const QByteArray& pOutputData, ECardApiResult::Minor pResultMinor)
	: IfdSlotHandle<IfdMessageResponse>(IfdMessageType::IFDModifyPINResponse, pSlotHandle, pResultMinor)
	, mOutputData(pOutputData)
{
}


IfdModifyPinResponse::IfdModifyPinResponse(const QJsonObject& pMessageObject)
	: IfdSlotHandle<IfdMessageResponse>(pMessageObject)
	, mOutputData()
{
	const QString& outputData = getStringValue(pMessageObject, OUTPUT_DATA());
	mOutputData = QByteArray::fromHex(outputData.toUtf8());

	ensureType(IfdMessageType::IFDModifyPINResponse);
	if (resultHasError())
	{
		ensureOutputData(getResultMinor());
	}
}


const QByteArray& IfdModifyPinResponse::getOutputData() const
{
	return mOutputData;
}


CardReturnCode IfdModifyPinResponse::getReturnCode() const
{
	if (!resultHasError())
	{
		return CardReturnCode::OK;
	}

	switch (getResultMinor())
	{
		case ECardApiResult::Minor::IFDL_Timeout_Error:
			return CardReturnCode::INPUT_TIME_OUT;

		case ECardApiResult::Minor::IFDL_CancellationByUser:
			return CardReturnCode::CANCELLATION_BY_USER;

		case ECardApiResult::Minor::IFDL_Terminal_NoCard:
		case ECardApiResult::Minor::IFDL_InvalidSlotHandle:
			return CardReturnCode::CARD_NOT_FOUND;

		case ECardApiResult::Minor::AL_Unknown_API_Function:
			return CardReturnCode::PROTOCOL_ERROR;

		case ECardApiResult::Minor::AL_Unknown_Error:
			return CardReturnCode::UNKNOWN;

		default:
			return CardReturnCode::COMMAND_FAILED;
	}
}


QByteArray IfdModifyPinResponse::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[OUTPUT_DATA()] = QString::fromLatin1(mOutputData.toHex());

	return IfdMessage::toByteArray(result);
}
