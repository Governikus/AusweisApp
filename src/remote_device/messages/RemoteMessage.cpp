/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */


#include "RemoteMessage.h"

#include "Initializer.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(MSG_TYPE, "msg")
VALUE_NAME(CONTEXT_HANDLE, "ContextHandle")
}


static Initializer::Entry E([] {
			qRegisterMetaType<QSharedPointer<const RemoteMessage> >("QSharedPointer<const RemoteMessage>");
		});


QJsonObject RemoteMessage::createMessageBody(const QString& pContextHandle) const
{
	QJsonObject messageBody;
	messageBody[MSG_TYPE()] = getEnumName(mMessageType);
	if (mMessageType == RemoteCardMessageType::IFDEstablishContext)
	{
		return messageBody;
	}
	else
	{
		Q_ASSERT(!pContextHandle.isEmpty() || mMessageType == RemoteCardMessageType::IFDError);
	}

	messageBody[CONTEXT_HANDLE()] = pContextHandle;
	return messageBody;
}


void RemoteMessage::missingValue(const QLatin1String& pName)
{
	qCCritical(remote_device) << "Missing value" << pName;
	mIsValid = false;
}


void RemoteMessage::invalidType(const QLatin1String& pName, const QLatin1String& pExpectedType)
{
	qCCritical(remote_device) << "The value of" << pName << "should be of type" << pExpectedType;
	mIsValid = false;
}


bool RemoteMessage::getBoolValue(const QJsonObject& pJsonObject, const QLatin1String& pName)
{
	if (pJsonObject.contains(pName))
	{
		const auto& value = pJsonObject.value(pName);
		if (value.isBool())
		{
			return value.toBool();
		}

		invalidType(pName, QLatin1String("boolean"));
		return false;
	}

	missingValue(pName);
	return false;
}


int RemoteMessage::getIntValue(const QJsonObject& pJsonObject, const QLatin1String& pName)
{
	if (pJsonObject.contains(pName))
	{
		const auto& value = pJsonObject.value(pName);
		if (value.isDouble())
		{
			return value.toInt();
		}

		invalidType(pName, QLatin1String("number"));
		return 0;
	}

	missingValue(pName);
	return 0;
}


QString RemoteMessage::getStringValue(const QJsonObject& pJsonObject, const QLatin1String& pName)
{
	if (pJsonObject.contains(pName))
	{
		const auto& value = pJsonObject.value(pName);
		if (value.isString())
		{
			return value.toString();
		}

		invalidType(pName, QLatin1String("string"));
		return QString();
	}

	missingValue(pName);
	return QString();
}


QJsonObject RemoteMessage::parseByteArray(const QByteArray& pMessage)
{
	QJsonParseError error;
	const QJsonDocument& doc = QJsonDocument::fromJson(pMessage, &error);
	if (error.error != QJsonParseError::NoError)
	{
		qCWarning(remote_device) << "Json parsing failed." << error.offset << ":" << error.errorString();
	}

	const QJsonObject& obj = doc.object();
	if (obj.isEmpty())
	{
		qCWarning(remote_device) << "Expected object at top level";
	}

	return obj;
}


RemoteMessage::RemoteMessage(RemoteCardMessageType pMessageType)
	: mIsValid(true)
	, mMessageType(pMessageType)
	, mContextHandle()
{
}


RemoteMessage::RemoteMessage(const QJsonObject& pMessageObject)
	: mIsValid(true)
	, mMessageType(RemoteCardMessageType::UNDEFINED)
	, mContextHandle()
{
	const QString& msg = getStringValue(pMessageObject, MSG_TYPE());
	mMessageType = Enum<RemoteCardMessageType>::fromString(msg, RemoteCardMessageType::UNDEFINED);

	if (mMessageType != RemoteCardMessageType::IFDEstablishContext)
	{
		mContextHandle = getStringValue(pMessageObject, CONTEXT_HANDLE());
	}
}


bool RemoteMessage::isValid() const
{
	return mIsValid;
}


RemoteCardMessageType RemoteMessage::getType() const
{
	return mMessageType;
}


const QString& RemoteMessage::getContextHandle() const
{
	return mContextHandle;
}


QJsonDocument RemoteMessage::toJson(const QString& pContextHandle) const
{
	Q_ASSERT(false);

	qCCritical(remote_device) << "Unable to convert an untyped RemoteMessage to json:" << pContextHandle;
	return QJsonDocument();
}


#include "moc_RemoteMessage.cpp"
