/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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


QUrl InternalActivationContext::getActivationURL()
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


bool InternalActivationContext::sendErrorPage(HttpStatusCode pStatusCode, const Result& pResult)
{
	Q_UNUSED(pStatusCode)
	Q_UNUSED(pResult)
	return true;
}


bool InternalActivationContext::sendRedirect(const QUrl& pRedirectAddress, const Result& pResult)
{
	Q_UNUSED(pRedirectAddress)
	Q_UNUSED(pResult)
	return true;
}
