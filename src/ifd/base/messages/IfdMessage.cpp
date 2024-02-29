/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */


#include "IfdMessage.h"

#include "Initializer.h"

#ifndef QT_NO_DEBUG
	#include <QCoreApplication>
#endif
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


namespace
{
VALUE_NAME(MSG_TYPE, "msg")
VALUE_NAME(CONTEXT_HANDLE, "ContextHandle")
} // namespace


INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<const IfdMessage>>("QSharedPointer<const IfdMessage>");
		})


QJsonObject IfdMessage::createMessageBody(const QString& pContextHandle) const
{
	QJsonObject messageBody;
	messageBody[MSG_TYPE()] = getEnumName(mMessageType);
	if (mMessageType == IfdMessageType::IFDEstablishContext)
	{
		return messageBody;
	}
	else
	{
		Q_ASSERT(!pContextHandle.isEmpty() || mMessageType == IfdMessageType::IFDError || mMessageType == IfdMessageType::IFDEstablishContextResponse);
	}

	messageBody[CONTEXT_HANDLE()] = pContextHandle;
	return messageBody;
}


QByteArray IfdMessage::toByteArray(const QJsonObject& pJsonObject)
{
#ifndef QT_NO_DEBUG
	if (QCoreApplication::applicationName().startsWith(QLatin1String("Test")))
	{
		return QJsonDocument(pJsonObject).toJson(QJsonDocument::Indented);
	}
#endif

	return QJsonDocument(pJsonObject).toJson(QJsonDocument::Compact);
}


void IfdMessage::markIncomplete(const QString& pLogMessage)
{
	Q_ASSERT(!pLogMessage.isEmpty());

	qCCritical(ifd) << pLogMessage;
	mIncomplete = true;
}


void IfdMessage::missingValue(const QLatin1String& pName)
{
	qCCritical(ifd) << "Missing value" << pName;
	mIncomplete = true;
}


void IfdMessage::invalidType(const QLatin1String& pName, const QLatin1String& pExpectedType)
{
	qCCritical(ifd) << "The value of" << pName << "should be of type" << pExpectedType;
	mIncomplete = true;
}


bool IfdMessage::getBoolValue(const QJsonObject& pJsonObject, const QLatin1String& pName)
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


int IfdMessage::getIntValue(const QJsonObject& pJsonObject, const QLatin1String& pName, int pDefault)
{
	if (pJsonObject.contains(pName))
	{
		const auto& value = pJsonObject.value(pName);
		if (value.isDouble())
		{
			return value.toInt();
		}

		invalidType(pName, QLatin1String("number"));
		return pDefault;
	}

	missingValue(pName);
	return pDefault;
}


QString IfdMessage::getStringValue(const QJsonObject& pJsonObject, const QLatin1String& pName)
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


QJsonObject IfdMessage::parseByteArray(const QByteArray& pMessage)
{
	QJsonParseError error {};
	const QJsonDocument& doc = QJsonDocument::fromJson(pMessage, &error);
	if (error.error != QJsonParseError::NoError)
	{
		qCWarning(ifd) << "Json parsing failed." << error.offset << ":" << error.errorString();
	}

	const QJsonObject& obj = doc.object();
	if (obj.isEmpty())
	{
		qCWarning(ifd) << "Expected object at top level";
	}

	return obj;
}


IfdMessage::IfdMessage(IfdMessageType pMessageType)
	: mIncomplete(false)
	, mMessageType(pMessageType)
	, mContextHandle()
{
}


IfdMessage::IfdMessage(const QJsonObject& pMessageObject)
	: mIncomplete(false)
	, mMessageType(IfdMessageType::UNDEFINED)
	, mContextHandle()
{
	const QString& msgType = getStringValue(pMessageObject, MSG_TYPE());
	mMessageType = Enum<IfdMessageType>::fromString(msgType, IfdMessageType::UNDEFINED);

	if (mMessageType == IfdMessageType::UNDEFINED)
	{
		qCWarning(ifd) << "Invalid messageType received:" << msgType;
		mIncomplete = true;
	}

	if (mMessageType != IfdMessageType::IFDEstablishContext)
	{
		mContextHandle = getStringValue(pMessageObject, CONTEXT_HANDLE());
	}
}


bool IfdMessage::isIncomplete() const
{
	return mIncomplete;
}


IfdMessageType IfdMessage::getType() const
{
	return mMessageType;
}


const QString& IfdMessage::getContextHandle() const
{
	return mContextHandle;
}


QByteArray IfdMessage::toByteArray(IfdVersion::Version, const QString&) const
{
	Q_ASSERT(false);

	qCCritical(ifd) << "Unable to convert an untyped IfdMessage to json";
	return QByteArray();
}


#include "moc_IfdMessage.cpp"
