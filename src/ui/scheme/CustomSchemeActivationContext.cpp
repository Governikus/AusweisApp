/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "CustomSchemeActivationContext.h"

#include "UrlUtil.h"

#include <QDesktopServices>
#include <QUrlQuery>

#ifdef Q_OS_ANDROID
	#include <QJniObject>
#endif


using namespace governikus;


CustomSchemeActivationContext::CustomSchemeActivationContext(const QUrl& pActivationUrl, const QString& pReferrer)
	: ActivationContext()
	, mActivationUrl(pActivationUrl)
	, mReferrer(pReferrer)
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
		 * has finished and the AuthModel is deleted.
		 */
		qDebug() << "Perform redirect to URL" << mRedirectAddress;

#ifdef Q_OS_ANDROID
		QJniObject context = QNativeInterface::QAndroidApplication::context();
		if (context.callMethod<jboolean>("openUrl", "(Ljava/lang/String;Ljava/lang/String;)Z", QJniObject::fromString(mRedirectAddress.url()).object<jstring>(), QJniObject::fromString(mReferrer).object<jstring>()))
		{
			return;
		}
#endif

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
