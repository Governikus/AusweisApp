/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "paos/retrieve/DidAuthenticateEacAdditionalParser.h"

#include "paos/invoke/PaosCreator.h"
#include "paos/retrieve/DidAuthenticateEacAdditional.h"

#include <QDebug>
#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(paos)


DidAuthenticateEacAdditionalParser::DidAuthenticateEacAdditionalParser()
	: PaosParser(QStringLiteral("DIDAuthenticate"))
{
}


PaosMessage* DidAuthenticateEacAdditionalParser::parseMessage()
{
	mDidAuthenticateEacAdditional.reset(new DIDAuthenticateEACAdditional());

	bool isConnectionHandleNotSet = true;
	QString didName;

	while (readNextStartElement())
	{
		const auto& name = getElementName();
		if (name == QLatin1String("ConnectionHandle"))
		{
			if (assertNoDuplicateElement(isConnectionHandleNotSet))
			{
				isConnectionHandleNotSet = false;
				mDidAuthenticateEacAdditional->setConnectionHandle(parseConnectionHandle());
			}
		}
		else if (name == QLatin1String("DIDName"))
		{
			if (readUniqueElementText(didName))
			{
				mDidAuthenticateEacAdditional->setDidName(didName);
			}
		}
		else if (name == QLatin1String("AuthenticationProtocolData"))
		{
			const auto value = getElementType();
			if (value.endsWith(QLatin1String("EACAdditionalInputType")))
			{
				mDidAuthenticateEacAdditional->setSignature(parseEacAdditionalInputType());
			}
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << name;
			skipCurrentElement();
		}
	}

	return parserFailed() ? nullptr : mDidAuthenticateEacAdditional.release();
}


QString DidAuthenticateEacAdditionalParser::parseEacAdditionalInputType()
{
	QString signature;
	while (readNextStartElement())
	{
		const auto& name = getElementName();
		if (name == QLatin1String("Signature"))
		{
			if (!readUniqueElementText(signature))
			{
				qCWarning(paos) << "Abort parsing of Signature";
				setParserFailed();
			}
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << name;
			skipCurrentElement();
		}
	}

	assertMandatoryElement(signature, "Signature");
	return signature;
}
