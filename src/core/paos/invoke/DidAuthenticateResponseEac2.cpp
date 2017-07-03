/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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


QDomElement DIDAuthenticateResponseEAC2::createDIDAuthenticateResponseEAC2Element()
{
	QDomElement element = mDoc.createElement(QStringLiteral("DIDAuthenticateResponse"));
	element.setAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::TECHSCHEMA));
	element.setAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	element.appendChild(createResultElement());
	element.appendChild(createAuthenticationProtocolDataElement());

	return element;
}


QDomElement DIDAuthenticateResponseEAC2::createAuthenticationProtocolDataElement()
{
	QDomElement element = mDoc.createElement(QStringLiteral("AuthenticationProtocolData"));

	element.setAttribute(getNamespacePrefix(Namespace::XSI, QStringLiteral("type")), getNamespaceType(Namespace::TECHSCHEMA, QStringLiteral("EAC2OutputType")));
	element.setAttribute(QStringLiteral("Protocol"), QStringLiteral("urn:oid:1.3.162.15480.3.0.14.2"));

	if (!mEfCardSecurity.isNull())
	{
		element.appendChild(createTextElement(QStringLiteral("EFCardSecurity"), mEfCardSecurity));
	}
	if (!mAuthenticationToken.isNull())
	{
		element.appendChild(createTextElement(QStringLiteral("AuthenticationToken"), mAuthenticationToken));
	}
	if (!mNonce.isNull())
	{
		element.appendChild(createTextElement(QStringLiteral("Nonce"), mNonce));
	}
	if (!mChallenge.isNull())
	{
		element.appendChild(createTextElement(QStringLiteral("Challenge"), mChallenge));
	}
	return element;
}


QDomElement DIDAuthenticateResponseEAC2::getDocumentStructure()
{
	return createEnvelopeElement(createDIDAuthenticateResponseEAC2Element(), getRelatesTo(), getMessageId());
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
