/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "paos/retrieve/DidAuthenticateEac2.h"

using namespace governikus;


void DIDAuthenticateEAC2::setEac2InputType(const Eac2InputType& pEac2)
{
	mEac2 = pEac2;
}


DIDAuthenticateEAC2::DIDAuthenticateEAC2()
	: DidAuthenticateMessage(PaosType::DID_AUTHENTICATE_EAC2)
{
}


DIDAuthenticateEAC2::~DIDAuthenticateEAC2() = default;


const QString& DIDAuthenticateEAC2::getSignature() const
{
	return mEac2.getSignature();
}


const QString& DIDAuthenticateEAC2::getEphemeralPublicKey() const
{
	return mEac2.getEphemeralPublicKey();
}


const QList<QSharedPointer<const CVCertificate>>& DIDAuthenticateEAC2::getCvCertificates() const
{
	return mEac2.getCvCertificates();
}
