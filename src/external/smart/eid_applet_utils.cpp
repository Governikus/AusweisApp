/*
 * Copyright (C) 2023 by Bundesdruckerei GmbH and Governikus GmbH & Co. KG
 * Licensed under the EUPL-1.2
 */

#include "eid_applet_utils.h"
#include <string>


std::string jniUtils::getString(JNIEnv* env, const jstring& dataJString) {
	const jsize length = env->GetStringUTFLength(dataJString);
	const char* charStr = env->GetStringUTFChars(dataJString, nullptr);
	if (charStr != nullptr) {
		std::string data(charStr, static_cast<unsigned long>(length));
		env->ReleaseStringUTFChars(dataJString, charStr);
		return data;
	}
	return "";
}


bool jniUtils::exceptionCheck(JNIEnv* env) {
	if (env->ExceptionCheck()) {
		env->ExceptionDescribe();
		env->ExceptionClear();
		return true;
	}
	return false;
}


/**
 * This findclass-method is used instead of the jni native method to avoid Crashes on Qt based applications since
 * Qt on Android uses a custom class loader
 * https://doc.qt.io/qt-5/qandroidjnienvironment.html#findClass
 * */
jclass jniUtils::findClass(JNIEnv* env, jobject applicationContext, const char* className) {
	jclass acl = env->GetObjectClass(applicationContext);
	jmethodID getClassLoader = env->GetMethodID(acl, "getClassLoader",
			"()Ljava/lang/ClassLoader;");
	jobject cls = env->CallObjectMethod(applicationContext, getClassLoader);
	jclass classLoader = env->FindClass("java/lang/ClassLoader");
	jmethodID findClass = env->GetMethodID(classLoader,
			"loadClass",
			"(Ljava/lang/String;)Ljava/lang/Class;");

	jstring strClassName = env->NewStringUTF(className);
	jclass jniClzRef = static_cast<jclass>(env->CallObjectMethod(cls, findClass, strClassName));
	env->DeleteLocalRef(strClassName);
	env->DeleteLocalRef(classLoader);
	env->DeleteLocalRef(cls);
	env->DeleteLocalRef(acl);

	return jniClzRef;
}


EidServiceResult appletUtils::getEidServiceResult(int value) {
	for (const auto result : {EidServiceResult::SUCCESS,
							  EidServiceResult::INFO,
							  EidServiceResult::WARN,
							  EidServiceResult::ERROR,
							  EidServiceResult::UNSUPPORTED,
							  EidServiceResult::OVERLOAD_PROTECTION,
							  EidServiceResult::UNDER_MAINTENANCE,
							  EidServiceResult::NFC_NOT_ACTIVATED,
							  EidServiceResult::INTEGRITY_CHECK_FAILED,
							  EidServiceResult::NOT_AUTHENTICATED,
							  EidServiceResult::NETWORK_CONNECTION_ERROR}) {
		if (static_cast<int>(result) == value) {
			return result;
		}
	}

	return EidServiceResult::UNDEFINED;
}


EidSupportStatus appletUtils::getEidSupportStatus(int value) {
	for (const auto result : {EidSupportStatus::AVAILABLE,
							  EidSupportStatus::UNAVAILABLE,
							  EidSupportStatus::UPDATE_AVAILABLE,
							  EidSupportStatus::UP_TO_DATE}) {
		if (static_cast<int>(result) == value) {
			return result;
		}
	}

	return EidSupportStatus::INTERNAL_ERROR;
}
