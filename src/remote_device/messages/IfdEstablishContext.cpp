/*!
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */


#include "IfdEstablishContext.h"

#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(PROTOCOL, "Protocol")
VALUE_NAME(UD_NAME, "UDName")
} // namespace


IfdEstablishContext::IfdEstablishContext(const IfdVersion& pProtocol, const QString& pUdName)
	: RemoteMessage(RemoteCardMessageType::IFDEstablishContext)
	, mProtocol(pProtocol)
	, mUdName(pUdName)
{
}


IfdEstablishContext::IfdEstablishContext(const QJsonObject& pMessageObject)
	: RemoteMessage(pMessageObject)
	, mProtocolRaw(getStringValue(pMessageObject, PROTOCOL()))
	, mProtocol(IfdVersion(mProtocolRaw))
	, mUdName(getStringValue(pMessageObject, UD_NAME()))
{
	if (getType() != RemoteCardMessageType::IFDEstablishContext)
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


QByteArray IfdEstablishContext::toByteArray(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);

	result[PROTOCOL()] = mProtocol.toString();
	result[UD_NAME()] = mUdName;

	return RemoteMessage::toByteArray(result);
}
