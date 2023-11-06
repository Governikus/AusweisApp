/*
 * Copyright (C) 2023 by Bundesdruckerei GmbH and Governikus GmbH & Co. KG
 * Licensed under the EUPL-1.2
 */

#include "eid_applet_service_android.h"


/*!
 * Creates an EidAppletServiceAndroid with the given environment / context.
 * This method should be called from the main thread / the thread that created the JVM.
 *
 * \param env The android JNI Environment pointer.
 * \param applicationContext The android application context.
 */
EidAppletServiceAndroid::EidAppletServiceAndroid(
	JNIEnv* env, jobject applicationContext) : mEnv(env) {
	if (mEnv) {
		mApplicationContext = mEnv->NewGlobalRef(applicationContext);
	}
}


/*!
 * Release all resources and shut down the eID-Applet-Service-Lib on Android
 */
EidAppletServiceAndroid::~EidAppletServiceAndroid() {
	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	if (!getJNIEnvForCurrentThread(env, threadGuard)) {
		return;
	}

	deleteGlobalRef(env, mJniServiceObj);
	deleteGlobalRef(env, mJniServiceClz);
	deleteGlobalRef(env, mApplicationContext);
}


/*!
 * Performs initialization of eID-Applet-Service-Lib on Android. This method should be
 * called from the main thread / the thread that created the JVM.
 *
 * \result mData is blank if mResult is equal to EidServiceResult::SUCCESS,
 *      otherwise it contains an error message
 */
GenericDataResult EidAppletServiceAndroid::initializeService(
		const std::string& pServiceId,
		const std::string& pSsdAid,
		const std::string& pVersionTag) {

	const std::lock_guard<std::mutex> guard(serviceMutex);

	if (mEnv == nullptr) {
		return {EidServiceResult::ERROR, "missing JNIEnv"};
	}

	if (mApplicationContext == nullptr) {
		return {EidServiceResult::ERROR, "missing ApplicationContext"};
	}

	jint getJavaVMResult = mEnv->GetJavaVM(&mJvm);
	if (exceptionCheck(mEnv) || getJavaVMResult) {
		return {EidServiceResult::ERROR, "initializeService method must be called"
										 "from the thread that created the JVM!"};
	}

	jclass jniServiceClzRef = findClass(mEnv, mApplicationContext, jniService::path);
	if (exceptionCheck(mEnv)) {
		std::string msg = "missing class:";
		msg.append(jniService::path);
		return {EidServiceResult::ERROR, msg};
	}

	mJniServiceClz = reinterpret_cast<jclass>(mEnv->NewGlobalRef(jniServiceClzRef));
	jmethodID jniCtorMethodID = mEnv->GetMethodID(mJniServiceClz, jniService::stdInit, "()V");
	if (exceptionCheck(mEnv)) {
		std::string msg = "method not found:";
		msg.append(jniService::path)
		.append("#")
		.append(jniService::stdInit);
		return {EidServiceResult::ERROR, msg};
	}

	jobject handler = mEnv->NewObject(mJniServiceClz, jniCtorMethodID);
	mJniServiceObj = mEnv->NewGlobalRef(handler);
	jmethodID jniInitMethodID = mEnv->GetMethodID(
			mJniServiceClz,
			jniService::init,
			jniService::sig);
	deleteLocalRef(mEnv, handler);
	if (exceptionCheck(mEnv)) {
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
			ssdAid,
			versionTag);
	bool hasError = exceptionCheck(mEnv);
	deleteLocalRef(mEnv, serviceId);
	deleteLocalRef(mEnv, versionTag);
	deleteLocalRef(mEnv, ssdAid);
	deleteLocalRef(mEnv, jniServiceClzRef);

	if (hasError) {
		std::string msg = "initialization of the eid_applet_service_lib failed";
		return {EidServiceResult::ERROR, msg};
	}

	auto clazz = mEnv->GetObjectClass(obj);
	auto fidResult = mEnv->GetFieldID(
			clazz,
			jniGenericDataResult::result,
			jniGenericDataResult::resultType);
	auto result = mEnv->GetObjectField(obj, fidResult);

	auto fidData = mEnv->GetFieldID(
			clazz,
			jniGenericDataResult::data,
			jniGenericDataResult::dataType);

	auto fidObjField = mEnv->GetObjectField(obj, fidData);
	auto jsData = reinterpret_cast<jstring>(fidObjField);

	jclass serviceResultClazz = findClass(mEnv, mApplicationContext, jniServiceResult::path);
	jmethodID getValueMethodID = mEnv->GetStaticMethodID(serviceResultClazz,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int serviceResultCode = mEnv->CallStaticIntMethod(serviceResultClazz, getValueMethodID, result);
	GenericDataResult gDResult(getEidServiceResult(serviceResultCode), getString(mEnv, jsData));

	deleteLocalRef(mEnv, result);
	deleteLocalRef(mEnv, fidObjField);
	deleteLocalRef(mEnv, obj);
	deleteLocalRef(mEnv, clazz);
	deleteLocalRef(mEnv, serviceResultClazz);

	return gDResult;
}


/*!
 * Performs the remote provisioning of the eID-applet from the TSM to the eSE on this
 * device. This function blocks the current thread until the TSM responds. It is
 * recommended to provide a separate WorkerThread for this purpose.
 *
 * \return EidServiceResult
 */
EidServiceResult EidAppletServiceAndroid::installSmartEid() {

	const std::lock_guard<std::mutex> guard(serviceMutex);

	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	if (!getJNIEnvForCurrentThread(env, threadGuard) || mApplicationContext == nullptr || mJniServiceClz == nullptr || mJniServiceObj == nullptr) {
		return EidServiceResult::ERROR;
	}

	jmethodID installSmartEidMethodID = env->GetMethodID(
			mJniServiceClz,
			jniService::installSmartEidMethod,
			jniService::installSmartEidMethodSig);
	jobject resultCode = env->CallObjectMethod(
			mJniServiceObj,
			installSmartEidMethodID);
	if (exceptionCheck(env)) {
		return EidServiceResult::ERROR;
	}

	jclass serviceResultClass = findClass(env, mApplicationContext, jniServiceResult::path);
	jmethodID getValueMethodID = env->GetStaticMethodID(
			serviceResultClass,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int serviceResultCode = env->CallStaticIntMethod(
			serviceResultClass,
			getValueMethodID,
			resultCode);

	if (exceptionCheck(env)) {
		return EidServiceResult::ERROR;
	}

	deleteLocalRef(env, resultCode);
	deleteLocalRef(env, serviceResultClass);

	return EidServiceResult(serviceResultCode);
}


/*!
 * Delete the eID-Applet from this device.
 *
 * \return EidServiceResult
 */
EidServiceResult EidAppletServiceAndroid::deleteSmartEid() {

	const std::lock_guard<std::mutex> guard(serviceMutex);

	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	if (!getJNIEnvForCurrentThread(env, threadGuard) || mApplicationContext == nullptr || mJniServiceClz == nullptr || mJniServiceObj == nullptr) {
		return EidServiceResult::ERROR;
	}

	jmethodID deleteSmartEidMethodID = env->GetMethodID(
			mJniServiceClz,
			jniService::deleteSmartEidMethod,
			jniService::deleteSmartEidMethodSig);
	jobject resultCode = env->CallObjectMethod(
			mJniServiceObj,
			deleteSmartEidMethodID);
	if (exceptionCheck(env)) {
		return EidServiceResult::ERROR;
	}

	jclass serviceResultClass = findClass(env, mApplicationContext, jniServiceResult::path);

	jmethodID getValueMethodID = env->GetStaticMethodID(
			serviceResultClass,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int serviceResultCode = env->CallStaticIntMethod(
			serviceResultClass,
			getValueMethodID,
			resultCode);

	if (exceptionCheck(env)) {
		return EidServiceResult::ERROR;
	}

	deleteLocalRef(env, resultCode);
	deleteLocalRef(env, serviceResultClass);

	return EidServiceResult(serviceResultCode);
}


/*!
 * Delete the eID-Applet from this device.
 *
 * \return EidServiceResult
 */
EidServiceResult EidAppletServiceAndroid::deletePersonalization() {

	const std::lock_guard<std::mutex> guard(serviceMutex);

	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	if (!getJNIEnvForCurrentThread(env, threadGuard) || mApplicationContext == nullptr || mJniServiceClz == nullptr || mJniServiceObj == nullptr) {
		return EidServiceResult::ERROR;
	}

	jmethodID deletePersonalizationMethodID = env->GetMethodID(
			mJniServiceClz,
			jniService::deletePersonalizationMethod,
			jniService::deletePersonalizationMethodSig);
	jobject resultCode = env->CallObjectMethod(
			mJniServiceObj,
			deletePersonalizationMethodID);
	if (exceptionCheck(env)) {
		return EidServiceResult::ERROR;
	}

	jclass serviceResultClass = findClass(env, mApplicationContext, jniServiceResult::path);
	jmethodID getValueMethodID = env->GetStaticMethodID(
			serviceResultClass,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int serviceResultCode = env->CallStaticIntMethod(
			serviceResultClass,
			getValueMethodID,
			resultCode);

	if (exceptionCheck(env)) {
		return EidServiceResult::ERROR;
	}

	deleteLocalRef(env, resultCode);
	deleteLocalRef(env, serviceResultClass);

	return EidServiceResult(serviceResultCode);
}


/*!
 * Performs APDU command
 *
 * \param pCommandApdu byte2hex encoded APDU
 * \return GenericDataResult with byte2hex encoded APDU response
 */
GenericDataResult EidAppletServiceAndroid::performAPDUCommand(
		const std::string& pCommandApdu) {

	const std::lock_guard<std::mutex> guard(serviceMutex);

	if (pCommandApdu.empty()) {
		return {EidServiceResult::ERROR, "command APDU should not be empty"};
	}

	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	if (!getJNIEnvForCurrentThread(env, threadGuard) || mApplicationContext == nullptr || mJniServiceClz == nullptr || mJniServiceObj == nullptr) {
		return {EidServiceResult::ERROR,
				"Current thread of the caller could not be attached"};
	}

	jmethodID performMethodID = env->GetMethodID(
			mJniServiceClz,
			jniService::performAPDUCommandMethod,
			jniService::performAPDUCommandMethodSig);
	jstring apdu = env->NewStringUTF(pCommandApdu.c_str());
	jobject obj = env->CallObjectMethod(mJniServiceObj, performMethodID, apdu);
	bool hasError = exceptionCheck(env);
	deleteLocalRef(env, apdu);

	if (hasError) {
		return {EidServiceResult::ERROR,
				"call of the performAPDUCommand function failed"};
	}

	auto clazz = env->GetObjectClass(obj);

	auto fidResult = env->GetFieldID(
			clazz,
			jniGenericDataResult::result,
			jniGenericDataResult::resultType);
	auto result = env->GetObjectField(obj, fidResult);

	auto fidData = env->GetFieldID(
			clazz,
			jniGenericDataResult::data,
			jniGenericDataResult::dataType);
	auto objField = env->GetObjectField(obj, fidData);
	auto jsData = reinterpret_cast<jstring>(objField);

	std::string data = getString(env, jsData);

	if (data.empty()) {
		return {EidServiceResult::ERROR, "response APDU should not be empty"};
	}

	jclass serviceResultClazz = findClass(env, mApplicationContext, jniServiceResult::path);
	jmethodID getValueMethodID = env->GetStaticMethodID(serviceResultClazz,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int serviceResultCode = env->CallStaticIntMethod(serviceResultClazz, getValueMethodID, result);
	GenericDataResult gDResult(getEidServiceResult(serviceResultCode), data);

	deleteLocalRef(env, result);
	deleteLocalRef(env, objField);
	deleteLocalRef(env, obj);
	deleteLocalRef(env, serviceResultClazz);
	deleteLocalRef(env, clazz);

	return gDResult;
}


/*!
 * Performs initialization of the Personalization
 *
 * \param pPin property is only relevant for iOS implementation and unused in Android
 * \param pChallenge challenge for key attestation
 * \return InitializeResult with public key and signed challenge
 */
InitializeResult EidAppletServiceAndroid::initializePersonalization(
		const std::string& pPin,
		const std::string& pChallenge) {

	const std::lock_guard<std::mutex> guard(serviceMutex);

	if (pPin.empty()) {
		// do nothing: Entering the eID-PIN is currently not necessary for Android here
	}

	if (pChallenge.empty()) {
		return {EidServiceResult::ERROR, "Challenge should not be empty"};
	}

	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	if (!getJNIEnvForCurrentThread(env, threadGuard) || mApplicationContext == nullptr || mJniServiceClz == nullptr || mJniServiceObj == nullptr) {
		return {EidServiceResult::ERROR,
				"Current thread of the caller could not be attached"};
	}

	jmethodID initializePersonalizationMethodID = env->GetMethodID(
			mJniServiceClz,
			jniService::initializePersonalizationMethod,
			jniService::initializePersonalizationMethodSig);
	jstring challenge = env->NewStringUTF(pChallenge.c_str());
	jobject obj = env->CallObjectMethod(
			mJniServiceObj,
			initializePersonalizationMethodID,
			challenge);

	deleteLocalRef(env, challenge);
	if (exceptionCheck(env)) {
		return {EidServiceResult::ERROR,
				"call of the initializePersonalization function failed"};
	}

	auto clazz = env->GetObjectClass(obj);
	auto fidResult = env->GetFieldID(clazz, jniInitializeResult::result,
			jniInitializeResult::resultType);
	auto result = env->GetObjectField(obj, fidResult);

	auto fidPPData = env->GetFieldID(clazz,
			jniInitializeResult::ppData,
			jniInitializeResult::ppDataType);

	auto objField = env->GetObjectField(obj, fidPPData);
	auto jsPPData = reinterpret_cast<jstring>(objField);

	if (exceptionCheck(env)) {
		return {EidServiceResult::ERROR,
				"call of the initializePersonalization function failed (cannot read result)"};
	}

	std::string ppData = getString(env, jsPPData);

	if (ppData.empty()) {
		return {EidServiceResult::ERROR, "PreparePersonalizationData should not be empty"};
	}
	jclass serviceResultClazz = findClass(env, mApplicationContext, jniServiceResult::path);
	jmethodID getValueMethodID = env->GetStaticMethodID(serviceResultClazz,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int serviceResultCode = env->CallStaticIntMethod(serviceResultClazz, getValueMethodID, result);
	InitializeResult initializeResult(getEidServiceResult(serviceResultCode), ppData);

	deleteLocalRef(env, result);
	deleteLocalRef(env, objField);
	deleteLocalRef(env, serviceResultClazz);
	deleteLocalRef(env, clazz);
	deleteLocalRef(env, obj);

	return initializeResult;
}


/*!
 * Performs personalization in a generic way controlled by Personalization Service
 *
 * \param pCommandPersonalization base64 encoded personalization step
 * \return GenericDataResult with base64 encoded personalization step response
 */
GenericDataResult EidAppletServiceAndroid::performPersonalization(const std::string& pCommandPersonalization) {

	const std::lock_guard<std::mutex> guard(serviceMutex);

	if (pCommandPersonalization.empty()) {
		return {EidServiceResult::ERROR, "personalization C-APDU should not be empty"};
	}

	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	if (!getJNIEnvForCurrentThread(env, threadGuard) || mApplicationContext == nullptr || mJniServiceClz == nullptr || mJniServiceObj == nullptr) {
		return {EidServiceResult::ERROR,
				"Current thread of the caller could not be attached"};
	}

	jmethodID performMethodID = env->GetMethodID(
			mJniServiceClz,
			jniService::performPersonalizationMethod,
			jniService::performPersonalizationMethodSig);
	jstring apdu = env->NewStringUTF(pCommandPersonalization.c_str());
	jobject obj = env->CallObjectMethod(mJniServiceObj, performMethodID, apdu);
	bool hasError = exceptionCheck(env);

	deleteLocalRef(env, apdu);

	if (hasError) {
		return {EidServiceResult::ERROR,
				"call of the performPersonalization function failed"};
	}

	auto clazz = env->GetObjectClass(obj);

	auto fidResult = env->GetFieldID(
			clazz,
			jniGenericDataResult::result,
			jniGenericDataResult::resultType);
	auto result = env->GetObjectField(obj, fidResult);

	auto fidData = env->GetFieldID(
			clazz,
			jniGenericDataResult::data,
			jniGenericDataResult::dataType);
	auto objField = env->GetObjectField(obj, fidData);
	auto jsData = reinterpret_cast<jstring>(objField);
	std::string data = getString(env, jsData);

	if (data.empty()) {
		return {EidServiceResult::ERROR, "response APDU should not be empty"};
	}
	jclass serviceResultClazz = findClass(env, mApplicationContext, jniServiceResult::path);
	jmethodID getValueMethodID = env->GetStaticMethodID(serviceResultClazz,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int serviceResultCode = env->CallStaticIntMethod(serviceResultClazz, getValueMethodID, result);
	GenericDataResult gDResult(getEidServiceResult(serviceResultCode), data);

	deleteLocalRef(env, result);
	deleteLocalRef(env, objField);
	deleteLocalRef(env, serviceResultClazz);
	deleteLocalRef(env, clazz);
	deleteLocalRef(env, obj);

	return gDResult;
}


/*!
 * Finalize the Personalization and provide the init-eID-PIN
 *
 * \return a PersonalizationResult object with the init-eID-PIN
 */
PersonalizationResult EidAppletServiceAndroid::finalizePersonalization(int status) {
	const std::lock_guard<std::mutex> guard(serviceMutex);

	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	if (!getJNIEnvForCurrentThread(env, threadGuard) || mApplicationContext == nullptr || mJniServiceClz == nullptr || mJniServiceObj == nullptr) {
		return {EidServiceResult::ERROR};
	}

	jmethodID finalizePersonalizationMethodID = env->GetMethodID(
			mJniServiceClz,
			jniService::finalizePersonalizationMethod,
			jniService::finalizePersonalizationMethodSig);
	jobject obj = env->CallObjectMethod(
			mJniServiceObj,
			finalizePersonalizationMethodID,
			status);
	if (exceptionCheck(env)) {
		return {EidServiceResult::ERROR};
	}

	auto clazz = env->GetObjectClass(obj);

	auto fidResult = env->GetFieldID(
			clazz,
			jniPersonalizationResult::result,
			jniPersonalizationResult::resultType);
	auto result = env->GetObjectField(obj, fidResult);

	auto fidPin = env->GetFieldID(
			clazz,
			jniPersonalizationResult::initPINData,
			jniPersonalizationResult::initPINDataType);
	auto objField = env->GetObjectField(obj, fidPin);
	auto jsData = reinterpret_cast<jstring>(objField);

	std::string pin = getString(env, jsData);

	jclass serviceResultClazz = findClass(env, mApplicationContext, jniServiceResult::path);
	jmethodID getValueMethodID = env->GetStaticMethodID(serviceResultClazz,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int serviceResultCode = env->CallStaticIntMethod(serviceResultClazz, getValueMethodID, result);
	PersonalizationResult personalizationResult(getEidServiceResult(serviceResultCode), pin);

	deleteLocalRef(env, result);
	deleteLocalRef(env, objField);
	deleteLocalRef(env, serviceResultClazz);
	deleteLocalRef(env, clazz);
	deleteLocalRef(env, obj);

	return personalizationResult;
}


/*!
 * Provides the status of the Smart-eID or eID-Applet.
 *
 * \return The status of the eID-Applet
 */
EidStatus EidAppletServiceAndroid::getSmartEidStatus() {

	const std::lock_guard<std::mutex> guard(serviceMutex);

	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	if (!getJNIEnvForCurrentThread(env, threadGuard) || mApplicationContext == nullptr || mJniServiceClz == nullptr || mJniServiceObj == nullptr) {
		return EidStatus::INTERNAL_ERROR;
	}

	jmethodID statusMethodID = env->GetMethodID(
			mJniServiceClz,
			jniService::eIDStatusMethod,
			jniService::eIDStatusMethodSig);

	jobject resultObj = env->CallObjectMethod(mJniServiceObj, statusMethodID);
	if (exceptionCheck(env)) {
		return EidStatus::INTERNAL_ERROR;
	}

	jclass statusClass = findClass(env, mApplicationContext, jniEidStatus::path);
	jmethodID getValueMethodID = env->GetStaticMethodID(
			statusClass,
			jniEidStatus::getValue,
			jniEidStatus::sigGetValue);

	int statusCode = env->CallStaticIntMethod(
			statusClass,
			getValueMethodID,
			resultObj);

	if (exceptionCheck(env)) {
		return EidStatus::INTERNAL_ERROR;
	}

	deleteLocalRef(env, resultObj);
	deleteLocalRef(env, statusClass);

	return EidStatus(statusCode);
}


/*!
 * Provides information of available updates of the installed eID-Applet and/or CSP
 * implementation or whether the device is supported by TSMS.
 *
 * \return The Support-Info of the eID-Applet
 */
EidSupportStatusResult EidAppletServiceAndroid::getSmartEidSupportInfo() {

	const std::lock_guard<std::mutex> guard(serviceMutex);

	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	if (!getJNIEnvForCurrentThread(env, threadGuard) || mApplicationContext == nullptr || mJniServiceClz == nullptr || mJniServiceObj == nullptr) {
		return {EidServiceResult::ERROR, EidSupportStatus::INTERNAL_ERROR};
	}

	jmethodID supportInfoMethodID = env->GetMethodID(
			mJniServiceClz,
			jniService::smartEidSupportInfo,
			jniService::smartEidSupportInfoMethodSig);

	jobject resultObj = env->CallObjectMethod(mJniServiceObj, supportInfoMethodID);
	if (exceptionCheck(env)) {
		return {EidServiceResult::ERROR, EidSupportStatus::INTERNAL_ERROR};
	}

	auto clazz = env->GetObjectClass(resultObj);

	auto fidResult = env->GetFieldID(
			clazz,
			jniEidSupportStatusResult::result,
			jniEidSupportStatusResult::resultType);
	auto result = env->GetObjectField(resultObj, fidResult);

	auto fidStatus = env->GetFieldID(
			clazz,
			jniEidSupportStatusResult::status,
			jniEidSupportStatusResult::statusType);
	auto status = env->GetObjectField(resultObj, fidStatus);
	if (exceptionCheck(env)) {
		return {EidServiceResult::ERROR, EidSupportStatus::INTERNAL_ERROR};
	}

	jclass serviceResultClazz = findClass(env, mApplicationContext, jniServiceResult::path);
	jmethodID serviceResultGetValueMethodID = env->GetStaticMethodID(serviceResultClazz,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int serviceResultCode = env->CallStaticIntMethod(serviceResultClazz, serviceResultGetValueMethodID, result);

	jclass supportStatusClazz = findClass(env, mApplicationContext, jniEidSupportStatus::path);
	jmethodID supportStatusGetValueMethodID = env->GetStaticMethodID(supportStatusClazz,
			jniEidSupportStatus::getValue,
			jniEidSupportStatus::sigGetValue);

	int supportStatusCode = env->CallStaticIntMethod(supportStatusClazz,
			supportStatusGetValueMethodID, status);
	EidSupportStatusResult supportStatusResult(getEidServiceResult(serviceResultCode), getEidSupportStatus(supportStatusCode));

	deleteLocalRef(env, result);
	deleteLocalRef(env, status);
	deleteLocalRef(env, supportStatusClazz);
	deleteLocalRef(env, serviceResultClazz);
	deleteLocalRef(env, clazz);
	deleteLocalRef(env, resultObj);

	return supportStatusResult;
}


/*!
 * Releases any open channel to the applet
 *
 * \return EidServiceResult
 */
EidServiceResult EidAppletServiceAndroid::releaseAppletConnection() {

	const std::lock_guard<std::mutex> guard(serviceMutex);

	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	if (!getJNIEnvForCurrentThread(env, threadGuard) || mApplicationContext == nullptr || mJniServiceClz == nullptr || mJniServiceObj == nullptr) {
		return EidServiceResult::ERROR;
	}

	jmethodID releaseAppletConnectionMethodID = env->GetMethodID(
			mJniServiceClz,
			jniService::releaseAppletConnectionMethod,
			jniService::releaseAppletConnectionMethodSig);
	jobject resultCode = env->CallObjectMethod(
			mJniServiceObj,
			releaseAppletConnectionMethodID);
	if (exceptionCheck(env)) {
		return EidServiceResult::ERROR;
	}

	jclass serviceResultClass = findClass(env, mApplicationContext, jniServiceResult::path);
	jmethodID getValueMethodID = env->GetStaticMethodID(
			serviceResultClass,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int serviceResultCode = env->CallStaticIntMethod(
			serviceResultClass,
			getValueMethodID,
			resultCode);

	if (exceptionCheck(env)) {
		return EidServiceResult::ERROR;
	}

	deleteLocalRef(env, resultCode);
	deleteLocalRef(env, serviceResultClass);

	return EidServiceResult(serviceResultCode);
}


/*!
 * Release all resources and shut down the eID-Applet-Service-Lib on Android
 *
 * \return GenericDataResult mData is blank if mResult is equal to
 *              EidServiceResult::SUCCESS, otherwise it contains an error message
 */
GenericDataResult EidAppletServiceAndroid::shutdownService() {

	const std::lock_guard<std::mutex> guard(serviceMutex);

	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	if (!getJNIEnvForCurrentThread(env, threadGuard) || mApplicationContext == nullptr || mJniServiceClz == nullptr || mJniServiceObj == nullptr) {
		return {EidServiceResult::ERROR,
				"Current thread of the caller could not be attached"};
	}

	jmethodID shutdownMethodID = env->GetMethodID(
			mJniServiceClz,
			jniService::shutdownMethod,
			jniService::shutdownMethodSig);
	if (exceptionCheck(env)) {
		return {EidServiceResult::ERROR,
				"Couldn't shutdown eID Applet Service. Method not found."};
	}

	jobject obj = env->CallObjectMethod(mJniServiceObj, shutdownMethodID);
	if (exceptionCheck(env)) {
		return {EidServiceResult::ERROR, "Couldn't shutdown eID Applet Service. Call of the shutdown function failed."};
	}

	auto clazz = env->GetObjectClass(obj);
	auto fidResult = env->GetFieldID(
			clazz,
			jniGenericDataResult::result,
			jniGenericDataResult::resultType);
	auto resultType = env->GetObjectField(obj, fidResult);

	auto fidData = env->GetFieldID(
			clazz,
			jniGenericDataResult::data,
			jniGenericDataResult::dataType);
	auto objField = env->GetObjectField(obj, fidData);
	auto jsData = reinterpret_cast<jstring>(objField);
	std::string data = getString(env, jsData);

	jclass serviceResultClazz = findClass(env, mApplicationContext, jniServiceResult::path);
	jmethodID getValueMethodID = env->GetStaticMethodID(serviceResultClazz,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int serviceResultCode = env->CallStaticIntMethod(serviceResultClazz, getValueMethodID,
			resultType);
	GenericDataResult gDResult(getEidServiceResult(serviceResultCode), data);

	deleteLocalRef(env, resultType);
	deleteLocalRef(env, objField);
	deleteLocalRef(env, serviceResultClazz);
	deleteLocalRef(env, clazz);
	deleteLocalRef(env, obj);

	return gDResult;
}


ServiceInformationResult EidAppletServiceAndroid::getServiceInformation() {

	const std::lock_guard<std::mutex> guard(serviceMutex);

	JNIEnv* env;
	ThreadGuard threadGuard(mJvm);
	ServiceInformationResult mServiceInfoResult;
	if (!getJNIEnvForCurrentThread(env, threadGuard) || mApplicationContext == nullptr || mJniServiceClz == nullptr || mJniServiceObj == nullptr) {
		mServiceInfoResult.mResult = EidServiceResult::ERROR;
		return mServiceInfoResult;
	}

	jmethodID updateInfoMethodID = env->GetMethodID(
			mJniServiceClz,
			jniService::getServiceInformationMethod,
			jniService::getServiceInformationMethodSig);

	jobject resultObj = env->CallObjectMethod(mJniServiceObj, updateInfoMethodID);
	if (exceptionCheck(env)) {
		mServiceInfoResult.mResult = EidServiceResult::ERROR;
		return mServiceInfoResult;
	}

	auto clazz = env->GetObjectClass(resultObj);

	auto fidResult = env->GetFieldID(
			clazz,
			jniServiceInformationResult::result,
			jniServiceInformationResult::resultType);
	auto result = env->GetObjectField(resultObj, fidResult);
	jclass serviceResultClazz = findClass(env, mApplicationContext, jniServiceResult::path);
	jmethodID gatValueMethodID = env->GetStaticMethodID(serviceResultClazz,
			jniServiceResult::getValue,
			jniServiceResult::sigGetValue);

	int returnValue = env->CallStaticIntMethod(serviceResultClazz, gatValueMethodID, result);

	mServiceInfoResult.mResult = static_cast<EidServiceResult>(returnValue);

	auto fidEidType = env->GetFieldID(
			clazz,
			jniServiceInformationResult::eidType,
			jniServiceInformationResult::eidTypeType);

	auto eidType = env->GetObjectField(resultObj, fidEidType);
	jclass eidTypeClazz = findClass(env, mApplicationContext, jniSmartEidType::path);
	jmethodID eidTypeStaticMethodID = env->GetStaticMethodID(eidTypeClazz,
			jniSmartEidType::getValue,
			jniSmartEidType::sigGetValue);

	int eidTypeValue = env->CallStaticIntMethod(eidTypeClazz, eidTypeStaticMethodID,
			eidType);

	mServiceInfoResult.mType = static_cast<SmartEidType>(eidTypeValue);

	auto fidChallenge = env->GetFieldID(
			clazz,
			jniServiceInformationResult::challengeData,
			jniServiceInformationResult::challengeDataType);

	auto challegeField = env->GetObjectField(resultObj, fidChallenge);
	auto jsData = reinterpret_cast<jstring>(challegeField);
	std::string challenge = getString(env, jsData);
	mServiceInfoResult.mChallengeType = challenge;

	auto fidLibVersionName = env->GetFieldID(
			clazz,
			jniServiceInformationResult::libVersionName,
			jniServiceInformationResult::libVersionNameType);

	auto versionNameField = env->GetObjectField(resultObj, fidLibVersionName);
	auto jsVersionName = reinterpret_cast<jstring>(versionNameField);
	std::string versionName = getString(env, jsVersionName);
	mServiceInfoResult.mLibVersionName = versionName;

	deleteLocalRef(env, result);
	deleteLocalRef(env, eidType);
	deleteLocalRef(env, challegeField);
	deleteLocalRef(env, versionNameField);
	deleteLocalRef(env, serviceResultClazz);
	deleteLocalRef(env, clazz);
	deleteLocalRef(env, resultObj);
	deleteLocalRef(env, eidTypeClazz);

	return mServiceInfoResult;
}


EidAppletServiceAndroid::ThreadGuard::ThreadGuard(JavaVM* pJvm)
	: mJvm(pJvm)
	, mDoDetach(false) {
}


EidAppletServiceAndroid::ThreadGuard::~ThreadGuard() {
	if (mDoDetach && mJvm) {
		mJvm->DetachCurrentThread();
	}
}


void EidAppletServiceAndroid::ThreadGuard::doDetach() {
	mDoDetach = true;
}


/*!
 * Returns the JNI Environment of the current thread. If the env is not attached it will be
 * attached. This is a save way to use the current JNIEnv, as it could be that the member
 * var env of the class is attached to another thread.
 *
 * \param _env the JNIEnv of the current thread (out param)
 * \param threadGuard who can be ordered to detach the current thread on destruction
 * \return true = success, false = error
 */
bool EidAppletServiceAndroid::getJNIEnvForCurrentThread(JNIEnv*& _env, ThreadGuard& threadGuard) {
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
		threadGuard.doDetach();

		return attachResult == JNI_OK;
	}

	return false;
}
