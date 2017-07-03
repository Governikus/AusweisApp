/*!
 * IntentActivationHandler.h
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "CustomSchemeActivationHandler.h"

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
 * This ActivationHandler implements an API by opening custom URLs with scheme 'eid',
 * as specified by TR-03124-1.
 * The URL is passed by an Android Intent mechanism to the application.
 */
class IntentActivationHandler
	: public CustomSchemeActivationHandler
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ActivationHandler" FILE "metadata.json")
	Q_INTERFACES(governikus::ActivationHandler)

#ifdef Q_OS_ANDROID
	friend void::Java_com_governikus_ausweisapp2_MainActivity_triggerActivation(JNIEnv*, jobject, jstring);
#endif

	public:
		IntentActivationHandler();
		virtual ~IntentActivationHandler();

		virtual bool start() override;
		virtual void stop() override;
};

} /* namespace governikus */
