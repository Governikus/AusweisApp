/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
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
}


IfdEstablishPaceChannelResponse::IfdEstablishPaceChannelResponse(const QString& pSlotHandle, const QByteArray& pOutputData, const QString& pResultMinor)
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
}


const QString& IfdEstablishPaceChannelResponse::getSlotHandle() const
{
	return mSlotHandle;
}


const QByteArray& IfdEstablishPaceChannelResponse::getOutputData() const
{
	return mOutputData;
}


QJsonDocument IfdEstablishPaceChannelResponse::toJson(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;
	result[OUTPUT_DATA()] = QString::fromLatin1(mOutputData.toHex());

	return QJsonDocument(result);
}
