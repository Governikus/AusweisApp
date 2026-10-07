/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "DidAuthenticateEac1.h"


using namespace governikus;


void DIDAuthenticateEAC1::setEac1InputType(const Eac1InputType& eac1InputType)
{
	mEac1InputType = eac1InputType;
}


DIDAuthenticateEAC1::DIDAuthenticateEAC1()
	: DidAuthenticateMessage(PaosType::DID_AUTHENTICATE_EAC1)
{
}


DIDAuthenticateEAC1::~DIDAuthenticateEAC1() = default;


const QSharedPointer<const AuthenticatedAuxiliaryData>& DIDAuthenticateEAC1::getAuthenticatedAuxiliaryData() const
{
	return mEac1InputType.getAuthenticatedAuxiliaryData();
}


const QByteArray& DIDAuthenticateEAC1::getAuthenticatedAuxiliaryDataAsBinary() const
{
	return mEac1InputType.getAuthenticatedAuxiliaryDataAsBinary();
}


const QSharedPointer<const CertificateDescription>& DIDAuthenticateEAC1::getCertificateDescription() const
{
	return mEac1InputType.getCertificateDescription();
}


const QByteArray& DIDAuthenticateEAC1::getCertificateDescriptionAsBinary() const
{
	return mEac1InputType.getCertificateDescriptionAsBinary();
}


const QList<QSharedPointer<const CVCertificate>>& DIDAuthenticateEAC1::getCvCertificates() const
{
	return mEac1InputType.getCvCertificates();
}


QList<QSharedPointer<const CVCertificate>> DIDAuthenticateEAC1::getCvCertificates(const QList<AccessRole>& pAccessRoles) const
{
	const auto& certificates = mEac1InputType.getCvCertificates();
	QList<QSharedPointer<const CVCertificate>> cvcs;
	for (const auto& cvc : certificates)
	{
		const auto& cvcAccessRole = cvc->getBody().getCHAT().getAccessRole();
		if (pAccessRoles.contains(cvcAccessRole))
		{
			cvcs << cvc;
		}
	}

	return cvcs;
}


const QSharedPointer<const CHAT>& DIDAuthenticateEAC1::getOptionalChat() const
{
	return mEac1InputType.getOptionalChat();
}


const QSharedPointer<const CHAT>& DIDAuthenticateEAC1::getRequiredChat() const
{
	return mEac1InputType.getRequiredChat();
}


const QString& DIDAuthenticateEAC1::getTransactionInfo() const
{
	return mEac1InputType.getTransactionInfo();
}


const QList<AcceptedEidType>& DIDAuthenticateEAC1::getAcceptedEidTypes() const
{
	return mEac1InputType.getAcceptedEidTypes();
}
