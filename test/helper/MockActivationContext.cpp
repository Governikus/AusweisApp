/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MockActivationContext.h"

using namespace governikus;

MockActivationContext::MockActivationContext(bool pProcessing, bool pAlreadyActive, bool pErroPage, bool pRedirect, const QString& pSendError)
	: ActivationContext()
	, mProcessingValue(pProcessing)
	, mAlreadyActiveValue(pAlreadyActive)
	, mErroPageValue(pErroPage)
	, mRedirectValue(pRedirect)
	, mErrorMessageOnSend(pSendError)
	, mSendProcessingCalled(false)
	, mSendAlreadyActiveCalled(false)
	, mSendErroPageCalled(false)
	, mSendRedirectCalled(false)
{
}


MockActivationContext::~MockActivationContext()
{
}
