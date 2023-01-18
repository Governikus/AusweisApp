/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "IfdModifyPinResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(OUTPUT_DATA, "OutputData")
} // namespace


IfdModifyPinResponse::IfdModifyPinResponse(const QString& pSlotHandle, const QByteArray& pOutputData, ECardApiResult::Minor pResultMinor)
	: IfdMessageResponse(IfdMessageType::IFDModifyPINResponse, pResultMinor)
	, mSlotHandle(pSlotHandle)
	, mOutputData(pOutputData)
{
}


IfdModifyPinResponse::IfdModifyPinResponse(const QJsonObject& pMessageObject)
	: IfdMessageResponse(pMessageObject)
	, mSlotHandle()
	, mOutputData()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	const QString& inputData = getStringValue(pMessageObject, OUTPUT_DATA());
	mOutputData = QByteArray::fromHex(inputData.toUtf8());

	if (getType() != IfdMessageType::IFDModifyPINResponse)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDModifyPINResponse"));
	}
}


const QString& IfdModifyPinResponse::getSlotHandle() const
{
	return mSlotHandle;
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

		case ECardApiResult::Minor::IFDL_IO_RepeatedDataMismatch:
			return CardReturnCode::NEW_PIN_MISMATCH;

		case ECardApiResult::Minor::IFDL_IO_UnknownPINFormat:
			return CardReturnCode::NEW_PIN_INVALID_LENGTH;

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

	result[SLOT_HANDLE()] = mSlotHandle;
	result[OUTPUT_DATA()] = QString::fromLatin1(mOutputData.toHex());

	return IfdMessage::toByteArray(result);
}
