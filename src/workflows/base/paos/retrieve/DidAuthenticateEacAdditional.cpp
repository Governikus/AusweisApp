/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "DidAuthenticateEacAdditional.h"

using namespace governikus;


DIDAuthenticateEACAdditional::DIDAuthenticateEACAdditional()
	: PaosMessage(PaosType::DID_AUTHENTICATE_EAC_ADDITIONAL_INPUT_TYPE)
{
}


DIDAuthenticateEACAdditional::~DIDAuthenticateEACAdditional() = default;


void DIDAuthenticateEACAdditional::setConnectionHandle(const ConnectionHandle& pConnectionHandle)
{
	mConnectionHandle = pConnectionHandle;
}


const QString& DIDAuthenticateEACAdditional::getDidName() const
{
	return mDidName;
}


void DIDAuthenticateEACAdditional::setDidName(const QString& didName)
{
	mDidName = didName;
}


const QString& DIDAuthenticateEACAdditional::getSignature() const
{
	return mSignature;
}


void DIDAuthenticateEACAdditional::setSignature(const QString& signature)
{
	mSignature = signature;
}
