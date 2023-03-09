/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "PaosParser.h"

#include "paos/invoke/PaosCreator.h"

#include <QDebug>

using namespace governikus;

PaosParser::PaosParser(const QString& pMessageName)
	: ElementParser(QSharedPointer<QXmlStreamReader>::create())
	, mMessageName(pMessageName)
	, mMessageID()
	, mRelatesTo()
{
}


PaosParser::~PaosParser() = default;


PaosMessage* PaosParser::parse(const QByteArray& pXmlData)
{
	PaosMessage* message = nullptr;

	initData(pXmlData);

	while (readNextStartElement())
	{
		if (getElementName() == QLatin1String("Envelope"))
		{
			if (!assertNoDuplicateElement(message == nullptr))
			{
				qCWarning(paos) << "Duplicate Envelope element";
				delete message;
				return nullptr;
			}

			message = parseEnvelope();

			if (message == nullptr)
			{
				return nullptr;
			}
		}
		else
		{
			skipCurrentElement();
		}
	}

	if (parserFailed())
	{
		delete message;
		message = nullptr;
	}

	if (message != nullptr)
	{
		message->setMessageId(mMessageID);
		message->setRelatesTo(mRelatesTo);
	}

	return message;
}


QStringView PaosParser::getElementType() const
{
	QString ns = PaosCreator::getNamespace(PaosCreator::Namespace::XSI);
	return getElementTypeByNamespace(ns);
}


PaosMessage* PaosParser::parseEnvelope()
{
	PaosMessage* message = nullptr;

	while (readNextStartElement())
	{
		const auto& name = getElementName();
		if (name == QLatin1String("Body"))
		{
			if (assertNoDuplicateElement(message == nullptr))
			{
				message = parseBody();
			}
		}
		else if (name == QLatin1String("Header"))
		{
			parseHeader();
		}
		else
		{
			skipCurrentElement();
		}
	}

	if (!parserFailed() && message == nullptr)
	{
		qCWarning(paos) << "Element Body not found";
	}

	return message;
}


void PaosParser::parseHeader()
{
	while (readNextStartElement())
	{
		const auto& name = getElementName();
		if (name == QLatin1String("MessageID"))
		{
			mMessageID = readElementText();
		}
		else if (name == QLatin1String("RelatesTo"))
		{
			mRelatesTo = readElementText();
		}
		else
		{
			skipCurrentElement();
		}
	}
}


PaosMessage* PaosParser::parseBody()
{
	PaosMessage* message = nullptr;

	while (readNextStartElement())
	{
		if (getElementName() == mMessageName)
		{
			if (assertNoDuplicateElement(message == nullptr))
			{
				message = parseMessage();
			}
		}
		else
		{
			skipCurrentElement();
		}
	}

	if (!parserFailed() && message == nullptr)
	{
		qCWarning(paos) << "Element" << mMessageName << "not found";
	}

	return message;
}
