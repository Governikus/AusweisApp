/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
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
}


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


QJsonDocument IfdEstablishContext::toJson(const QString& pContextHandle) const
{
	QJsonObject result = createMessageBody(pContextHandle);
	result[PROTOCOL()] = mProtocol.toString();
	result[UD_NAME()] = mUdName;
	return QJsonDocument(result);
}
