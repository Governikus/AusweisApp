/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "IfdEstablishPaceChannel.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(INPUT_DATA, "InputData")
} // namespace


IfdEstablishPaceChannel::IfdEstablishPaceChannel(const QString& pSlotHandle, const QByteArray& pInputData)
	: RemoteMessage(RemoteCardMessageType::IFDEstablishPACEChannel)
	, mSlotHandle(pSlotHandle)
	, mInputData(pInputData)
{
}


IfdEstablishPaceChannel::IfdEstablishPaceChannel(const QJsonObject& pMessageObject)
	: RemoteMessage(pMessageObject)
	, mSlotHandle()
	, mInputData()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	const QString& inputData = getStringValue(pMessageObject, INPUT_DATA());
	mInputData = QByteArray::fromHex(inputData.toUtf8());

	if (getType() != RemoteCardMessageType::IFDEstablishPACEChannel)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDEstablishPACEChannel"));
	}
}


const QString& IfdEstablishPaceChannel::getSlotHandle() const
{
	return mSlotHandle;
}


const QByteArray& IfdEstablishPaceChannel::getInputData() const
{
	return mInputData;
}


QByteArray IfdEstablishPaceChannel::toByteArray(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;
	result[INPUT_DATA()] = QString::fromLatin1(mInputData.toHex());

	return RemoteMessage::toByteArray(result);
}
