/*!
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#include "InternalActivationContext.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(activation)

using namespace governikus;


InternalActivationContext::InternalActivationContext(const QUrl& pUrl)
	: ActivationContext()
	, mTcTokenUrl(pUrl)
{
}


InternalActivationContext::~InternalActivationContext()
{
}


QUrl InternalActivationContext::getActivationURL() const
{
	return mTcTokenUrl;
}


bool InternalActivationContext::sendProcessing()
{
	return true;
}


bool InternalActivationContext::sendOperationAlreadyActive()
{
	return true;
}


bool InternalActivationContext::sendErrorPage(HttpStatusCode pStatusCode, const GlobalStatus& pStatus)
{
	Q_UNUSED(pStatusCode)
	Q_UNUSED(pStatus)
	return true;
}


bool InternalActivationContext::sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus)
{
	Q_UNUSED(pRedirectAddress)
	Q_UNUSED(pStatus)
	return true;
}
