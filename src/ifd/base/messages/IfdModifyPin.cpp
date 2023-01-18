/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "IfdModifyPin.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_HANDLE, "SlotHandle")
VALUE_NAME(INPUT_DATA, "InputData")
} // namespace


IfdModifyPin::IfdModifyPin(const QString& pSlotHandle, const QByteArray& pInputData)
	: IfdMessage(IfdMessageType::IFDModifyPIN)
	, mSlotHandle(pSlotHandle)
	, mInputData(pInputData)
{
}


IfdModifyPin::IfdModifyPin(const QJsonObject& pMessageObject)
	: IfdMessage(pMessageObject)
	, mSlotHandle()
	, mInputData()
{
	mSlotHandle = getStringValue(pMessageObject, SLOT_HANDLE());

	const QString& inputData = getStringValue(pMessageObject, INPUT_DATA());
	mInputData = QByteArray::fromHex(inputData.toUtf8());

	if (getType() != IfdMessageType::IFDModifyPIN)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDModifyPIN"));
	}
}


bool IfdModifyPin::isValid() const
{
	return !mSlotHandle.isEmpty() && !mInputData.isEmpty();
}


const QString& IfdModifyPin::getSlotHandle() const
{
	return mSlotHandle;
}


const QByteArray& IfdModifyPin::getInputData() const
{
	return mInputData;
}


QByteArray IfdModifyPin::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_HANDLE()] = mSlotHandle;
	result[INPUT_DATA()] = QString::fromLatin1(mInputData.toHex());

	return IfdMessage::toByteArray(result);
}
