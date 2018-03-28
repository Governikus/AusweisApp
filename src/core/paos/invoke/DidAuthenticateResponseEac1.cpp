/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "DidAuthenticateResponseEac1.h"

#include "EstablishPACEChannel.h"
#include "paos/PaosType.h"

using namespace governikus;


DIDAuthenticateResponseEAC1::DIDAuthenticateResponseEAC1()
	: PaosCreator()
	, ResponseType(PaosType::DID_AUTHENTICATE_RESPONSE_EAC1)
	, mCertificateHolderAuthorizationTemplate()
	, mCertificationAuthorityReferences()
	, mEfCardAccess()
	, mIdPICC()
	, mChallenge()
{
}


DIDAuthenticateResponseEAC1::~DIDAuthenticateResponseEAC1()
{
}


QDomElement DIDAuthenticateResponseEAC1::getDocumentStructure()
{
	return createEnvelopeElement(createDIDAuthenticateResponseEAC1Element(), getRelatesTo(), getMessageId());
}


const QByteArray& DIDAuthenticateResponseEAC1::getCertificateHolderAuthorizationTemplate() const
{
	return mCertificateHolderAuthorizationTemplate;
}


const QByteArray& DIDAuthenticateResponseEAC1::getEFCardAccess() const
{
	return mEfCardAccess;
}


const QByteArray& DIDAuthenticateResponseEAC1::getIDPICC() const
{
	return mIdPICC;
}


const QByteArray& DIDAuthenticateResponseEAC1::getChallenge() const
{
	return mChallenge;
}


void DIDAuthenticateResponseEAC1::setCertificateHolderAuthorizationTemplate(const QByteArray& pValue)
{
	mCertificateHolderAuthorizationTemplate = pValue;
}


void DIDAuthenticateResponseEAC1::setEFCardAccess(const QByteArray& pValue)
{
	mEfCardAccess = pValue;
}


void DIDAuthenticateResponseEAC1::setIDPICC(const QByteArray& pValue)
{
	mIdPICC = pValue;
}


void DIDAuthenticateResponseEAC1::setCertificationAuthorityReference(const EstablishPACEChannelOutput& pPaceChannelOutput)
{
	mCertificationAuthorityReferences += pPaceChannelOutput.getCARcurr();
	if (!pPaceChannelOutput.getCARprev().isEmpty())
	{
		mCertificationAuthorityReferences += pPaceChannelOutput.getCARprev();
	}
}


void DIDAuthenticateResponseEAC1::setChallenge(const QByteArray& pValue)
{
	mChallenge = pValue;
}


QDomElement DIDAuthenticateResponseEAC1::createDIDAuthenticateResponseEAC1Element()
{
	QDomElement element = mDoc.createElement(QStringLiteral("DIDAuthenticateResponse"));
	element.setAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::TECHSCHEMA));
	element.setAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	element.appendChild(createResultElement(*this));
	element.appendChild(createAuthenticationProtocolDataElement());

	return element;
}


QDomElement DIDAuthenticateResponseEAC1::createAuthenticationProtocolDataElement()
{
	QDomElement element = mDoc.createElement(QStringLiteral("AuthenticationProtocolData"));

	element.setAttribute(getNamespacePrefix(Namespace::XSI, QStringLiteral("type")), getNamespaceType(Namespace::TECHSCHEMA, QStringLiteral("EAC1OutputType")));
	element.setAttribute(QStringLiteral("Protocol"), QStringLiteral("urn:oid:1.3.162.15480.3.0.14.2"));

	if (!mCertificateHolderAuthorizationTemplate.isNull())
	{
		element.appendChild(createTextElement(QStringLiteral("CertificateHolderAuthorizationTemplate"), mCertificateHolderAuthorizationTemplate));
	}
	for (const auto& reference : qAsConst(mCertificationAuthorityReferences))
	{
		element.appendChild(createTextElement(QStringLiteral("CertificationAuthorityReference"), reference));
	}
	if (!mEfCardAccess.isNull())
	{
		element.appendChild(createTextElement(QStringLiteral("EFCardAccess"), mEfCardAccess));
	}
	if (!mIdPICC.isNull())
	{
		element.appendChild(createTextElement(QStringLiteral("IDPICC"), mIdPICC));
	}
	if (!mChallenge.isNull())
	{
		element.appendChild(createTextElement(QStringLiteral("Challenge"), mChallenge));
	}

	return element;
}


Result DIDAuthenticateResponseEAC1::getResult() const
{
	return ResponseType::getResult();
}
