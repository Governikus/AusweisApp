/*
 * Copyright (C) 2023 by Bundesdruckerei GmbH and Governikus GmbH & Co. KG
 * Licensed under the EUPL-1.2
 */

#pragma once

#include "eid_applet_results.h"
#include <jni.h>
#include <string>

namespace jniService {
	// constructor
	const char*const path = "de/bundesdruckerei/android/eid_applet_service_lib/jni/SmartEidServiceNativeBridge";
	const char*const stdInit = "<init>";

	// methods
	const char*const init = "initialize";
	const char*const sig = "(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Lde/bundesdruckerei/android/eid_applet_service_lib/GenericDataResult;";

	const char*const shutdownMethod = "shutdown";
	const char*const shutdownMethodSig = "()Lde/bundesdruckerei/android/eid_applet_service_lib/GenericDataResult;";

	const char*const performAPDUCommandMethod = "performAPDUCommand";
	const char*const performAPDUCommandMethodSig = "(Ljava/lang/String;)Lde/bundesdruckerei/android/eid_applet_service_lib/GenericDataResult;";

	const char*const installSmartEidMethod = "installSmartEid";
	const char*const installSmartEidMethodSig = "()Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;";

	const char*const deleteSmartEidMethod = "deleteSmartEid";
	const char*const deleteSmartEidMethodSig = "()Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;";

	const char*const initializePersonalizationMethod = "initializePersonalization";
	const char*const initializePersonalizationMethodSig = "(Ljava/lang/String;)Lde/bundesdruckerei/android/eid_applet_service_lib/InitializeResult;";

	const char*const performPersonalizationMethod = "performPersonalization";
	const char*const performPersonalizationMethodSig = "(Ljava/lang/String;)Lde/bundesdruckerei/android/eid_applet_service_lib/GenericDataResult;";

	const char*const releaseAppletConnectionMethod = "releaseAppletConnection";
	const char*const releaseAppletConnectionMethodSig = "()Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;";

	const char*const eIDStatusMethod = "smartEidStatus";
	const char*const eIDStatusMethodSig = "()Lde/bundesdruckerei/android/eid_applet_service_lib/EidStatus;";

	const char*const smartEidSupportInfo = "smartEidSupportInfo";
	const char*const smartEidSupportInfoMethodSig = "()Lde/bundesdruckerei/android/eid_applet_service_lib/EidSupportStatusResult;";

	const char*const finalizePersonalizationMethod = "finalizePersonalization";
	const char*const finalizePersonalizationMethodSig = "(I)Lde/bundesdruckerei/android/eid_applet_service_lib/PersonalizationResult;";

	const char*const deletePersonalizationMethod = "deletePersonalization";
	const char*const deletePersonalizationMethodSig = "()Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;";

	const char*const getServiceInformationMethod = "serviceInformation";
	const char*const getServiceInformationMethodSig = "()Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceInformationResult;";
} // namespace jniService

namespace jniGenericDataResult {
	const char*const path = "de/bundesdruckerei/android/eid_applet_service_lib/GenericDataResult";
	const char*const sig = "(Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;Ljava/lang/String;)V";
	const char*const init = "<init>";
	const char*const result = "result";
	const char*const resultType = "Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;";
	const char*const data = "data";
	const char*const dataType = "Ljava/lang/String;";
} // namespace jniGenericDataResult

namespace jniServiceResult {
	const char*const init = "<init>";
	const char*const sig = "()V";
	const char*const path = "de/bundesdruckerei/android/eid_applet_service_lib/jni/ServiceResultMap";
	const char*const sigGetByValue = "(I)Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;";
	const char*const getByValue = "getByValue";
	const char*const sigGetValue = "(Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;)I";
	const char*const getValue = "getValue";
} // namespace jniServiceResult

namespace jniSmartEidType {
	const char*const init = "<init>";
	const char*const sig = "()V";
	const char*const path = "de/bundesdruckerei/android/eid_applet_service_lib/jni/SmartEidTypeMap";
	const char*const sigGetByValue = "(I)Lde/bundesdruckerei/android/eid_applet_service_lib/SmartEidType;";
	const char*const getByValue = "getByValue";
	const char*const sigGetValue = "(Lde/bundesdruckerei/android/eid_applet_service_lib/SmartEidType;)I";
	const char*const getValue = "getValue";
} // namespace jniSmartEidType

namespace jniEidStatus {
	const char*const path = "de/bundesdruckerei/android/eid_applet_service_lib/jni/EidStatusMap";
	const char*const sigGetByValue = "(I)Lde/bundesdruckerei/android/eid_applet_service_lib/EidStatus;";
	const char*const getByValue = "getByValue";
	const char*const sigGetValue = "(Lde/bundesdruckerei/android/eid_applet_service_lib/EidStatus;)I";
	const char*const getValue = "getValue";
} // namespace jniEidStatus

namespace jniEidSupportStatusResult {
	const char*const path = "de/bundesdruckerei/android/eid_applet_service_lib/EidSupportStatusResult";
	const char*const sig = "(Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;Lde/bundesdruckerei/android/eid_applet_service_lib/EidSupportStatus;)V";
	const char*const init = "<init>";
	// ServiceResult
	const char*const result = "result";
	const char*const resultType = "Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;";
	// EidSupportInfo
	const char*const status = "supportStatus";
	const char*const statusType = "Lde/bundesdruckerei/android/eid_applet_service_lib/EidSupportStatus;";
} // namespace jniEidSupportStatusResult

namespace jniEidSupportStatus {
	const char*const path = "de/bundesdruckerei/android/eid_applet_service_lib/jni/EidSupportStatusMap";
	const char*const init = "<init>";
	const char*const sig = "()V";
	const char*const sigGetByValue = "(I)Lde/bundesdruckerei/android/eid_applet_service_lib/EidSupportStatus;";
	const char*const getByValue = "getByValue";
	const char*const sigGetValue = "(Lde/bundesdruckerei/android/eid_applet_service_lib/EidSupportStatus;)I";
	const char*const getValue = "getValue";
} // namespace jniEidSupportStatus

namespace jniInitializeResult {
	const char*const path = "de/bundesdruckerei/android/eid_applet_service_lib/InitializeResult";
	const char*const sig = "(Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;Ljava/lang/String;)V";
	const char*const init = "<init>";
	const char*const result = "result";
	const char*const resultType = "Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;";
	const char*const ppData = "preparePersonalizationData";
	const char*const ppDataType = "Ljava/lang/String;";
} // namespace jniInitializeResult

namespace jniPersonalizationResult {
	const char*const path = "de/bundesdruckerei/android/eid_applet_service_lib/PersonalizationResult";
	const char*const sig = "(Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;Ljava/lang/String;)V";
	const char*const init = "<init>";
	const char*const result = "result";
	const char*const resultType = "Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;";
	const char*const initPINData = "initPIN";
	const char*const initPINDataType = "Ljava/lang/String;";
} // namespace jniPersonalizationResult

namespace jniServiceInformationResult {
	const char*const path = "de/bundesdruckerei/android/eid_applet_service_lib/ServiceInformationResult";
	const char*const sig = "(Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;Lde/bundesdruckerei/android/eid_applet_service_lib/SmartEidType;Ljava/lang/String;Ljava/lang/String;)V";
	const char*const init = "<init>";
	const char*const result = "result";
	const char*const resultType = "Lde/bundesdruckerei/android/eid_applet_service_lib/ServiceResult;";
	const char*const eidType = "type";
	const char*const eidTypeType = "Lde/bundesdruckerei/android/eid_applet_service_lib/SmartEidType;";
	const char*const challengeData = "challengeType";
	const char*const challengeDataType = "Ljava/lang/String;";
	const char*const libVersionName = "libVersionName";
	const char*const libVersionNameType = "Ljava/lang/String;";
} // namespace jniServiceInformationResult

namespace jniUtils {
	std::string getString(JNIEnv* env, const jstring& dataJString);
	bool exceptionCheck(JNIEnv* env);
	jclass findClass(JNIEnv* env, jobject applicationContext, const char* className);

	template<typename T>
	void deleteLocalRef(JNIEnv* env, T*& ref) {
		if (ref && env) {
			env->DeleteLocalRef(ref);
			ref = nullptr;
		}
	}


	template<typename T>
	void deleteGlobalRef(JNIEnv* env, T*& ref) {
		if (ref && env) {
			env->DeleteGlobalRef(ref);
			ref = nullptr;
		}
	}


} // namespace jniUtils

namespace appletUtils {

	EidServiceResult getEidServiceResult(int value);

	EidSupportStatus getEidSupportStatus(int value);

} // namespace appletUtils
