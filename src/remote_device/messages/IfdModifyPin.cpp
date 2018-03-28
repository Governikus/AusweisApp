/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "IfdModifyPin.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(INPUT_DATA, "InputData")
}


IfdModifyPin::IfdModifyPin(const QString& pSlotHandle, const QByteArray& pInputData)
	: RemoteMessage(RemoteCardMessageType::IFDModifyPIN)
	, mSlotHandle(pSlotHandle)
	, mInputData(pInputData)
{

}


IfdModifyPin::IfdModifyPin(const QJsonObject& pMessageObject)
	: RemoteMessage(pMessageObject)
	, mSlotHandle()
	, mInputData()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	const QString& inputData = getStringValue(pMessageObject, INPUT_DATA());
	mInputData = QByteArray::fromHex(inputData.toUtf8());
}


const QString& IfdModifyPin::getSlotHandle() const
{
	return mSlotHandle;
}


const QByteArray& IfdModifyPin::getInputData() const
{
	return mInputData;
}


QJsonDocument IfdModifyPin::toJson(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;
	result[INPUT_DATA()] = QString::fromLatin1(mInputData.toHex());

	return QJsonDocument(result);
}
