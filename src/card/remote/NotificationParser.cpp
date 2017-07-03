/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */


#include "NotificationParser.h"

#include <QJsonArray>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(card_remote)

namespace governikus
{

class NotificationParserPrivate
{
	private:
		QVector<int> parseArrayOfInt(const QJsonArray& pArray) const;

		bool containsInvalidHexDigit(const QString& pString) const;

		QSharedPointer<const RemoteCardNotification> fail(const QString& pLogMessage) const;

		QSharedPointer<const RemoteCardNotification> parseRemoteReaderOffer(const QJsonObject& pJsonObject) const;

		QSharedPointer<const RemoteCardNotification> parseSetApiLevel(const QJsonObject& pJsonObject) const;

		QSharedPointer<const RemoteCardNotification> parseApiLevel(const QJsonObject& pJsonObject) const;

		CardDescription parseCardDescription(const QJsonValue& pJsonValue, bool& pResultOk) const;

		ReaderDescription parseReaderDescription(const QJsonObject& pJsonObject) const;

		QSharedPointer<const RemoteCardNotification> parseReaderList(const QJsonObject& pJsonObject) const;

		QSharedPointer<const RemoteCardNotification> parseReader(const QJsonObject& pJsonObject) const;

		QSharedPointer<const RemoteCardNotification> parseConnect(const QJsonObject& pJsonObject) const;

		QSharedPointer<const RemoteCardNotification> parseDisconnect(const QJsonObject& pJsonObject) const;

		QSharedPointer<const RemoteCardNotification> parseTransmit(const QJsonObject& pJsonObject) const;

		QSharedPointer<const RemoteCardNotification> parseTransmitResponse(const QJsonObject& pJsonObject) const;

		QSharedPointer<const RemoteCardNotification> parseUnsupported(const QJsonObject& pJsonObject) const;

		QSharedPointer<const RemoteCardNotification> parseCommand(const QJsonObject& pJsonObject) const;

		QSharedPointer<const RemoteCardNotification> parseMessage(const QJsonObject& pJsonObject) const;

	public:
		NotificationParserPrivate() = default;

		~NotificationParserPrivate() = default;

		QSharedPointer<const RemoteCardNotification> parse(const QByteArray& pJsonData) const;

		QSharedPointer<const RemoteCardNotification> parse(const QJsonDocument& pJsonDocument) const;
};


QVector<int> NotificationParserPrivate::parseArrayOfInt(const QJsonArray& pArray) const
{
	QVector<int> result;
	for (const QJsonValue& v : pArray)
	{
		if (!v.isDouble())
		{
			qCWarning(card_remote) << "Expected number but found" << v.type();

			continue;
		}

		result += v.toInt();
	}

	return result;
}


bool NotificationParserPrivate::containsInvalidHexDigit(const QString& pString) const
{
	static const QChar A('A');
	static const QChar F('F');

	const QString& uppercase = pString.toUpper();
	for (const QChar& c : uppercase)
	{
		if (c.isDigit() || (A <= c && c <= F))
		{
			continue;
		}
		else
		{
			return true;
		}
	}

	return false;
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::fail(const QString& pLogMessage) const
{
	qCWarning(card_remote) << pLogMessage;

	return QSharedPointer<RemoteCardNotification>();
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parseRemoteReaderOffer(const QJsonObject& pJsonObject) const
{
	static const QLatin1String DEVICE_NAME("deviceName");
	static const QLatin1String ENCRYPTED("encrypted");
	static const QLatin1String PORT("port");
	static const QLatin1String AVAILABLE_API_LEVELS("availableApiLevels");

	const QJsonValue& deviceNameValue = pJsonObject.value(DEVICE_NAME);
	if (!deviceNameValue.isString())
	{
		return fail(QStringLiteral("The value of \"deviceName\" should be of type string"));
	}
	const QString& deviceName = deviceNameValue.toString();

	const QJsonValue& encryptedValue = pJsonObject.value(ENCRYPTED);
	if (!encryptedValue.isBool())
	{
		return fail(QStringLiteral("The value of \"encrypted\" should be of type boolean"));
	}
	const bool encrypted = encryptedValue.toBool();

	const QJsonValue& portValue = pJsonObject.value(PORT);
	if (!portValue.isDouble())
	{
		return fail(QStringLiteral("The value of \"port\" should be of type double"));
	}
	const long portLong = portValue.toInt();
	if (portLong <= 0 || portLong > static_cast<long>(USHRT_MAX))
	{
		return fail(QStringLiteral("Expected unsigned integer number (16 bit) for port number"));
	}
	const quint16 port = static_cast<quint16>(portLong);

	const QJsonValue& availableApiLevelsValue = pJsonObject.value(AVAILABLE_API_LEVELS);
	if (!availableApiLevelsValue.isArray())
	{
		return fail(QStringLiteral("The value of \"availableApiLevels\" should be of type number array"));
	}
	const QVector<int>& availableApiLevels = parseArrayOfInt(availableApiLevelsValue.toArray());

	return QSharedPointer<RemoteCardNotification>(new RemoteReaderOfferMsg(deviceName, encrypted, port, availableApiLevels));
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parseSetApiLevel(const QJsonObject& pJsonObject) const
{
	static const QLatin1String LEVEL("level");

	const QJsonValue& levelValue = pJsonObject.value(LEVEL);
	if (!levelValue.isDouble())
	{
		return fail(QStringLiteral("The value of \"level\" should be of type double"));
	}
	const int level = levelValue.toInt();
	if (level <= 0)
	{
		return fail(QStringLiteral("Expected non-negative integer number for level"));
	}

	return QSharedPointer<RemoteCardNotification>(new SetApiLevelCmd(level));
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parseApiLevel(const QJsonObject& pJsonObject) const
{
	static const QLatin1String ERROR("error");
	static const QLatin1String AVAILABLE("available");
	static const QLatin1String CURRENT("current");

	const QJsonValue& errorValue = pJsonObject.value(ERROR);
	if (!errorValue.isString())
	{
		return fail(QStringLiteral("The value of \"error\" should be of type string"));
	}
	const QString& error = errorValue.toString();

	const QJsonValue& availableValue = pJsonObject.value(AVAILABLE);
	if (!availableValue.isArray())
	{
		return fail(QStringLiteral("The value of \"available\" should be of type number array"));
	}
	const QVector<int>& available = parseArrayOfInt(availableValue.toArray());

	const QJsonValue& currentValue = pJsonObject.value(CURRENT);
	if (!currentValue.isDouble())
	{
		return fail(QStringLiteral("The value of \"current\" should be of type integer"));
	}
	const int current = currentValue.toInt();
	if (current <= 0)
	{
		return fail(QStringLiteral("Expected non-negative integer number for level"));
	}

	return QSharedPointer<RemoteCardNotification>(new ApiLevelMsg(error, available, current));
}


CardDescription NotificationParserPrivate::parseCardDescription(const QJsonValue& pJsonValue, bool& pResultOk) const
{
	static const QLatin1String EF_CARD_ACCESS("EFCardAccess");
	static const QLatin1String RETRY_COUNTER("retryCounter");
	static const QLatin1String PIN_DEACTIVATED("pinDeactivated");
	static const QLatin1String CONNECTED("connected");

	pResultOk = false;

	// No card inserted: null
	if (pJsonValue.isNull())
	{
		pResultOk = true;

		return CardDescription();
	}

	// Not null: must be an object.
	if (!pJsonValue.isObject())
	{
		qCWarning(card_remote) << "A card must be either null or an object";

		return CardDescription();
	}

	const QJsonObject& jsonObject = pJsonValue.toObject();
	const QJsonValue& efCardAccessValue = jsonObject.value(EF_CARD_ACCESS);
	if (!efCardAccessValue.isString())
	{
		qCWarning(card_remote) << "The value of \"EFCardAccess\" should be of type string";

		return CardDescription();
	}
	const QString& efCardAccessString = efCardAccessValue.toString();
	if (containsInvalidHexDigit(efCardAccessString))
	{
		qCWarning(card_remote) << "The value of \"EFCardAccess\" should only contain hexadecimal digits";

		return CardDescription();
	}
	const QByteArray& efCardAccess = QByteArray::fromHex(efCardAccessString.toUtf8());

	const QJsonValue& retryCounterValue = jsonObject.value(RETRY_COUNTER);
	if (!retryCounterValue.isDouble())
	{
		qCWarning(card_remote) << "The value of \"retryCounter\" should be of type integer";

		return CardDescription();
	}
	const int retryCounter = retryCounterValue.toInt();

	const QJsonValue& pinDeactivatedValue = jsonObject.value(PIN_DEACTIVATED);
	if (!pinDeactivatedValue.isBool())
	{
		qCWarning(card_remote) << "The value of \"pinDeactivated\" should be of type boolean";

		return CardDescription();
	}
	const int pinDeactivated = pinDeactivatedValue.toBool();

	const QJsonValue& connectedValue = jsonObject.value(CONNECTED);
	if (!connectedValue.isBool())
	{
		qCWarning(card_remote) << "The value of \"connected\" should be of type boolean";

		return CardDescription();
	}
	const int connected = connectedValue.toBool();

	pResultOk = true;

	return CardDescription(efCardAccess, retryCounter, pinDeactivated, connected);
}


ReaderDescription NotificationParserPrivate::parseReaderDescription(const QJsonObject& pJsonObject) const
{
	static const QLatin1String NAME("name");
	static const QLatin1String ATTACHED("attached");
	static const QLatin1String EXTENDED_LENGTH("extendedLength");
	static const QLatin1String CARD("card");
	bool parseCardOk = false;

	const QJsonValue& nameValue = pJsonObject.value(NAME);
	if (!nameValue.isString())
	{
		qCWarning(card_remote) << "The value of \"name\" should be of type string";

		return ReaderDescription();
	}
	const QString& name = nameValue.toString();

	const QJsonValue& attachedValue = pJsonObject.value(ATTACHED);
	if (!attachedValue.isBool())
	{
		qCWarning(card_remote) << "The value of \"attached\" should be of type boolean";

		return ReaderDescription();
	}
	const bool attached = attachedValue.toBool();

	const QJsonValue& extendedLengthValue = pJsonObject.value(EXTENDED_LENGTH);
	if (!extendedLengthValue.isString())
	{
		qCWarning(card_remote) << "The value of \"extendedLength\" should be of type string";

		return ReaderDescription();
	}
	const ExtendedLengthApduSupportCode extendedLength =
			Enum<ExtendedLengthApduSupportCode>::fromString(extendedLengthValue.toString(),
			ExtendedLengthApduSupportCode::INVALID);
	if (extendedLength == ExtendedLengthApduSupportCode::INVALID)
	{
		qCWarning(card_remote) << "The value of \"extendedLength\" should be \"UNKNOWN\", \"SUPPORTED\" or \"UNSUPPORTED\"";

		return ReaderDescription();
	}

	const QJsonValue& cardValue = pJsonObject.value(CARD);
	const CardDescription& card = parseCardDescription(cardValue, parseCardOk);
	if (parseCardOk)
	{
		return ReaderDescription(name, attached, extendedLength, card);
	}
	else
	{
		qCWarning(card_remote) << "Cannot parse card description";

		return ReaderDescription();
	}
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parseReaderList(const QJsonObject& pJsonObject) const
{
	static const QLatin1String READERS("readers");

	const QJsonValue& readersValue = pJsonObject.value(QLatin1String(READERS));
	if (!readersValue.isArray())
	{
		return fail(QStringLiteral("The value of \"readers\" should be an array"));
	}
	const QJsonArray& readersArray = readersValue.toArray();

	QVector<ReaderDescription> readers;
	for (const QJsonValue readerValue : readersArray)
	{
		if (!readerValue.isObject())
		{
			return fail(QStringLiteral("The \"readers\" should contain only objects"));
		}

		const ReaderDescription& reader = parseReaderDescription(readerValue.toObject());
		if (reader.getExtendedLength() == ExtendedLengthApduSupportCode::INVALID)
		{
			return fail(QStringLiteral("Cannot parse an element of \"readers\""));
		}

		readers += reader;
	}

	return QSharedPointer<RemoteCardNotification>(new ReaderListMsg(readers));
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parseReader(const QJsonObject& pJsonObject) const
{
	static const QLatin1String READER("reader");
	static const QLatin1String ERROR("error");

	const QJsonValue& readerValue = pJsonObject.value(READER);
	if (!readerValue.isObject())
	{
		return fail(QStringLiteral("The value of \"reader\" should be of type object"));
	}
	const ReaderDescription& reader = parseReaderDescription(readerValue.toObject());
	if (reader.getExtendedLength() == ExtendedLengthApduSupportCode::INVALID)
	{
		return fail(QStringLiteral("Cannot parse \"reader\" object"));
	}

	const QJsonValue& errorValue = pJsonObject.value(ERROR);
	if (!errorValue.isString())
	{
		return fail(QStringLiteral("The value of \"error\" should be of type string"));
	}
	const QString& error = errorValue.toString();

	return QSharedPointer<RemoteCardNotification>(new ReaderMsg(reader, error));
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parseConnect(const QJsonObject& pJsonObject) const
{
	static const QLatin1String READER_NAME("readerName");

	const QJsonValue& readerNameValue = pJsonObject.value(READER_NAME);
	if (!readerNameValue.isString())
	{
		return fail(QStringLiteral("The value of \"readerName\" should be of type string"));
	}
	const QString& readerName = readerNameValue.toString();

	return QSharedPointer<RemoteCardNotification>(new ConnectCmd(readerName));
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parseDisconnect(const QJsonObject& pJsonObject) const
{
	static const QLatin1String READER_NAME("readerName");

	const QJsonValue& readerNameValue = pJsonObject.value(READER_NAME);
	if (!readerNameValue.isString())
	{
		return fail(QStringLiteral("The value of \"readerName\" should be of type string"));
	}
	const QString& readerName = readerNameValue.toString();

	return QSharedPointer<RemoteCardNotification>(new DisconnectCmd(readerName));
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parseTransmit(const QJsonObject& pJsonObject) const
{
	static const QLatin1String READER_NAME("readerName");
	static const QLatin1String COMMAND_APDU("commandApdu");

	const QJsonValue& readerNameValue = pJsonObject.value(READER_NAME);
	if (!readerNameValue.isString())
	{
		return fail(QStringLiteral("The value of \"readerName\" should be of type string"));
	}
	const QString& readerName = readerNameValue.toString();

	const QJsonValue& commandApduValue = pJsonObject.value(COMMAND_APDU);
	if (!commandApduValue.isString())
	{
		return fail(QStringLiteral("The value of \"commandApdu\" should be of type string"));
	}
	const QString& commandApduString = commandApduValue.toString();
	if (containsInvalidHexDigit(commandApduString))
	{
		return fail(QStringLiteral("The value of \"commandApdu\" should only contain hexadecimal digits"));
	}
	const QByteArray& commandApdu = QByteArray::fromHex(commandApduString.toUtf8());

	return QSharedPointer<RemoteCardNotification>(new TransmitCmd(readerName, commandApdu));
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parseTransmitResponse(const QJsonObject& pJsonObject) const
{
	static const QLatin1String READER_NAME("readerName");
	static const QLatin1String ERROR("error");
	static const QLatin1String RESPONSE_APDU("responseApdu");

	const QJsonValue& readerNameValue = pJsonObject.value(READER_NAME);
	if (!readerNameValue.isString())
	{
		return fail(QStringLiteral("The value of \"readerName\" should be of type string"));
	}
	const QString& readerName = readerNameValue.toString();

	const QJsonValue& errorValue = pJsonObject.value(ERROR);
	if (!errorValue.isString())
	{
		return fail(QStringLiteral("The value of \"error\" should be of type string"));
	}
	const QString& error = errorValue.toString();

	const QJsonValue& responseApduValue = pJsonObject.value(RESPONSE_APDU);
	if (!responseApduValue.isString())
	{
		return fail(QStringLiteral("The value of \"responseApdu\" should be of type string"));
	}
	const QString& responseApduString = responseApduValue.toString();
	if (containsInvalidHexDigit(responseApduString))
	{
		return fail(QStringLiteral("The value of \"responseApdu\" should only contain hexadecimal digits"));
	}
	const QByteArray& responseApdu = QByteArray::fromHex(responseApduString.toUtf8());

	return QSharedPointer<RemoteCardNotification>(new TransmitResponseMsg(readerName, error, responseApdu));
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parseUnsupported(const QJsonObject& pJsonObject) const
{
	static const QLatin1String ERROR("error");

	const QJsonValue& errorValue = pJsonObject.value(ERROR);
	if (!errorValue.isString())
	{
		return fail(QStringLiteral("The value of \"error\" should be of type string"));
	}
	const QString& error = errorValue.toString();

	return QSharedPointer<RemoteCardNotification>(new UnsupportedMsg(error));
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parseCommand(const QJsonObject& pJsonObject) const
{
	static const QString CMD("cmd");
	static const QSharedPointer<const RemoteCardNotification> sRemoteReaderDiscoverCmd(new RemoteReaderDiscoverCmd());
	static const QSharedPointer<const RemoteCardNotification> sGetApiLevelCmd(new GetApiLevelCmd());
	static const QSharedPointer<const RemoteCardNotification> sGetReaderListCmd(new GetReaderListCmd());

	const QJsonValue& cmdValue = pJsonObject.value(CMD);
	if (!cmdValue.isString())
	{
		return fail(QStringLiteral("The value of \"cmd\" should be of type string"));
	}

	const QString& cmd = cmdValue.toString();
	const CardNotificationType notificationType = Enum<CardNotificationType>::fromString(cmd, CardNotificationType::UNSUPPORTED);
	switch (notificationType)
	{
		case CardNotificationType::REMOTE_READER_DISCOVER:
			return sRemoteReaderDiscoverCmd;

		case CardNotificationType::GET_API_LEVEL:
			return sGetApiLevelCmd;

		case CardNotificationType::SET_API_LEVEL:
			return parseSetApiLevel(pJsonObject);

		case CardNotificationType::GET_READER_LIST:
			return sGetReaderListCmd;

		case CardNotificationType::CONNECT:
			return parseConnect(pJsonObject);

		case CardNotificationType::DISCONNECT:
			return parseDisconnect(pJsonObject);

		case CardNotificationType::TRANSMIT:
			return parseTransmit(pJsonObject);

		default:
			return fail(QStringLiteral("Unknown command: %1").arg(cmd));
	}
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parseMessage(const QJsonObject& pJsonObject) const
{
	static const QString MSG("msg");

	const QJsonValue& msgValue = pJsonObject.value(MSG);
	if (!msgValue.isString())
	{
		return fail(QStringLiteral("The value of \"msg\" should be of type string"));
	}

	const QString& msg = msgValue.toString();
	const CardNotificationType notificationType = Enum<CardNotificationType>::fromString(msg, CardNotificationType::UNSUPPORTED);
	switch (notificationType)
	{
		case CardNotificationType::REMOTE_READER_OFFER:
			return parseRemoteReaderOffer(pJsonObject);

		case CardNotificationType::API_LEVEL:
			return parseApiLevel(pJsonObject);

		case CardNotificationType::READER_LIST:
			return parseReaderList(pJsonObject);

		case CardNotificationType::READER:
			return parseReader(pJsonObject);

		case CardNotificationType::TRANSMIT_RESPONSE:
			return parseTransmitResponse(pJsonObject);

		case CardNotificationType::UNSUPPORTED:
			return parseUnsupported(pJsonObject);

		default:
			return fail(QStringLiteral("Unknown message: %1").arg(msg));
	}
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parse(const QByteArray& pJsonData) const
{
	QJsonParseError error;
	const QJsonDocument& doc = QJsonDocument::fromJson(pJsonData, &error);
	if (error.error != QJsonParseError::NoError)
	{
		qCWarning(card_remote) << pJsonData;

		return fail(QStringLiteral("Json parsing failed. Error at offset %1: %2").arg(error.offset).arg(error.errorString()));
	}

	return parse(doc);
}


QSharedPointer<const RemoteCardNotification> NotificationParserPrivate::parse(const QJsonDocument& pJsonDocument) const
{
	const QJsonObject& rootObject = pJsonDocument.object();
	if (rootObject.isEmpty())
	{
		return fail(QStringLiteral("Expected object at top level"));
	}

	if (rootObject.contains(QStringLiteral("cmd")))
	{
		return parseCommand(rootObject);
	}
	else if (rootObject.contains(QStringLiteral("msg")))
	{
		return parseMessage(rootObject);
	}
	else
	{
		return fail(QStringLiteral("Root object has neither type cmd nor msg"));
	}
}


} /* namespace governikus */


using namespace governikus;


NotificationParser::NotificationParser()
	: d_ptr(new NotificationParserPrivate())
{
}


NotificationParser::~NotificationParser()
{
}


QSharedPointer<const RemoteCardNotification> NotificationParser::parse(const QByteArray& pJsonData) const
{
	Q_D(const NotificationParser);

	return d->parse(pJsonData);
}


QSharedPointer<const RemoteCardNotification> NotificationParser::parse(const QJsonDocument& pJsonDocument) const
{
	Q_D(const NotificationParser);

	return d->parse(pJsonDocument);
}
