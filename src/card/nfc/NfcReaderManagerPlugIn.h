/*!
 * \brief Implementation of \ref ReaderManagerPlugIn for NFC on Android.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once

#include "Reader.h"
#include "ReaderManagerPlugIn.h"


#ifdef Q_OS_ANDROID
#include <jni.h>


extern "C"
{
JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_NfcAdapterStateChangeReceiver_nfcAdapterStateChanged(JNIEnv* env, jobject obj, jint newState);
}
#endif


namespace governikus
{

class NfcReaderManagerPlugIn
	: public ReaderManagerPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)

#ifdef Q_OS_ANDROID
	friend void::Java_com_governikus_ausweisapp2_NfcAdapterStateChangeReceiver_nfcAdapterStateChanged(JNIEnv * env, jobject obj, jint newState);
#endif

	private:
		static NfcReaderManagerPlugIn* mInstance;
		bool mInitialized;
		QList<Reader*> mReaderList;

		void addNfcReader();

	private Q_SLOTS:
		void setNfcStatus(bool pEnabled);

	public:
		NfcReaderManagerPlugIn();
		virtual ~NfcReaderManagerPlugIn();

		virtual QList<Reader*> getReader() const override;

		virtual void init() override;
		virtual void shutdown() override;
};

} /* namespace governikus */
