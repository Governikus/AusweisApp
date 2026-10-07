/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "DidAuthenticateParser.h"

#include "DidAuthenticateEac1Parser.h"
#include "DidAuthenticateEac2Parser.h"
#include "DidAuthenticateEacAdditionalParser.h"
#include "paos/element/ConnectionHandleParser.h"
#include "paos/invoke/PaosCreator.h"


using namespace governikus;


QStringView DidAuthenticateParser::getElementType() const
{
	QString ns = PaosCreator::getNamespace(PaosCreator::Namespace::XSI);
	return mParser->getElementTypeByNamespace(ns);
}


DidAuthenticateParser::DidAuthenticateParser(const QSharedPointer<ElementParser>& pParser)
	: mConnectionHandle()
	, mDidName()
	, mParser(pParser)
{
}


std::unique_ptr<DidAuthenticateMessage> DidAuthenticateParser::parse()
{
	std::unique_ptr<DidAuthenticateMessage> message;
	auto isConnectionHandleNotSet = true;

	while (mParser->readNextStartElement())
	{
		const auto& name = mParser->getElementName();
		if (name == QLatin1String("DIDName"))
		{
			mParser->readUniqueElementText(mDidName);
		}
		else if (name == QLatin1String("ConnectionHandle"))
		{
			if (mParser->assertNoDuplicateElement(isConnectionHandleNotSet))
			{
				isConnectionHandleNotSet = false;
				mConnectionHandle = ConnectionHandleParser(mParser).parse();
			}
		}
		else if (name == QLatin1String("AuthenticationProtocolData"))
		{
			auto type = getElementType();

			if (type.endsWith(QLatin1String("EAC1InputType")))
			{
				message = DidAuthenticateEac1Parser(mParser).parse();
			}
			else if (type.endsWith(QLatin1String("EAC2InputType")))
			{
				message = DidAuthenticateEac2Parser(mParser).parse();
			}
			else if (type.endsWith(QLatin1String("EACAdditionalInputType")))
			{
				message = DidAuthenticateEacAdditionalParser(mParser).parse();
			}

		}
		else
		{
			mParser->skipCurrentElement();
		}
	}
	if (message != nullptr && !isConnectionHandleNotSet)
	{
		message->setDidName(mDidName);
		message->setConnectionHandle(mConnectionHandle);
		qDebug() << "DidAuthenticateParser: " << message->getDidName() << message->getConnectionHandle().getIfdName();
	}

	return message;
}
