/*!
 * IntentActivationHandler.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "IntentActivationHandler.h"

#include "IntentActivationContext.h"

#include <QLoggingCategory>

#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#endif


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(activation)

#ifdef Q_OS_ANDROID

JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_MainActivity_triggerActivation(JNIEnv* env, jobject obj, jstring lastIntent)
{
	Q_UNUSED(env)
	Q_UNUSED(obj)
	if (lastIntent != nullptr)
	{
		QString lastIntentString = QAndroidJniObject("java/lang/String", "(Ljava/lang/String;)V", lastIntent).toString();
		if (!lastIntentString.isNull())
		{
			auto handler = ActivationHandler::getInstance<IntentActivationHandler>();
			Q_ASSERT(handler);
			handler->onIntent(QUrl(lastIntentString));
		}
	}
}


#endif


void IntentActivationHandler::onIntent(const QUrl& pUrl)
{
	qCDebug(activation) << "Got new authentication request";
	qCDebug(activation) << "Request URL:" << pUrl;
	Q_EMIT fireAuthenticationRequest(new IntentActivationContext(pUrl));
}


bool IntentActivationHandler::start()
{
#ifdef Q_OS_ANDROID
	const QString& initialIntent = QAndroidJniObject::callStaticObjectMethod<jstring>("com/governikus/ausweisapp2/MainActivity", "getInitialIntent").toString();
	if (!initialIntent.isNull())
	{
		onIntent(initialIntent);
	}

	return true;

#else
	qCCritical(activation) << "Cannot use IntentActivationHandler on non android system";
	return false;

#endif
}


void IntentActivationHandler::stop()
{
}
