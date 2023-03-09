/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ActivationContext.h"

using namespace governikus;


ActivationContext::ActivationContext()
	: mSendError()
{
}


void ActivationContext::setSendError(const QString& pError)
{
	mSendError = pError;
}
