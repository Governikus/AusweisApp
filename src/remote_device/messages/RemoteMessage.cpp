/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */


#include "RemoteMessage.h"

#include "Initializer.h"

#ifndef QT_NO_DEBUG
	#include <QCoreApplication>
#endif
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


namespace
{
VALUE_NAME(MSG_TYPE, "msg")
VALUE_NAME(CONTEXT_HANDLE, "ContextHandle")
} // namespace


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
		Q_ASSERT(!pContextHandle.isEmpty() || mMessageType == RemoteCardMessageType::IFDError || mMessageType == RemoteCardMessageType::IFDEstablishContextResponse);
	}

	messageBody[CONTEXT_HANDLE()] = pContextHandle;
	return messageBody;
}


QByteArray RemoteMessage::toByteArray(const QJsonObject& pJsonObject)
{
#ifndef QT_NO_DEBUG
	if (QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		return QJsonDocument(pJsonObject).toJson(QJsonDocument::Indented);
	}
#endif

	return QJsonDocument(pJsonObject).toJson(QJsonDocument::Compact);
}


void RemoteMessage::markIncomplete(const QString& pLogMessage)
{
	Q_ASSERT(!pLogMessage.isEmpty());

	qCCritical(remote_device) << pLogMessage;
	mIncomplete = true;
}


void RemoteMessage::missingValue(const QLatin1String& pName)
{
	qCCritical(remote_device) << "Missing value" << pName;
	mIncomplete = true;
}


void RemoteMessage::invalidType(const QLatin1String& pName, const QLatin1String& pExpectedType)
{
	qCCritical(remote_device) << "The value of" << pName << "should be of type" << pExpectedType;
	mIncomplete = true;
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
	: mIncomplete(false)
	, mMessageType(pMessageType)
	, mContextHandle()
{
}


RemoteMessage::RemoteMessage(const QJsonObject& pMessageObject)
	: mIncomplete(false)
	, mMessageType(RemoteCardMessageType::UNDEFINED)
	, mContextHandle()
{
	const QString& msgType = getStringValue(pMessageObject, MSG_TYPE());
	mMessageType = Enum<RemoteCardMessageType>::fromString(msgType, RemoteCardMessageType::UNDEFINED);

	if (mMessageType == RemoteCardMessageType::UNDEFINED)
	{
		qCWarning(remote_device) << "Invalid messageType received:" << msgType;
		mIncomplete = true;
	}

	if (mMessageType != RemoteCardMessageType::IFDEstablishContext)
	{
		mContextHandle = getStringValue(pMessageObject, CONTEXT_HANDLE());
	}
}


bool RemoteMessage::isIncomplete() const
{
	return mIncomplete;
}


RemoteCardMessageType RemoteMessage::getType() const
{
	return mMessageType;
}


const QString& RemoteMessage::getContextHandle() const
{
	return mContextHandle;
}


QByteArray RemoteMessage::toByteArray(const QString& pContextHandle) const
{
	Q_ASSERT(false);

	qCCritical(remote_device) << "Unable to convert an untyped RemoteMessage to json:" << pContextHandle;
	return QByteArray();
}


#include "moc_RemoteMessage.cpp"
