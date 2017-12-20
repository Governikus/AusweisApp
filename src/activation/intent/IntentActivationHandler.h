/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationHandler.h"

#ifdef Q_OS_ANDROID
#include <jni.h>


extern "C"
{
JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_MainActivity_triggerActivation(JNIEnv* env, jobject obj, jstring lastIntent);
}
#endif


namespace governikus
{

/*!
 * This ActivationHandler implements an API by opening custom URLs registered in the Android manifest,
 * as specified by TR-03124-1.
 * The URL is passed by an Android Intent mechanism to the application.
 */
class IntentActivationHandler
	: public ActivationHandler
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ActivationHandler" FILE "metadata.json")
	Q_INTERFACES(governikus::ActivationHandler)

#ifdef Q_OS_ANDROID
	friend void ::Java_com_governikus_ausweisapp2_MainActivity_triggerActivation(JNIEnv*, jobject, jstring);
#endif

	private:
		void onIntent(const QUrl& pUrl);

	public:
		IntentActivationHandler() = default;

		virtual bool start() override;
		virtual void stop() override;
};

} /* namespace governikus */
