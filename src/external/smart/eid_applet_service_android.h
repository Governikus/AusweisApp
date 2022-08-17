/*
 * Copyright (C) 2021 Bundesdruckerei GmbH and Governikus GmbH
 *
 */

#pragma once

#include "eid_applet_interface.h"
#include "eid_applet_results.h"
#include "eid_applet_utils.h"
#include <jni.h>
#include <stdexcept>


using namespace appletUtils;
using namespace jniUtils;


template<class JNIEnvironment, class JavaVirtualMachine>
class EidAppletServiceAndroid {
	public:
		EidAppletServiceAndroid(JNIEnvironment* env, jobject applicationContext);
		~EidAppletServiceAndroid();

		GenericDataResult initializeService(
			const std::string& pServiceId,
			const std::string& pVersionTag,
			const std::string& pSsdAid);
		EidServiceResult installSmartEid();
		EidServiceResult deleteSmartEid();
		EidServiceResult deletePersonalization();
		GenericDataResult performAPDUCommand(const std::string& pCommandApdu);
		InitializeResult initializePersonalization(const std::string& pPin, const std::string& pChallenge);
		GenericDataResult performPersonalization(const std::string& pCommandPersonalization);
		PersonalizationResult finalizePersonalization();
		EidStatus getSmartEidStatus();
		EidUpdateInfo getUpdateInfo();
		EidServiceResult releaseAppletConnection();
		GenericDataResult shutdownService();

	private:
		class ThreadGuard {
			private:
				JavaVirtualMachine* mJvm;
				bool mDoDetach;

			public:
				explicit ThreadGuard(JavaVirtualMachine* pJvm);
				~ThreadGuard();

				void doDetach();
		};

		JNIEnvironment* mEnv;
		JavaVirtualMachine* mJvm;
		jclass mJniServiceClz;
		jobject mJniServiceObj;
		jobject mApplicationContext;

		bool getJNIEnvForCurrentThread(JNIEnvironment*& _env, ThreadGuard& guard);
};
