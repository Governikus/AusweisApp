/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */


#include "IfdConnect.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(SLOT_NAME, "SlotName")
VALUE_NAME(EXCLUSIVE, "exclusive")
} // namespace


IfdConnect::IfdConnect(const QString& pSlotName, bool pExclusive)
	: IfdMessage(IfdMessageType::IFDConnect)
	, mSlotName(pSlotName)
	, mExclusive(pExclusive)
{
}


IfdConnect::IfdConnect(const QJsonObject& pMessageObject)
	: IfdMessage(pMessageObject)
	, mSlotName()
	, mExclusive(true)
{
	mSlotName = getStringValue(pMessageObject, SLOT_NAME());
	mExclusive = getBoolValue(pMessageObject, EXCLUSIVE());

	if (getType() != IfdMessageType::IFDConnect)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDConnect"));
	}
}


const QString& IfdConnect::getSlotName() const
{
	return mSlotName;
}


bool IfdConnect::isExclusive() const
{
	return mExclusive;
}


QByteArray IfdConnect::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[SLOT_NAME()] = mSlotName;
	result[EXCLUSIVE()] = mExclusive;

	return IfdMessage::toByteArray(result);
}
