/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "IfdEstablishPaceChannel.h"

#include "MessageReceiver.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(INPUT_DATA, "InputData")
}


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
}


const QString& IfdEstablishPaceChannel::getSlotHandle() const
{
	return mSlotHandle;
}


const QByteArray& IfdEstablishPaceChannel::getInputData() const
{
	return mInputData;
}


QJsonDocument IfdEstablishPaceChannel::toJson(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;
	result[INPUT_DATA()] = QString::fromLatin1(mInputData.toHex());

	return QJsonDocument(result);
}
