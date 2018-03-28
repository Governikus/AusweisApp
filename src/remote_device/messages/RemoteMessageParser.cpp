/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */


#include "RemoteMessageParser.h"

#include "messages/Discovery.h"
#include "messages/GetIfdStatus.h"
#include "messages/IfdConnect.h"
#include "messages/IfdConnectResponse.h"
#include "messages/IfdDisconnect.h"
#include "messages/IfdDisconnectResponse.h"
#include "messages/IfdError.h"
#include "messages/IfdEstablishContext.h"
#include "messages/IfdEstablishContextResponse.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdEstablishPaceChannelResponse.h"
#include "messages/IfdModifyPin.h"
#include "messages/IfdModifyPinResponse.h"
#include "messages/IfdStatus.h"
#include "messages/IfdTransmit.h"
#include "messages/IfdTransmitResponse.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


static QStringList parseArrayOfString(const QJsonArray& pArray)
{
	QStringList result;
	for (const QJsonValue& v : pArray)
	{
		if (!v.isString())
		{
			qCWarning(remote_device) << "Expected string but found" << v.type();
			continue;
		}
		result += v.toString();
	}

	return result;
}


static QSharedPointer<const RemoteMessage> fail(const QString& pLogMessage)
{
	qCWarning(remote_device) << pLogMessage;

	return QSharedPointer<RemoteMessage>();
}


static QSharedPointer<const RemoteMessage> parseMessage(const QJsonObject& pJsonObject)
{
	const RemoteMessage remoteMessage(pJsonObject);
	const RemoteCardMessageType messageType = remoteMessage.getType();
	switch (messageType)
	{
		case RemoteCardMessageType::IFDEstablishContext:
			return QSharedPointer<RemoteMessage>(new IfdEstablishContext(pJsonObject));

		case RemoteCardMessageType::IFDEstablishContextResponse:
			return QSharedPointer<RemoteMessage>(new IfdEstablishContextResponse(pJsonObject));

		case RemoteCardMessageType::IFDGetStatus:
			return QSharedPointer<RemoteMessage>(new GetIfdStatus(pJsonObject));

		case RemoteCardMessageType::IFDConnect:
			return QSharedPointer<RemoteMessage>(new IfdConnect(pJsonObject));

		case RemoteCardMessageType::IFDDisconnect:
			return QSharedPointer<RemoteMessage>(new IfdDisconnect(pJsonObject));

		case RemoteCardMessageType::IFDError:
			return QSharedPointer<RemoteMessage>(new IfdError(pJsonObject));

		case RemoteCardMessageType::IFDTransmit:
			return QSharedPointer<RemoteMessage>(new IfdTransmit(pJsonObject));

		case RemoteCardMessageType::IFDStatus:
			return QSharedPointer<RemoteMessage>(new IfdStatus(pJsonObject));

		case RemoteCardMessageType::IFDConnectResponse:
			return QSharedPointer<RemoteMessage>(new IfdConnectResponse(pJsonObject));

		case RemoteCardMessageType::IFDDisconnectResponse:
			return QSharedPointer<RemoteMessage>(new IfdDisconnectResponse(pJsonObject));

		case RemoteCardMessageType::IFDTransmitResponse:
			return QSharedPointer<RemoteMessage>(new IfdTransmitResponse(pJsonObject));

		case RemoteCardMessageType::IFDEstablishPACEChannel:
			return QSharedPointer<RemoteMessage>(new IfdEstablishPaceChannel(pJsonObject));

		case RemoteCardMessageType::IFDEstablishPACEChannelResponse:
			return QSharedPointer<RemoteMessage>(new IfdEstablishPaceChannelResponse(pJsonObject));

		case RemoteCardMessageType::IFDModifyPIN:
			return QSharedPointer<RemoteMessage>(new IfdModifyPin(pJsonObject));

		case RemoteCardMessageType::IFDModifyPINResponse:
			return QSharedPointer<RemoteMessage>(new IfdModifyPinResponse(pJsonObject));

		case RemoteCardMessageType::UNDEFINED:
			return fail(QStringLiteral("Unknown RemoteMessage received"));
	}

	Q_UNREACHABLE();
}


RemoteMessageParser::RemoteMessageParser()
{
}


RemoteMessageParser::~RemoteMessageParser()
{
}


QSharedPointer<const Discovery> RemoteMessageParser::parseDiscovery(const QJsonDocument& pJsonDocument) const
{
	const QJsonObject& rootObject = pJsonDocument.object();
	if (rootObject.isEmpty())
	{
		qCWarning(remote_device) << "Expected object at top level";
		return QSharedPointer<const Discovery>();
	}

	const QJsonValue& ifdNameValue = rootObject.value(QLatin1String("IFDName"));
	if (!ifdNameValue.isString())
	{
		qCWarning(remote_device) << "The value of \"IFDName\" should be of type string";
		return QSharedPointer<const Discovery>();
	}
	const QString& idfName = ifdNameValue.toString();

	const QJsonValue& ifdIdValue = rootObject.value(QLatin1String("IFDID"));
	if (!ifdIdValue.isString())
	{
		qCWarning(remote_device) << "The value of \"IFDID\" should be of type string";
		return QSharedPointer<const Discovery>();
	}
	const QString& ifdId = ifdIdValue.toString();

	const QJsonValue& msgValue = rootObject.value(QLatin1String("msg"));
	if (!msgValue.isString())
	{
		qCWarning(remote_device) << "The value of \"msg\" should be of type string";
		return QSharedPointer<const Discovery>();
	}
	else if (msgValue.toString() != QLatin1String("REMOTE_IFD"))
	{
		qCWarning(remote_device) << "The value of \"msg\" should be \"REMOTE_IFD\"";
		return QSharedPointer<const Discovery>();
	}
	// This field is not saved in the C++ object.

	const QJsonValue& portValue = rootObject.value(QLatin1String("port"));
	if (!portValue.isDouble())
	{
		qCWarning(remote_device) << "The value of \"port\" should be of type double";
		return QSharedPointer<const Discovery>();
	}
	const long portLong = portValue.toInt();
	if (portLong <= 0 || portLong > static_cast<long>(USHRT_MAX))
	{
		qCWarning(remote_device) << "Expected unsigned integer number (16 bit) for port number";
		return QSharedPointer<const Discovery>();
	}
	const quint16 port = static_cast<quint16>(portLong);

	const QJsonValue& supportedApisValue = rootObject.value(QLatin1String("SupportedAPI"));
	if (!supportedApisValue.isArray())
	{
		qCWarning(remote_device) << "The value of \"availableApiLevels\" should be of type string array";
		return QSharedPointer<const Discovery>();
	}
	const QStringList& availableApiLevelStrings = parseArrayOfString(supportedApisValue.toArray());

	QVector<IfdVersion::Version> availableApiLevels;
	for (const QString& levelString : availableApiLevelStrings)
	{
		const IfdVersion& level = IfdVersion::fromString(levelString);
		if (level.isValid())
		{
			availableApiLevels += level.getVersion();
		}
	}

	return QSharedPointer<Discovery>(new Discovery(idfName, ifdId, port, availableApiLevels));
}


QSharedPointer<const RemoteMessage> RemoteMessageParser::parse(const QByteArray& pJsonData) const
{
	QJsonParseError error;
	const QJsonDocument& doc = QJsonDocument::fromJson(pJsonData, &error);
	if (error.error != QJsonParseError::NoError)
	{
		qCWarning(remote_device) << pJsonData;

		return fail(QStringLiteral("Json parsing failed. Error at offset %1: %2").arg(error.offset).arg(error.errorString()));
	}

	return parse(doc);
}


QSharedPointer<const RemoteMessage> RemoteMessageParser::parse(const QJsonDocument& pJsonDocument) const
{
	const QJsonObject& rootObject = pJsonDocument.object();
	if (rootObject.isEmpty())
	{
		return fail(QStringLiteral("Expected object at top level"));
	}

	if (rootObject.contains(QStringLiteral("msg")))
	{
		return parseMessage(rootObject);
	}
	else
	{
		return fail(QStringLiteral("Root object has no messageType"));
	}
}
