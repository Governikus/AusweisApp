/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "paos/retrieve/DidAuthenticateEac2.h"
#include "paos/retrieve/DidAuthenticateEac2Parser.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(paos)


Eac2InputType DidAuthenticateEac2Parser::parseEac2InputType()
{
	Eac2InputType eac2;

	QString ephemeralPublicKey;
	QString signature;

	while (mParser->readNextStartElement())
	{
		const auto& name = mParser->getElementName();
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
			mParser->skipCurrentElement();
		}
	}

	mParser->assertMandatoryElement(eac2.getEphemeralPublicKey(), "EphemeralPublicKey");

	return eac2;
}


void DidAuthenticateEac2Parser::parseCertificate(Eac2InputType& pEac2)
{
	const QByteArray hexCvc = mParser->readElementText().toLatin1();
	if (auto cvc = CVCertificate::fromRaw(QByteArray::fromHex(hexCvc)))
	{
		pEac2.appendCvcert(cvc);
	}
	else
	{
		qCCritical(paos) << "Cannot parse Certificate";
		mParser->setParserFailed();
	}
}


void DidAuthenticateEac2Parser::parseEphemeralPublicKey(Eac2InputType& pEac2, QString& pEphemeralPublicKey)
{
	if (mParser->readUniqueElementText(pEphemeralPublicKey))
	{
		pEac2.setEphemeralPublicKey(pEphemeralPublicKey);
	}
}


void DidAuthenticateEac2Parser::parseSignature(Eac2InputType& pEac2, QString& pSignature)
{
	if (mParser->readUniqueElementText(pSignature))
	{
		pEac2.setSignature(pSignature);
	}
}


DidAuthenticateEac2Parser::DidAuthenticateEac2Parser(const QSharedPointer<ElementParser>& pParser)
	: mParser(pParser)
{
}


std::unique_ptr<DIDAuthenticateEAC2> DidAuthenticateEac2Parser::parse()
{
	auto didAuthenticateEac2 = std::make_unique<DIDAuthenticateEAC2>();
	didAuthenticateEac2->setEac2InputType(parseEac2InputType());
	return mParser->parserFailed() ? nullptr : std::move(didAuthenticateEac2);
}
