/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "paos/retrieve/DidAuthenticateEac2Parser.h"

#include "paos/invoke/PaosCreator.h"
#include "paos/retrieve/DidAuthenticateEac2.h"

#include <QDebug>


using namespace governikus;


DidAuthenticateEac2Parser::DidAuthenticateEac2Parser()
	: PaosParser(QStringLiteral("DIDAuthenticate"))
{
}


PaosMessage* DidAuthenticateEac2Parser::parseMessage()
{
	mDidAuthenticateEac2.reset(new DIDAuthenticateEAC2());

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
				mDidAuthenticateEac2->setConnectionHandle(parseConnectionHandle());
			}
		}
		else if (name == QLatin1String("DIDName"))
		{
			if (readUniqueElementText(didName))
			{
				mDidAuthenticateEac2->setDidName(didName);
			}
		}
		else if (name == QLatin1String("AuthenticationProtocolData"))
		{
			const auto value = getElementType();
			if (value.endsWith(QLatin1String("EAC2InputType")))
			{
				mDidAuthenticateEac2->setEac2InputType(parseEac2InputType());
			}
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << name;
			skipCurrentElement();
		}
	}

	return parserFailed() ? nullptr : mDidAuthenticateEac2.release();
}


Eac2InputType DidAuthenticateEac2Parser::parseEac2InputType()
{
	Eac2InputType eac2;

	QString ephemeralPublicKey;
	QString signature;
	while (readNextStartElement())
	{
		const auto& name = getElementName();
		if (name == QLatin1String("Certificate"))
		{
			parseCertificate(eac2);
		}
		else if (name == QLatin1String("EphemeralPublicKey"))
		{
			parseEphemeralPublicKey(eac2, ephemeralPublicKey);
		}
		else if (name == QLatin1String("Signature"))
		{
			parseSignature(eac2, signature);
		}
		else
		{
			qCWarning(paos) << "Unknown element:" << name;
			skipCurrentElement();
		}
	}

	assertMandatoryElement(eac2.getEphemeralPublicKey(), "EphemeralPublicKey");

	return eac2;
}


void DidAuthenticateEac2Parser::parseCertificate(Eac2InputType& pEac2)
{
	const QByteArray hexCvc = readElementText().toLatin1();
	if (auto cvc = CVCertificate::fromRaw(QByteArray::fromHex(hexCvc)))
	{
		pEac2.appendCvcert(cvc);
	}
	else
	{
		qCCritical(paos) << "Cannot parse Certificate";
		setParserFailed();
	}
}


void DidAuthenticateEac2Parser::parseEphemeralPublicKey(Eac2InputType& pEac2, QString& pEphemeralPublicKey)
{
	if (readUniqueElementText(pEphemeralPublicKey))
	{
		pEac2.setEphemeralPublicKey(pEphemeralPublicKey);
	}
}


void DidAuthenticateEac2Parser::parseSignature(Eac2InputType& pEac2, QString& pSignature)
{
	if (readUniqueElementText(pSignature))
	{
		pEac2.setSignature(pSignature);
	}
}
