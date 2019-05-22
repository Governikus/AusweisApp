/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "IfdEstablishPaceChannelResponse.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(OUTPUT_DATA, "OutputData")
} // namespace


IfdEstablishPaceChannelResponse::IfdEstablishPaceChannelResponse(const QString& pSlotHandle, const QByteArray& pOutputData, ECardApiResult::Minor pResultMinor)
	: RemoteMessageResponse(RemoteCardMessageType::IFDEstablishPACEChannelResponse, pResultMinor)
	, mSlotHandle(pSlotHandle)
	, mOutputData(pOutputData)
{
}


IfdEstablishPaceChannelResponse::IfdEstablishPaceChannelResponse(const QJsonObject& pMessageObject)
	: RemoteMessageResponse(pMessageObject)
	, mSlotHandle()
	, mOutputData()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	const QString& inputData = getStringValue(pMessageObject, OUTPUT_DATA());
	mOutputData = QByteArray::fromHex(inputData.toUtf8());

	if (getType() != RemoteCardMessageType::IFDEstablishPACEChannelResponse)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDEstablishPACEChannelResponse"));
	}
}


const QString& IfdEstablishPaceChannelResponse::getSlotHandle() const
{
	return mSlotHandle;
}


const QByteArray& IfdEstablishPaceChannelResponse::getOutputData() const
{
	return mOutputData;
}


QByteArray IfdEstablishPaceChannelResponse::toByteArray(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;
	result[OUTPUT_DATA()] = QString::fromLatin1(mOutputData.toHex());

	return RemoteMessage::toByteArray(result);
}
