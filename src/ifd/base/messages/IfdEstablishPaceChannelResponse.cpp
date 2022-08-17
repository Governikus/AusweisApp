/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "IfdEstablishPaceChannelResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(RESULT_CODE, "ResultCode")
VALUE_NAME(OUTPUT_DATA, "OutputData")
} // namespace


void IfdEstablishPaceChannelResponse::parseOutputData(const QJsonObject& pMessageObject)
{
	const bool v0Supported = IfdVersion(IfdVersion::Version::v0).isSupported();
	const bool v2Received = pMessageObject.contains(RESULT_CODE());

	if (!v0Supported || v2Received)
	{
		const QString& resultCode = getStringValue(pMessageObject, RESULT_CODE());
		if (!isIncomplete() && !mOutputData.parseResultCode(QByteArray::fromHex(resultCode.toUtf8())))
		{
			markIncomplete(QStringLiteral("The value of ResultCode should be as defined in the result value table of [PC/SC], Part 10 AMD1, section 2.5.12"));
		}
	}

	const QString& outputData = getStringValue(pMessageObject, OUTPUT_DATA());
	if (isIncomplete())
	{
		return;
	}

	if (!v0Supported || v2Received)
	{
		if (!mOutputData.parseOutputData(QByteArray::fromHex(outputData.toUtf8())))
		{
			markIncomplete(QStringLiteral("The value of OutputData should be as defined in the result value table of [PC/SC], Part 10 AMD1, section 2.6.16"));
		}

		return;
	}

	if (!mOutputData.parseFromCcid(QByteArray::fromHex(outputData.toUtf8())))
	{
		markIncomplete(QStringLiteral("The value of OutputData should be as defined in TR-03119 section D.3"));
	}
}


IfdEstablishPaceChannelResponse::IfdEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput& pOutputData, ECardApiResult::Minor pResultMinor)
	: IfdMessageResponse(IfdMessageType::IFDEstablishPACEChannelResponse, pResultMinor)
	, mSlotHandle(pSlotHandle)
	, mOutputData(pOutputData)
{
}


IfdEstablishPaceChannelResponse::IfdEstablishPaceChannelResponse(const QJsonObject& pMessageObject)
	: IfdMessageResponse(pMessageObject)
	, mSlotHandle()
	, mOutputData()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	parseOutputData(pMessageObject);

	if (getType() != IfdMessageType::IFDEstablishPACEChannelResponse)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDEstablishPACEChannelResponse"));
	}
}


const QString& IfdEstablishPaceChannelResponse::getSlotHandle() const
{
	return mSlotHandle;
}


const EstablishPaceChannelOutput& IfdEstablishPaceChannelResponse::getOutputData() const
{
	return mOutputData;
}


QByteArray IfdEstablishPaceChannelResponse::toByteArray(IfdVersion::Version pIfdVersion, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;
	if (pIfdVersion >= IfdVersion::Version::v2)
	{
		result[RESULT_CODE()] = QString::fromLatin1(mOutputData.toResultCode().toHex());
		result[OUTPUT_DATA()] = QString::fromLatin1(mOutputData.toOutputData().toHex());
	}
	else
	{
		result[OUTPUT_DATA()] = QString::fromLatin1(mOutputData.toCcid().toHex());
	}

	return IfdMessage::toByteArray(result);
}
