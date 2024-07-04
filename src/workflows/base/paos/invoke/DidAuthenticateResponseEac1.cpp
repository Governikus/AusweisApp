/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "DidAuthenticateResponseEac1.h"

#include "paos/PaosType.h"
#include "pinpad/EstablishPaceChannel.h"
#include "pinpad/EstablishPaceChannelOutput.h"

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


void DIDAuthenticateResponseEAC1::setCertificationAuthorityReference(const EstablishPaceChannelOutput& pPaceChannelOutput)
{
	mCertificationAuthorityReferences += pPaceChannelOutput.getCarCurr();
	if (!pPaceChannelOutput.getCarPrev().isEmpty())
	{
		mCertificationAuthorityReferences += pPaceChannelOutput.getCarPrev();
	}
}


void DIDAuthenticateResponseEAC1::setChallenge(const QByteArray& pValue)
{
	mChallenge = pValue;
}


void DIDAuthenticateResponseEAC1::createBodyElement(QXmlStreamWriter& pWriter)
{
	pWriter.writeStartElement(QStringLiteral("DIDAuthenticateResponse"));
	pWriter.writeAttribute(getNamespacePrefix(Namespace::DEFAULT), getNamespace(Namespace::TECHSCHEMA));
	pWriter.writeAttribute(QStringLiteral("Profile"), getNamespace(Namespace::ECARD));

	createResultElement(*this);
	createAuthenticationProtocolDataElement(pWriter);

	pWriter.writeEndElement(); // DIDAuthenticateResponse
}


void DIDAuthenticateResponseEAC1::createAuthenticationProtocolDataElement(QXmlStreamWriter& pWriter)
{
	pWriter.writeStartElement(QStringLiteral("AuthenticationProtocolData"));

	pWriter.writeAttribute(getNamespacePrefix(Namespace::XSI, QStringLiteral("type")), getNamespaceType(Namespace::TECHSCHEMA, QStringLiteral("EAC1OutputType")));
	pWriter.writeAttribute(QStringLiteral("Protocol"), QStringLiteral("urn:oid:1.3.162.15480.3.0.14.2"));

	if (!mCertificateHolderAuthorizationTemplate.isNull())
	{
		writeTextElement(QStringLiteral("CertificateHolderAuthorizationTemplate"), mCertificateHolderAuthorizationTemplate);
	}
	for (const auto& reference : std::as_const(mCertificationAuthorityReferences))
	{
		writeTextElement(QStringLiteral("CertificationAuthorityReference"), reference);
	}

	writeTextElement(QStringLiteral("EFCardAccess"), mEfCardAccess);
	writeTextElement(QStringLiteral("IDPICC"), mIdPICC);
	writeTextElement(QStringLiteral("Challenge"), mChallenge);

	pWriter.writeEndElement(); // AuthenticationProtocolData
}


ECardApiResult DIDAuthenticateResponseEAC1::getResult() const
{
	return ResponseType::getResult();
}
