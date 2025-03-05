/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "DidAuthenticateResponseEac2.h"

#include "paos/PaosType.h"

using namespace governikus;

DIDAuthenticateResponseEAC2::DIDAuthenticateResponseEAC2()
	: ResponseType(PaosType::DID_AUTHENTICATE_RESPONSE_EAC2)
	, mEfCardSecurity()
	, mAuthenticationToken()
	, mNonce()
	, mChallenge()
{
}


void DIDAuthenticateResponseEAC2::createBodyElement(QXmlStreamWriter& pWriter)
{
	pWriter.writeStartElement(QStringLiteral("DIDAuthenticateResponse"));
	pWriter.writeAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::TECHSCHEMA));
	pWriter.writeAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	createResultElement(*this);
	createAuthenticationProtocolDataElement(pWriter);

	pWriter.writeEndElement(); // DIDAuthenticateResponse
}


void DIDAuthenticateResponseEAC2::createAuthenticationProtocolDataElement(QXmlStreamWriter& pWriter)
{
	pWriter.writeStartElement(QStringLiteral("AuthenticationProtocolData"));
	pWriter.writeAttribute(getNamespacePrefix(Namespace::XSI, QStringLiteral("type")), getNamespaceType(Namespace::TECHSCHEMA, QStringLiteral("EAC2OutputType")));
	pWriter.writeAttribute(QStringLiteral("Protocol"), QStringLiteral("urn:oid:1.3.162.15480.3.0.14.2"));

	if (!mEfCardSecurity.isNull())
	{
		writeTextElement(QStringLiteral("EFCardSecurity"), mEfCardSecurity);
	}
	if (!mAuthenticationToken.isNull())
	{
		writeTextElement(QStringLiteral("AuthenticationToken"), mAuthenticationToken);
	}
	if (!mNonce.isNull())
	{
		writeTextElement(QStringLiteral("Nonce"), mNonce);
	}
	if (!mChallenge.isNull())
	{
		writeTextElement(QStringLiteral("Challenge"), mChallenge);
	}

	pWriter.writeEndElement(); // AuthenticationProtocolData
}


void DIDAuthenticateResponseEAC2::setAuthenticationToken(const QByteArray& pAuthenticationToken)
{
	mAuthenticationToken = pAuthenticationToken;
}


void DIDAuthenticateResponseEAC2::setEfCardSecurity(const QByteArray& pEfCardSecurity)
{
	mEfCardSecurity = pEfCardSecurity;
}


void DIDAuthenticateResponseEAC2::setNonce(const QByteArray& pNonce)
{
	mNonce = pNonce;
}


void DIDAuthenticateResponseEAC2::setChallenge(const QByteArray& pChallenge)
{
	mChallenge = pChallenge;
}
