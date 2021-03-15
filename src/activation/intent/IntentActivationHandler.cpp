/*!
 * \copyright Copyright (c) 2015-2021 Governikus GmbH & Co. KG, Germany
 */

#include "IntentActivationHandler.h"

#include "IntentActivationContext.h"

#include <QLoggingCategory>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#endif


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(activation)


void IntentActivationHandler::onIntent(const QUrl& pUrl, const QString& pReferrer)
{
	qCDebug(activation) << "Got new authentication request by:" << pReferrer;
	qCDebug(activation) << "Request URL:" << pUrl;
	const auto& context = QSharedPointer<IntentActivationContext>::create(pUrl, pReferrer);
	connect(context.data(), &IntentActivationContext::fireShowUserInformation, this, &ActivationHandler::fireShowUserInformation);
	Q_EMIT fireAuthenticationRequest(context);
}


bool IntentActivationHandler::start()
{
#ifdef Q_OS_ANDROID
	return true;

#else
	qCCritical(activation) << "Cannot use IntentActivationHandler on non android system";
	return false;

#endif
}


void IntentActivationHandler::stop()
{
}


void IntentActivationHandler::onApplicationActivated()
{
#ifdef Q_OS_ANDROID
	const QString& intent = QAndroidJniObject::callStaticObjectMethod<jstring>("com/governikus/ausweisapp2/MainActivity", "fetchStoredIntent").toString();
	const QString& referrer = QAndroidJniObject::callStaticObjectMethod<jstring>("com/governikus/ausweisapp2/MainActivity", "fetchStoredReferrer").toString();
	if (!intent.isNull())
	{
		onIntent(intent, referrer);
	}
#endif
}
