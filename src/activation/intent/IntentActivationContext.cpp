/*!
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "IntentActivationContext.h"

#include "GlobalStatus.h"
#include "UrlUtil.h"

#include <QDesktopServices>

#ifdef Q_OS_ANDROID
	#include <QtAndroidExtras/QAndroidJniObject>
	#include <QtAndroidExtras/QtAndroid>
#endif

using namespace governikus;


IntentActivationContext::IntentActivationContext(const QUrl& pActivationUrl, const QString& pReferrer)
	: ActivationContext()
	, mActivationUrl(pActivationUrl)
	, mReferrer(pReferrer)
	, mRedirectAddress()
{
}


IntentActivationContext::~IntentActivationContext()
{
	if (!mRedirectAddress.isEmpty())
	{
		/*
		 * Opening an URL on Android will bring the browser to the foreground, our app
		 * will go to the background, causing the authentication controller to stop.
		 *
		 * Therefore we open the URL during deletion, which takes place when authentication
		 * has finished and the AuthMdel is deleted.
		 */
		qDebug() << "Perform redirect to URL" << mRedirectAddress;
#ifdef Q_OS_ANDROID
		if (QtAndroid::androidActivity().callMethod<jboolean>("openUrl", "(Ljava/lang/String;Ljava/lang/String;)Z", QAndroidJniObject::fromString(mRedirectAddress.url()).object<jstring>(), QAndroidJniObject::fromString(mReferrer).object<jstring>()))
		{
			return;
		}
#endif

		QDesktopServices::openUrl(mRedirectAddress);
	}
}


QUrl IntentActivationContext::getActivationURL() const
{
	return mActivationUrl;
}


bool IntentActivationContext::sendProcessing()
{
	// nothing to be done in this case
	return true;
}


bool IntentActivationContext::sendOperationAlreadyActive()
{
	Q_EMIT fireShowUserInformation(GlobalStatus(GlobalStatus::Code::Workflow_AlreadyInProgress_Error).toErrorDescription());
	return true;
}


bool IntentActivationContext::sendErrorPage(http_status, const GlobalStatus&)
{
	// the error is displayed in the application,
	// so here is nothing to be done in this case
	return true;
}


bool IntentActivationContext::sendRedirect(const QUrl& pRedirectAddress, const GlobalStatus& pStatus)
{
	mRedirectAddress = UrlUtil::addMajorMinor(pRedirectAddress, pStatus);
	qDebug() << "Determined redirect URL" << mRedirectAddress;
	return true;
}
