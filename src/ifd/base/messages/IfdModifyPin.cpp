/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "IfdModifyPin.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(INPUT_DATA, "InputData")
} // namespace


IfdModifyPin::IfdModifyPin(const QString& pSlotHandle, const QByteArray& pInputData)
	: IfdSlotHandle<IfdMessage>(IfdMessageType::IFDModifyPIN, pSlotHandle)
	, mInputData(pInputData)
{
}


IfdModifyPin::IfdModifyPin(const QJsonObject& pMessageObject)
	: IfdSlotHandle<IfdMessage>(pMessageObject)
	, mInputData()
{
	const QString& inputData = getStringValue(pMessageObject, INPUT_DATA());
	mInputData = QByteArray::fromHex(inputData.toUtf8());

	ensureType(IfdMessageType::IFDModifyPIN);
}


bool IfdModifyPin::isValid() const
{
	return !getSlotHandle().isEmpty() && !mInputData.isEmpty();
}


const QByteArray& IfdModifyPin::getInputData() const
{
	return mInputData;
}


QByteArray IfdModifyPin::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[INPUT_DATA()] = QString::fromLatin1(mInputData.toHex());

	return IfdMessage::toByteArray(result);
}
