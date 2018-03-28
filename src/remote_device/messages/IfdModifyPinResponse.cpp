/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "IfdModifyPinResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(OUTPUT_DATA, "OutputData")
}


IfdModifyPinResponse::IfdModifyPinResponse(const QString& pSlotHandle, const QByteArray& pOutputData, const QString& pResultMinor)
	: RemoteMessageResponse(RemoteCardMessageType::IFDModifyPINResponse, pResultMinor)
	, mSlotHandle(pSlotHandle)
	, mOutputData(pOutputData)
{
}


IfdModifyPinResponse::IfdModifyPinResponse(const QJsonObject& pMessageObject)
	: RemoteMessageResponse(pMessageObject)
	, mSlotHandle()
	, mOutputData()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	const QString& inputData = getStringValue(pMessageObject, OUTPUT_DATA());
	mOutputData = QByteArray::fromHex(inputData.toUtf8());
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

	const auto& minor = getResultMinor();
	if (minor == QLatin1String("http://www.bsi.bund.de/ecard/api/1.1/resultminor/ifdl/common#timeoutError"))
	{
		return CardReturnCode::INPUT_TIME_OUT;
	}
	if (minor == QLatin1String("http://www.bsi.bund.de/ecard/api/1.1/resultminor/ifdl#cancellationByUser"))
	{
		return CardReturnCode::CANCELLATION_BY_USER;
	}
	if (minor == QLatin1String("http://www.bsi.bund.de/ecard/api/1.1/resultminor/ifdl/IO#repeatedDataMismatch"))
	{
		return CardReturnCode::NEW_PIN_MISMATCH;
	}
	if (minor == QLatin1String("http://www.bsi.bund.de/ecard/api/1.1/resultminor/ifdl/IO#unknownPINFormat"))
	{
		return CardReturnCode::NEW_PIN_INVALID_LENGTH;
	}
	if (minor == QLatin1String("http://www.bsi.bund.de/ecard/api/1.1/resultminor/al/common#unknownError"))
	{
		return CardReturnCode::UNKNOWN;
	}

	return CardReturnCode::COMMAND_FAILED;
}


QJsonDocument IfdModifyPinResponse::toJson(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;
	result[OUTPUT_DATA()] = QString::fromLatin1(mOutputData.toHex());

	return QJsonDocument(result);
}
