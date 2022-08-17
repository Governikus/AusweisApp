/*
 * Copyright (C) 2021 Bundesdruckerei GmbH and Governikus GmbH
 *
 */

#include "eid_applet_service_android.h"


/*!
 * Creates an EidAppletServiceAndroid with the given environment / context.
 * This method should be called from the main thread / the thread that created the JVM.
 *
 * \param env The android JNI Environment pointer.
 * \param applicationContext The android application context.
 */
template<class JNIEnvironment, class JavaVirtualMachine>
EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::EidAppletServiceAndroid(
	JNIEnvironment* env, jobject applicationContext) : mEnv(env) {

	if (mEnv) {
		mApplicationContext = mEnv->NewGlobalRef(applicationContext);
	}
}


/*!
 * Release all resources and shut down the eID-Applet-Service-Lib on Android
 */
template<class JNIEnvironment, class JavaVirtualMachine>
EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::~EidAppletServiceAndroid() {
	//get environment of current thread
	JNIEnvironment* curThreadEnv;
	ThreadGuard guard(mJvm);
	if (!getJNIEnvForCurrentThread(curThreadEnv, guard)) {
		return;
	}

	curThreadEnv->DeleteGlobalRef(mApplicationContext);
}


/*!
 * Performs initialization of eID-Applet-Service-Lib on Android. This method should be
 * called from the main thread / the thread that created the JVM.
 *
 * \result mData is blank if mResult is equal to EidServiceResult::SUCCESS,
 *      otherwise it contains an error message
 */
template<class JNIEnvironment, class JavaVirtualMachine>
GenericDataResult EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::initializeService(
		const std::string& pServiceId,
		const std::string& pVersionTag,
		const std::string& pSsdAid) {
	if (mEnv == nullptr) {
		return {EidServiceResult::ERROR, "missing JNIEnvironment"};
	}

	jint getJavaVMResult = mEnv->GetJavaVM(&mJvm);
	if (exceptionCheck(mEnv) || getJavaVMResult) {
		return {EidServiceResult::ERROR, "initializeService method must be called"
										 "from the thread that created the JVM!"};
	}

	jclass jniServiceClzRef = findClass(mEnv, mApplicationContext, jniService::path);
	if (exceptionCheck<JNIEnvironment>(mEnv)) {
		std::string msg = "missing class:";
		msg.append(jniService::path);
		return {EidServiceResult::ERROR, msg};
	}

	mJniServiceClz = reinterpret_cast<jclass>(mEnv->NewGlobalRef(jniServiceClzRef));
	jmethodID jniCtorMethodID = mEnv->GetMethodID(mJniServiceClz, jniService::stdInit, "()V");
	if (exceptionCheck<JNIEnvironment>(mEnv)) {
		std::string msg = "method not found:";
		msg.append(jniService::path)
		.append("#")
		.append(jniService::stdInit);
		return {EidServiceResult::ERROR, msg};
	}

	jobject handler = mEnv->NewObject(mJniServiceClz, jniCtorMethodID);
	mJniServiceObj = mEnv->NewGlobalRef(handler);

	jmethodID jniInitMethodID = mEnv->GetMethodID(mJniServiceClz,
			jniService::init,
			jniService::sig);
	if (exceptionCheck<JNIEnvironment>(mEnv)) {
		std::string msg = "method not found:";
		msg.append(jniService::path)
		.append("#")
		.append(jniService::init);
		return {EidServiceResult::ERROR, msg};
	}

	jstring serviceId = mEnv->NewStringUTF(pServiceId.c_str());
	jstring versionTag = mEnv->NewStringUTF(pVersionTag.c_str());
	jstring ssdAid = mEnv->NewStringUTF(pSsdAid.c_str());

	jobject obj = mEnv->CallObjectMethod(
			mJniServiceObj,
			jniInitMethodID,
			mApplicationContext,
			serviceId,
			versionTag,
			ssdAid);
	bool hasError = exceptionCheck<JNIEnvironment>(mEnv);
	mEnv->DeleteLocalRef(serviceId);
	mEnv->DeleteLocalRef(versionTag);
	mEnv->DeleteLocalRef(ssdAid);

	if (hasError) {
		std::string msg = "initialization of the eid_applet_service_lib failed";
		return {EidServiceResult::ERROR, msg};
	}

	auto clazz = mEnv->GetObjectClass(obj);
	auto fidResult = mEnv->GetFieldID(
			clazz,
			jniGenericDataResult::result,
			jniGenericDataResult::resultType);
	int result = mEnv->GetIntField(obj, fidResult);

	auto fidData = mEnv->GetFieldID(
			clazz,
			jniGenericDataResult::data,
			jniGenericDataResult::dataType);
	auto jsData = reinterpret_cast<jstring>(mEnv->GetObjectField(obj, fidData));

	return {getEidServiceResult(result), getString(mEnv, jsData)};
}


/*!
 * Performs the remote provisioning of the eID-applet from the TSM to the eSE on this
 * device. This function blocks the current thread until the TSM responds. It is
 * recommended to provide a separate WorkerThread for this purpose.
 *
 * \return EidServiceResult
 */
template<class JNIEnvironment, class JavaVirtualMachine>
EidServiceResult EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::installSmartEid() {
	//get environment of current thread
	JNIEnvironment* curThreadEnv;
	ThreadGuard guard(mJvm);
	if (!getJNIEnvForCurrentThread(curThreadEnv, guard)) {
		return EidServiceResult::ERROR;
	}
	//call installApplet method
	jclass serviceClass = findClass(curThreadEnv, mApplicationContext, jniService::path);
	jmethodID installSmartEidMethodID = curThreadEnv->GetMethodID(
			serviceClass,
			jniService::installSmartEidMethod,
			jniService::installSmartEidMethodSig);
	jobject resultCode = curThreadEnv->CallObjectMethod(
			mJniServiceObj,
			installSmartEidMethodID);
	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return EidServiceResult::ERROR;
	}

	//cast ServiceResult to int
	jclass serviceResultClass = findClass(
			curThreadEnv,
			mApplicationContext,
			jniServiceResult::path);

	jmethodID methodId = curThreadEnv->GetStaticMethodID(
			serviceResultClass,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int intResultCode = curThreadEnv->CallStaticIntMethod(
			serviceResultClass,
			methodId,
			resultCode);

	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return EidServiceResult::ERROR;
	}

	return EidServiceResult(intResultCode);
}


/*!
 * Delete the eID-Applet from this device.
 *
 * \return EidServiceResult
 */
template<class JNIEnvironment, class JavaVirtualMachine>
EidServiceResult EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::deleteSmartEid() {
	//get environment of current thread
	JNIEnvironment* curThreadEnv;
	ThreadGuard guard(mJvm);
	if (!getJNIEnvForCurrentThread(curThreadEnv, guard)) {
		return EidServiceResult::ERROR;
	}
	//call deleteApplet method
	jclass serviceClass = findClass(curThreadEnv, mApplicationContext, jniService::path);
	jmethodID deleteSmartEidMethodID = curThreadEnv->GetMethodID(
			serviceClass,
			jniService::deleteSmartEidMethod,
			jniService::deleteSmartEidMethodSig);
	jobject resultCode = curThreadEnv->CallObjectMethod(
			mJniServiceObj,
			deleteSmartEidMethodID);
	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return EidServiceResult::ERROR;
	}

	//cast ServiceResult to int
	jclass serviceResultClass = findClass(
			curThreadEnv,
			mApplicationContext,
			jniServiceResult::path);

	jmethodID methodId = curThreadEnv->GetStaticMethodID(
			serviceResultClass,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int intResultCode = curThreadEnv->CallStaticIntMethod(
			serviceResultClass,
			methodId,
			resultCode);

	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return EidServiceResult::ERROR;
	}

	return EidServiceResult(intResultCode);
}


/*!
 * Delete the eID-Applet from this device.
 *
 * \return EidServiceResult
 */
template<class JNIEnvironment, class JavaVirtualMachine>
EidServiceResult EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::deletePersonalization() {
	//get environment of current thread
	JNIEnvironment* curThreadEnv;
	ThreadGuard guard(mJvm);
	if (!getJNIEnvForCurrentThread(curThreadEnv, guard)) {
		return EidServiceResult::ERROR;
	}
	//call deletePersonalization method
	jclass serviceClass = findClass(curThreadEnv, mApplicationContext, jniService::path);
	jmethodID deletePersonalizationMethodID = curThreadEnv->GetMethodID(
			serviceClass,
			jniService::deletePersonalizationMethod,
			jniService::deletePersonalizationMethodSig);
	jobject resultCode = curThreadEnv->CallObjectMethod(
			mJniServiceObj,
			deletePersonalizationMethodID);
	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return EidServiceResult::ERROR;
	}

	//cast ServiceResult to int
	jclass serviceResultClass = findClass(
			curThreadEnv,
			mApplicationContext,
			jniServiceResult::path);

	jmethodID methodId = curThreadEnv->GetStaticMethodID(
			serviceResultClass,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int intResultCode = curThreadEnv->CallStaticIntMethod(
			serviceResultClass,
			methodId,
			resultCode);

	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return EidServiceResult::ERROR;
	}

	return EidServiceResult(intResultCode);
}


/*!
 * Performs APDU command
 *
 * \param pCommandApdu byte2hex encoded APDU
 * \return GenericDataResult with byte2hex encoded APDU response
 */
template<class JNIEnvironment, class JavaVirtualMachine>
GenericDataResult EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::performAPDUCommand(
		const std::string& pCommandApdu) {
	if (pCommandApdu.empty()) {
		return {EidServiceResult::ERROR, "command APDU should not be empty"};
	}

	//get environment of current thread
	JNIEnvironment* curThreadEnv;
	ThreadGuard guard(mJvm);
	if (!getJNIEnvForCurrentThread(curThreadEnv, guard)) {
		return {EidServiceResult::ERROR,
				"Current thread of the caller could not be attached"};
	}

	jclass serviceClass = findClass(curThreadEnv, mApplicationContext, jniService::path);
	jmethodID performMethodID = curThreadEnv->GetMethodID(
			serviceClass,
			jniService::performAPDUCommandMethod,
			jniService::performAPDUCommandMethodSig);
	jstring apdu = curThreadEnv->NewStringUTF(pCommandApdu.c_str());
	jobject obj = curThreadEnv->CallObjectMethod(mJniServiceObj, performMethodID, apdu);
	bool hasError = exceptionCheck<JNIEnvironment>(curThreadEnv);
	curThreadEnv->DeleteLocalRef(apdu);
	if (hasError) {
		return {EidServiceResult::ERROR,
				"call of the performAPDUCommand function failed"};
	}

	auto clazz = curThreadEnv->GetObjectClass(obj);

	auto fidResult = curThreadEnv->GetFieldID(
			clazz,
			jniGenericDataResult::result,
			jniGenericDataResult::resultType);
	int result = curThreadEnv->GetIntField(obj, fidResult);

	auto fidData = curThreadEnv->GetFieldID(
			clazz,
			jniGenericDataResult::data,
			jniGenericDataResult::dataType);
	auto jsData = reinterpret_cast<jstring>(curThreadEnv->GetObjectField(obj, fidData));
	std::string data = getString(curThreadEnv, jsData);

	if (data.empty()) {
		return {EidServiceResult::ERROR, "response APDU should not be empty"};
	}

	return {getEidServiceResult(result), data};
}


/*!
 * Performs initialization of the Personalization
 *
 * \param pPin property is only relevant for iOS implementation and unused in Android
 * \param pChallenge challenge for key attestation
 * \return InitializeResult with public key and signed challenge
 */
template<class JNIEnvironment, class JavaVirtualMachine>
InitializeResult EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::initializePersonalization(
		const std::string& pPin,
		const std::string& pChallenge) {
	if (pPin.empty()) {
		// do nothing: Entering the eID-PIN is currently not necessary for Android here
	}

	if (pChallenge.empty()) {
		return {EidServiceResult::ERROR, "Challenge should not be empty"};
	}

	//get environment of current thread
	JNIEnvironment* curThreadEnv;
	ThreadGuard guard(mJvm);
	if (!getJNIEnvForCurrentThread(curThreadEnv, guard)) {
		return {EidServiceResult::ERROR,
				"Current thread of the caller could not be attached"};
	}

	jmethodID initializePersonalizationMethodID = curThreadEnv->GetMethodID(
			mJniServiceClz,
			jniService::initializePersonalizationMethod,
			jniService::initializePersonalizationMethodSig);
	jstring challenge = curThreadEnv->NewStringUTF(pChallenge.c_str());
	jobject obj = curThreadEnv->CallObjectMethod(mJniServiceObj,
			initializePersonalizationMethodID,
			challenge);

	curThreadEnv->DeleteLocalRef(challenge);
	if (exceptionCheck(curThreadEnv)) {
		return {EidServiceResult::ERROR,
				"call of the initializePersonalization function failed"};
	}

	auto clazz = curThreadEnv->GetObjectClass(obj);
	auto fidResult = curThreadEnv->GetFieldID(clazz, jniInitializeResult::result,
			jniInitializeResult::resultType);
	int result = curThreadEnv->GetIntField(obj, fidResult);

	auto fidPPData = curThreadEnv->GetFieldID(clazz,
			jniInitializeResult::ppData,
			jniInitializeResult::ppDataType);
	auto jsPPData = reinterpret_cast<jstring>(curThreadEnv->GetObjectField(obj,
			fidPPData));
	if (exceptionCheck(curThreadEnv)) {
		return {EidServiceResult::ERROR,
				"call of the initializePersonalization function failed (cannot read result)"};
	}

	std::string ppData = getString(curThreadEnv, jsPPData);

	if (ppData.empty()) {
		return {EidServiceResult::ERROR, "PreparePersonalizationData should not be empty"};
	}

	return {getEidServiceResult(result), ppData};
}


/*!
 * Performs personalization in a generic way controlled by Personalization Service
 *
 * \param pCommandPersonalization base64 encoded personalization step
 * \return GenericDataResult with base64 encoded personalization step response
 */
template<class JNIEnvironment, class JavaVirtualMachine>
GenericDataResult EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::performPersonalization(const std::string& pCommandPersonalization) {
	if (pCommandPersonalization.empty()) {
		return {EidServiceResult::ERROR, "personalization C-APDU should not be empty"};
	}

	//get environment of current thread
	JNIEnvironment* curThreadEnv;
	ThreadGuard guard(mJvm);
	if (!getJNIEnvForCurrentThread(curThreadEnv, guard)) {
		return {EidServiceResult::ERROR,
				"Current thread of the caller could not be attached"};
	}

	jclass serviceClass = findClass(curThreadEnv, mApplicationContext, jniService::path);
	jmethodID performMethodID = curThreadEnv->GetMethodID(
			serviceClass,
			jniService::performPersonalizationMethod,
			jniService::performPersonalizationMethodSig);
	jstring apdu = curThreadEnv->NewStringUTF(pCommandPersonalization.c_str());
	jobject obj = curThreadEnv->CallObjectMethod(mJniServiceObj, performMethodID, apdu);
	bool hasError = exceptionCheck<JNIEnvironment>(curThreadEnv);
	curThreadEnv->DeleteLocalRef(apdu);
	if (hasError) {
		return {EidServiceResult::ERROR,
				"call of the performPersonalization function failed"};
	}

	auto clazz = curThreadEnv->GetObjectClass(obj);

	auto fidResult = curThreadEnv->GetFieldID(
			clazz,
			jniGenericDataResult::result,
			jniGenericDataResult::resultType);
	int result = curThreadEnv->GetIntField(obj, fidResult);

	auto fidData = curThreadEnv->GetFieldID(
			clazz,
			jniGenericDataResult::data,
			jniGenericDataResult::dataType);
	auto jsData = reinterpret_cast<jstring>(curThreadEnv->GetObjectField(obj, fidData));
	std::string data = getString(curThreadEnv, jsData);

	if (data.empty()) {
		return {EidServiceResult::ERROR, "response APDU should not be empty"};
	}

	return {getEidServiceResult(result), data};
}


/*!
 * Finalize the Personalization and provide the init-eID-PIN
 *
 * \return a PersonalizationResult object with the init-eID-PIN
 */
template<class JNIEnvironment, class JavaVirtualMachine>
PersonalizationResult EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::finalizePersonalization() {
	JNIEnvironment* curThreadEnv;
	ThreadGuard guard(mJvm);
	if (!getJNIEnvForCurrentThread(curThreadEnv, guard)) {
		return {EidServiceResult::ERROR};
	}

	jclass serviceClass = findClass(curThreadEnv, mApplicationContext, jniService::path);
	jmethodID finalizePersonalizationMethodId = curThreadEnv->GetMethodID(
			serviceClass,
			jniService::finalizePersonalizationMethod,
			jniService::finalizePersonalizationMethodSig);
	jobject obj = curThreadEnv->CallObjectMethod(mJniServiceObj,
			finalizePersonalizationMethodId);
	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return {EidServiceResult::ERROR};
	}

	auto clazz = curThreadEnv->GetObjectClass(obj);

	auto fidResult = curThreadEnv->GetFieldID(
			clazz,
			jniPersonalizationResult::result,
			jniPersonalizationResult::resultType);
	int result = curThreadEnv->GetIntField(obj, fidResult);

	auto fidPin = curThreadEnv->GetFieldID(
			clazz,
			jniPersonalizationResult::initPINData,
			jniPersonalizationResult::initPINDataType);
	auto jsData = reinterpret_cast<jstring>(curThreadEnv->GetObjectField(obj, fidPin));
	std::string pin = getString(curThreadEnv, jsData);

	return {getEidServiceResult(result), pin};
}


/*!
 * Provides the status of the Smart-eID or eID-Applet.
 *
 * \return The status of the eID-Applet
 */
template<class JNIEnvironment, class JavaVirtualMachine>
EidStatus EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::getSmartEidStatus() {
	//get environment of current thread
	JNIEnvironment* curThreadEnv;
	ThreadGuard guard(mJvm);
	if (!getJNIEnvForCurrentThread(curThreadEnv, guard)) {
		return EidStatus::INTERNAL_ERROR;
	}

	jmethodID eIDStatusMethodID = curThreadEnv->GetMethodID(
			mJniServiceClz,
			jniService::eIDStatusMethod,
			jniService::eIDStatusMethodSig);

	jobject resultObj = curThreadEnv->CallObjectMethod(mJniServiceObj, eIDStatusMethodID);
	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return EidStatus::INTERNAL_ERROR;
	}

	//cast EidStatus to int
	jclass statusClass = findClass(
			curThreadEnv,
			mApplicationContext,
			jniEidStatus::path);

	jmethodID methodId = curThreadEnv->GetStaticMethodID(
			statusClass,
			jniEidStatus::getValue,
			jniEidStatus::sigGetValue);

	int intResultCode = curThreadEnv->CallStaticIntMethod(
			statusClass,
			methodId,
			resultObj);

	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return EidStatus::INTERNAL_ERROR;
	}

	return EidStatus(intResultCode);
}


/*!
 * Provides information of available updates of the installed eID-Applet and/or CSP
 * implementation or whether the device is supported by TSMS.
 *
 * \return The Update-Info of the eID-Applet
 */
template<class JNIEnvironment, class JavaVirtualMachine>
EidUpdateInfo EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::getUpdateInfo() {
	//get environment of current thread
	JNIEnvironment* curThreadEnv;
	ThreadGuard guard(mJvm);
	if (!getJNIEnvForCurrentThread(curThreadEnv, guard)) {
		return EidUpdateInfo::INTERNAL_ERROR;
	}

	jmethodID updateInfoMethodeId = curThreadEnv->GetMethodID(
			mJniServiceClz,
			jniService::updateInfoMethod,
			jniService::updateInfoMethodSig);

	jobject resultObj = curThreadEnv->CallObjectMethod(mJniServiceObj, updateInfoMethodeId);
	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return EidUpdateInfo::INTERNAL_ERROR;
	}

	//cast EidStatus to int
	jclass statusClass = findClass(
			curThreadEnv,
			mApplicationContext,
			jniEidUpdateInfo::path);

	jmethodID methodId = curThreadEnv->GetStaticMethodID(
			statusClass,
			jniEidUpdateInfo::getValue,
			jniEidUpdateInfo::sigGetValue);

	int intResultCode = curThreadEnv->CallStaticIntMethod(
			statusClass,
			methodId,
			resultObj);

	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return EidUpdateInfo::INTERNAL_ERROR;
	}

	return EidUpdateInfo(intResultCode);
}


/*!
 * Releases any open channel to the applet
 *
 * \return EidServiceResult
 */
template<class JNIEnvironment, class JavaVirtualMachine>
EidServiceResult EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::releaseAppletConnection() {
	JNIEnvironment* curThreadEnv;
	ThreadGuard guard(mJvm);
	if (!getJNIEnvForCurrentThread(curThreadEnv, guard)) {
		return EidServiceResult::ERROR;
	}

	//call release method
	jclass serviceClass = findClass(curThreadEnv, mApplicationContext, jniService::path);
	jmethodID releaseAppletConnectionMethodId = curThreadEnv->GetMethodID(
			serviceClass,
			jniService::releaseAppletConnectionMethod,
			jniService::releaseAppletConnectionMethodSig);
	jobject resultCode = curThreadEnv->CallObjectMethod(
			mJniServiceObj,
			releaseAppletConnectionMethodId);
	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return EidServiceResult::ERROR;
	}

	//cast ServiceResult to int
	jclass serviceResultClass = findClass(
			curThreadEnv,
			mApplicationContext,
			jniServiceResult::path);

	jmethodID methodId = curThreadEnv->GetStaticMethodID(
			serviceResultClass,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int intResultCode = curThreadEnv->CallStaticIntMethod(
			serviceResultClass,
			methodId,
			resultCode);

	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return EidServiceResult::ERROR;
	}

	return EidServiceResult(intResultCode);
}


/*!
 * Release all resources and shut down the eID-Applet-Service-Lib on Android
 *
 * \return GenericDataResult mData is blank if mResult is equal to
 *              EidServiceResult::SUCCESS, otherwise it contains an error message
 */
template<class JNIEnvironment, class JavaVirtualMachine>
GenericDataResult EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::shutdownService() {
	//get environment of current thread
	JNIEnvironment* curThreadEnv;
	ThreadGuard guard(mJvm);
	if (!getJNIEnvForCurrentThread(curThreadEnv, guard)) {
		return {EidServiceResult::ERROR,
				"Current thread of the caller could not be attached"};
	}

	jmethodID shutdownMethodID = curThreadEnv->GetMethodID(
			mJniServiceClz,
			jniService::shutdownMethod,
			jniService::shutdownMethodSig);
	if (exceptionCheck<JNIEnvironment>(curThreadEnv)) {
		return {EidServiceResult::ERROR,
				"Couldn't shutdown eID Applet Service. Method not found."};
	}

	jobject obj = curThreadEnv->CallObjectMethod(mJniServiceObj, shutdownMethodID);
	bool hasError = exceptionCheck<JNIEnvironment>(curThreadEnv);
	if (hasError) {
		return {EidServiceResult::ERROR,
				"Couldn't shutdown eID Applet Service. Call of the shutdown function failed."};
	}

	curThreadEnv->DeleteGlobalRef(mJniServiceClz);
	curThreadEnv->DeleteGlobalRef(mJniServiceObj);

	auto clazz = curThreadEnv->GetObjectClass(obj);
	auto fidResult = curThreadEnv->GetFieldID(
			clazz,
			jniGenericDataResult::result,
			jniGenericDataResult::resultType);
	int resultType = curThreadEnv->GetIntField(obj, fidResult);

	auto fidData = curThreadEnv->GetFieldID(
			clazz,
			jniGenericDataResult::data,
			jniGenericDataResult::dataType);
	auto jsData = reinterpret_cast<jstring>(curThreadEnv->GetObjectField(obj, fidData));
	std::string data = getString(curThreadEnv, jsData);

	return {getEidServiceResult(resultType), data};
}


template<class JNIEnvironment, class JavaVirtualMachine>
EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::ThreadGuard::ThreadGuard(JavaVirtualMachine* pJvm)
	: mJvm(pJvm)
	, mDoDetach(false) {
}


template<class JNIEnvironment, class JavaVirtualMachine>
EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::ThreadGuard::~ThreadGuard() {
	if (mDoDetach) {
		mJvm->DetachCurrentThread();
	}
}


template<class JNIEnvironment, class JavaVirtualMachine>
void EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::ThreadGuard::doDetach() {
	mDoDetach = true;
}


/*!
 * Returns the JNI Environment of the current thread. If the env is not attached it will be
 * attached. This is a save way to use the current JNIEnv, as it could be that the member
 * var env of the class is attached to another thread.
 *
 * \param _env the JNIEnv of the current thread (out param)
 * \param guard who can be ordered to detach the current thread on destruction
 * \return true = success, false = error
 */
template<class JNIEnvironment, class JavaVirtualMachine>
bool EidAppletServiceAndroid<JNIEnvironment, JavaVirtualMachine>::getJNIEnvForCurrentThread(JNIEnvironment*& _env, ThreadGuard& guard) {
	int getEnvResult = mJvm->GetEnv(reinterpret_cast<void**>(&_env), JNI_VERSION_1_6);
	if (getEnvResult == JNI_OK) {
		return true;
	}

	if (getEnvResult == JNI_EDETACHED) {
#ifdef __ANDROID__
		int attachResult = mJvm->AttachCurrentThread(&_env, nullptr);
#else
		int attachResult = mJvm->AttachCurrentThread(reinterpret_cast<void**>(&mEnv), nullptr);
#endif
		guard.doDetach();

		return attachResult == JNI_OK;
	}

	return false;
}


//Needed so the linker knows what kinda instance of the template class we need otherwise this .cpp cant be linked to the .h!
template class EidAppletServiceAndroid<JNIEnv, JavaVM>;
