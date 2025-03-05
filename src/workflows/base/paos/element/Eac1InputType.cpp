/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "Eac1InputType.h"

using namespace governikus;

Eac1InputType::Eac1InputType()
	: mCvCertificates(),
	mCertificateDescriptionAsBinary(),
	mCertificateDescription(),
	mRequiredChat(),
	mOptionalChat(),
	mAuthenticatedAuxiliaryDataAsBinary(),
	mAuthenticatedAuxiliaryData(),
	mTransactionInfo(),
	mAcceptedEidTypes()
{
}
