/*!
 * CustomSchemeActivationContext.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CustomSchemeActivationContext.h"

#include "UrlUtil.h"

#include <QDesktopServices>
#include <QUrlQuery>


using namespace governikus;


CustomSchemeActivationContext::CustomSchemeActivationContext(const QUrl& pActivationUrl)
	: ActivationContext()
	, mActivationUrl(pActivationUrl)
	, mRedirectAddress()
{
}


CustomSchemeActivationContext::~CustomSchemeActivationContext()
{
	if (!mRedirectAddress.isEmpty())
	{
		/*
		 * Opening an URL on iOS will bring Safari to the foreground, our app
		 * will go to the background, causing the authentication controller to stop.
		 *
		 * Therefore we open the URL during deletion, which takes place when authentication
		 * has finished and the AuthMdel is deleted.
		 */
		qDebug() << "Redirecting now to URL " << mRedirectAddress;
		QDesktopServices::openUrl(mRedirectAddress);
	}
}


QUrl CustomSchemeActivationContext::getActivationURL()
{
	return mActivationUrl;
}


bool CustomSchemeActivationContext::sendProcessing()
{
	// nothing to be done in this case
	return true;
}


bool CustomSchemeActivationContext::sendOperationAlreadyActive()
{
	// probably we emit a signal, that will be caught by the AppController
	// so that then UI can display some message or so?
	return true;
}


bool CustomSchemeActivationContext::sendErrorPage(HttpStatusCode pStatusCode, const Result& pResult)
{
	Q_UNUSED(pStatusCode);
	Q_UNUSED(pResult);
	// probably we emit a signal, that will be caught by the AppController
	// so that then UI can display some message or so?
	return true;
}


bool CustomSchemeActivationContext::sendRedirect(const QUrl& pRedirectAddress, const Result& pResult)
{
	mRedirectAddress = UrlUtil::addMajorMinor(pRedirectAddress, pResult);
	qDebug() << "Redirect URL:" << mRedirectAddress;
	return true;

	/*
	 * Don't redirect now, but when deleting this object --> see comment in destructor
	 */
}
