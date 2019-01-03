/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "PaosParser.h"

#include <QDebug>

using namespace governikus;

PaosParser::PaosParser(const QString& pMessageName)
	: ElementParser(QSharedPointer<QXmlStreamReader>::create())
	, mMessageName(pMessageName)
	, mMessageID()
	, mRelatesTo()
{
}


PaosParser::~PaosParser()
{
}


PaosMessage* PaosParser::parse(const QByteArray& pXmlData)
{
	PaosMessage* message = nullptr;

	mXmlReader->addData(pXmlData);

	while (readNextStartElement())
	{
		if (mXmlReader->name() == QLatin1String("Envelope"))
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
			mXmlReader->skipCurrentElement();
		}
	}

	if (mParseError)
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


PaosMessage* PaosParser::parseEnvelope()
{
	PaosMessage* message = nullptr;

	while (readNextStartElement())
	{
		if (mXmlReader->name() == QLatin1String("Body"))
		{
			if (assertNoDuplicateElement(message == nullptr))
			{
				message = parseBody();
			}
		}
		else if (mXmlReader->name() == QLatin1String("Header"))
		{
			parseHeader();
		}
		else
		{
			mXmlReader->skipCurrentElement();
		}
	}

	if (!mParseError && message == nullptr)
	{
		qCWarning(paos) << "Element Body not found";
	}

	return message;
}


void PaosParser::parseHeader()
{
	while (readNextStartElement())
	{
		if (mXmlReader->name() == QLatin1String("MessageID"))
		{
			mMessageID = readElementText();
		}
		else if (mXmlReader->name() == QLatin1String("RelatesTo"))
		{
			mRelatesTo = readElementText();
		}
		else
		{
			mXmlReader->skipCurrentElement();
		}
	}
}


PaosMessage* PaosParser::parseBody()
{
	PaosMessage* message = nullptr;

	while (readNextStartElement())
	{
		if (mXmlReader->name() == mMessageName)
		{
			if (assertNoDuplicateElement(message == nullptr))
			{
				message = parseMessage();
			}
		}
		else
		{
			mXmlReader->skipCurrentElement();
		}
	}

	if (!mParseError && message == nullptr)
	{
		qCWarning(paos) << "Element" << mMessageName << "not found";
	}

	return message;
}
