/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
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
		qDebug() << "Perform redirect to URL" << mRedirectAddress;
		QDesktopServices::openUrl(mRedirectAddress);
	}
}


QUrl CustomSchemeActivationContext::getActivationURL() const
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
	Q_EMIT fireShowUserInformation(GlobalStatus(GlobalStatus::Code::Workflow_AlreadyInProgress_Error).toErrorDescription());
	return true;
}


bool CustomSchemeActivationContext::sendErrorPage(http_status, const GlobalStatus&)
{
	// The error is displayed in the application,
	// so here is nothing to be done in this case.
	return true;
}


bool CustomSchemeActivationContext::sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus)
{
	mRedirectAddress = UrlUtil::addMajorMinor(pRedirectAddress, pStatus);
	qDebug() << "Determined redirect URL" << mRedirectAddress;
	return true;

	/*
	 * Don't redirect now, but when deleting this object --> see comment in destructor
	 */
}
