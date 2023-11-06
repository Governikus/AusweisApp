/*
 * Copyright (C) 2023 by Bundesdruckerei GmbH and Governikus GmbH & Co. KG
 * Licensed under the EUPL-1.2
 */

#pragma once

#include "eid_applet_interface.h"
#include "eid_applet_results.h"
#include "eid_applet_utils.h"
#include <jni.h>
#include <mutex>
#include <stdexcept>


using namespace appletUtils;
using namespace jniUtils;


class EidAppletServiceAndroid {
	public:
		EidAppletServiceAndroid(JNIEnv* env, jobject applicationContext);
		~EidAppletServiceAndroid();

		GenericDataResult initializeService(
			const std::string& pServiceId,
			const std::string& pSsdAid,
			const std::string& pVersionTag);
		EidServiceResult installSmartEid();
		EidServiceResult deleteSmartEid();
		EidServiceResult deletePersonalization();
		GenericDataResult performAPDUCommand(const std::string& pCommandApdu);
		InitializeResult initializePersonalization(const std::string& pPin, const std::string& pChallenge);
		GenericDataResult performPersonalization(const std::string& pCommandPersonalization);
		PersonalizationResult finalizePersonalization(jint status);
		EidStatus getSmartEidStatus();
		EidSupportStatusResult getSmartEidSupportInfo();
		EidServiceResult releaseAppletConnection();
		GenericDataResult shutdownService();
		ServiceInformationResult getServiceInformation();

	private:
		class ThreadGuard {
			private:
				JavaVM* mJvm;
				bool mDoDetach;

			public:
				explicit ThreadGuard(JavaVM* pJvm);
				~ThreadGuard();

				void doDetach();
		};

		std::mutex serviceMutex;

		JNIEnv* mEnv;
		JavaVM* mJvm;
		jobject mApplicationContext = nullptr;
		jclass mJniServiceClz = nullptr;
		jobject mJniServiceObj = nullptr;

		bool getJNIEnvForCurrentThread(JNIEnv*& _env, ThreadGuard& threadGuard);
};
