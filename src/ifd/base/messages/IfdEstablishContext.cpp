/*!
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */


#include "IfdEstablishContext.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(PROTOCOL, "Protocol")
VALUE_NAME(UD_NAME, "UDName")
} // namespace


IfdEstablishContext::IfdEstablishContext(const IfdVersion::Version& pVersion, const QString& pUdName)
	: IfdMessage(IfdMessageType::IFDEstablishContext)
	, mProtocol(IfdVersion(pVersion))
	, mUdName(pUdName)
{
}


IfdEstablishContext::IfdEstablishContext(const QJsonObject& pMessageObject)
	: IfdMessage(pMessageObject)
	, mProtocolRaw(getStringValue(pMessageObject, PROTOCOL()))
	, mProtocol(IfdVersion(mProtocolRaw))
	, mUdName(getStringValue(pMessageObject, UD_NAME()))
{
	if (getType() != IfdMessageType::IFDEstablishContext)
	{
		markIncomplete(QStringLiteral("The value of msg should be IFDEstablishContext"));
	}
}


const IfdVersion& IfdEstablishContext::getProtocol() const
{
	return mProtocol;
}


const QString& IfdEstablishContext::getProtocolRaw() const
{
	return mProtocolRaw;
}


const QString& IfdEstablishContext::getUdName() const
{
	return mUdName;
}


QByteArray IfdEstablishContext::toByteArray(IfdVersion::Version, const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[PROTOCOL()] = mProtocol.toString();
	result[UD_NAME()] = mUdName;

	return IfdMessage::toByteArray(result);
}
