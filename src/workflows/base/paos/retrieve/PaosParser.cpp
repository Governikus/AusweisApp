/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "PaosParser.h"

#include "DidAuthenticateParser.h"
#include "InitializeFramework.h"
#include "StartPaosResponse.h"
#include "TransmitParser.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(paos)


std::unique_ptr<PaosMessage> PaosParser::parseMessage()
{
	if (mParser->getElementName() == QLatin1String("InitializeFramework"))
	{
		return std::make_unique<InitializeFramework>();
	}
	else if (mParser->getElementName() == QLatin1String("DIDAuthenticate"))
	{
		return DidAuthenticateParser(mParser).parse();
	}
	else if (mParser->getElementName() == QLatin1String("Transmit"))
	{
		return TransmitParser(mParser).parse();
	}
	else if (mParser->getElementName() == QLatin1String("StartPAOSResponse"))
	{
		return std::make_unique<StartPaosResponse>(mParser);
	}

	return nullptr;
}


std::unique_ptr<PaosMessage> PaosParser::parseEnvelope()
{
	std::unique_ptr<PaosMessage> message;

	while (mParser->readNextStartElement())
	{
		const auto& name = mParser->getElementName();
		if (name == QLatin1String("Body"))
		{
			if (mParser->assertNoDuplicateElement(message == nullptr))
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
			mParser->skipCurrentElement();
		}
	}

	if (!mParser->parserFailed() && message == nullptr)
	{
		qCWarning(paos) << "Element Body not found";
	}

	return message;
}


void PaosParser::parseHeader()
{
	while (mParser->readNextStartElement())
	{
		const auto& name = mParser->getElementName();
		if (name == QLatin1String("MessageID"))
		{
			mMessageID = mParser->readElementText();
		}
		else if (name == QLatin1String("RelatesTo"))
		{
			mRelatesTo = mParser->readElementText();
		}
		else
		{
			mParser->skipCurrentElement();
		}
	}
}


std::unique_ptr<PaosMessage> PaosParser::parseBody()
{
	const QStringList expectedElements({
				QStringLiteral("InitializeFramework"),
				QStringLiteral("DIDAuthenticate"),
				QStringLiteral("Transmit"),
				QStringLiteral("StartPAOSResponse")
			});

	std::unique_ptr<PaosMessage> message;

	while (mParser->readNextStartElement())
	{
		if (expectedElements.contains(mParser->getElementName()))
		{
			if (mParser->assertNoDuplicateElement(message == nullptr))
			{
				message = parseMessage();
			}
		}
		else
		{
			mParser->skipCurrentElement();
		}
	}

	if (!mParser->parserFailed() && message == nullptr)
	{
		qCWarning(paos) << "No valid Body element found";
	}

	return message;
}


QSharedPointer<ElementParser> PaosParser::getParser()
{
	return mParser;
}


PaosParser::PaosParser()
	: mParser()
	, mMessageID()
	, mRelatesTo()
{
}


PaosParser::~PaosParser() = default;


std::unique_ptr<PaosMessage> PaosParser::parse(const QSharedPointer<ElementParser>& pParser)
{
	std::unique_ptr<PaosMessage> message;
	mParser = pParser;
	while (mParser->readNextStartElement())
	{
		if (mParser->getElementName() == QLatin1String("Envelope"))
		{
			if (!mParser->assertNoDuplicateElement(message == nullptr))
			{
				qCWarning(paos) << "Duplicate Envelope element";
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
			mParser->skipCurrentElement();
		}
	}

	if (mParser->parserFailed())
	{
		message = nullptr;
	}

	if (message == nullptr)
	{
		return nullptr;
	}
	message->setMessageId(mMessageID);
	message->setRelatesTo(mRelatesTo);
	return message;
}
