/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "paos/retrieve/DidAuthenticateEacAdditional.h"
#include "paos/retrieve/DidAuthenticateEacAdditionalParser.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(paos)


QString DidAuthenticateEacAdditionalParser::parseEacAdditionalInputType()
{
	QString signature;

	while (mParser->readNextStartElement())
	{
		const auto& name = mParser->getElementName();
		if (name == QLatin1String("Signature"))
		{
			if (!mParser->readUniqueElementText(signature))
			{
				qCWarning(paos) << "Abort parsing of Signature";
				mParser->setParserFailed();
			}
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << name;
			mParser->skipCurrentElement();
		}
	}

	mParser->assertMandatoryElement(signature, "Signature");
	return signature;
}


DidAuthenticateEacAdditionalParser::DidAuthenticateEacAdditionalParser(const QSharedPointer<ElementParser>& pParser)
	: mParser(pParser)
{
}


std::unique_ptr<DIDAuthenticateEACAdditional> DidAuthenticateEacAdditionalParser::parse()
{
	auto didAuthenticateEacAdditional = std::make_unique<DIDAuthenticateEACAdditional>();
	didAuthenticateEacAdditional->setSignature(parseEacAdditionalInputType());
	return mParser->parserFailed() ? nullptr : std::move(didAuthenticateEacAdditional);
}
