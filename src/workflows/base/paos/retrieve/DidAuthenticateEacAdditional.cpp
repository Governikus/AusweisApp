/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "DidAuthenticateEacAdditional.h"

using namespace governikus;


void DIDAuthenticateEACAdditional::setSignature(const QString& signature)
{
	mSignature = signature;
}


DIDAuthenticateEACAdditional::DIDAuthenticateEACAdditional()
	: DidAuthenticateMessage(PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE)
{
}


DIDAuthenticateEACAdditional::~DIDAuthenticateEACAdditional() = default;


const QString& DIDAuthenticateEACAdditional::getSignature() const
{
	return mSignature;
}
