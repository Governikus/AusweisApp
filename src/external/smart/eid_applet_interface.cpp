//
// Copyright (C) 2021 Bundesdruckerei GmbH
//

#include "eid_applet_interface.h"
#include "eid_applet_service_android.h"
#include "eid_applet_utils.h"
#include <string>


using namespace appletUtils;


#if defined(__ANDROID__)

EidAppletServiceAndroid<JNIEnv, JavaVM>* eidAppletService = nullptr;

//! Static ProgressHandler object
static ProgressHandler progressHandler = nullptr;

//! Provides the current Smart-eID Status. This function does not include an online-check.

/*!
   \return The status of the Smart-eID e.g. EidStatus::NO_PERSONALIZATION for the eID-Applet.
 */
EidStatus getSmartEidStatus() {
	if (eidAppletService) {
		return eidAppletService->getSmartEidStatus();
	}

	return EidStatus::INTERNAL_ERROR;
}


//! Provides information of available updates of the installed eID-Applet and/or CSP implementation
//! or whether the device is supported by Trusted Service Management System. The function includes
//! an online-check.

/*!
   \return The updateInfo of the Smart-eID e.g. EidUpdateInfo::UPDATE_AVAILABLE for the eID-Applet.
 */
EidUpdateInfo getUpdateInfo() {
	if (eidAppletService) {
		return eidAppletService->getUpdateInfo();
	}

	return EidUpdateInfo::INTERNAL_ERROR;
}


//! Performs initialization of eID-Applet-Service-Lib on Android. This method should be called from
//! the main thread / the thread that created the JVM

/*!
   \param env The android JNI Environment pointer.
   \param applicationContext The android application context.
   \return GenericDataResult mData is blank if mResult is equal to EidServiceResult::SUCCESS,
                            otherwise it contains an error message
 */
GenericDataResult initializeService(
		JNIEnv* env,
		jobject applicationContext,
		const std::string& pServiceId,
		const std::string& pVersionTag,
		const std::string& pSsdAid) {
	if (eidAppletService) {
		return {EidServiceResult::ERROR, "Service is already initialized"};
	}

	eidAppletService = new EidAppletServiceAndroid<JNIEnv, JavaVM>(env, applicationContext);
	return eidAppletService->initializeService(pServiceId, pVersionTag, pSsdAid);
}


//! Performs APDU command

/*!
   \param pCommandApdu byte2hex encoded APDU
   \return GenericDataResult with byte2hex encoded APDU response
 */
GenericDataResult performAPDUCommand(const std::string& pCommandApdu) {
	if (eidAppletService) {
		return eidAppletService->performAPDUCommand(pCommandApdu);
	}

	return {EidServiceResult::ERROR, "Service not initialized"};
}


//! Performs the remote provisioning of the eID-applet from the Trusted Service Management System
//! to the eSE on this device, or the ATM module initialization and the license check.

/*!
   \param pHandler function pointer of the implemented ProgressHandler function
   \return EidServiceResult
 */
EidServiceResult installSmartEid(const ProgressHandler& pHandler) {
	if (eidAppletService) {
		progressHandler = pHandler;
		EidServiceResult result = eidAppletService->installSmartEid();
		progressHandler = nullptr;
		return result;
	}

	return EidServiceResult::ERROR;
}


//! Performs remote deletion of the eID-applet in eSE on this device, or resets ATM module,
//! keychain items and Secure Enclave elements.

/*!
   \param pHandler function pointer of the implemented ProgressHandler function
   \return EidServiceResult
 */
EidServiceResult deleteSmartEid(const ProgressHandler& pHandler) {
	if (eidAppletService) {
		progressHandler = pHandler;
		EidServiceResult result = eidAppletService->deleteSmartEid();
		progressHandler = nullptr;
		return result;
	}

	return EidServiceResult::ERROR;
}


//! Delete Personalization from eID Applet.

/*!
   \return EidServiceResult
 */
EidServiceResult deletePersonalization() {
	if (eidAppletService) {
		return eidAppletService->deletePersonalization();
	}

	return EidServiceResult::ERROR;
}


//! Performs personalization in a generic way controlled by Personalization Service

/*!
   \param pCommand byte2hex encoded Command e.g APDU for Android
   \return GenericDataResult with byte2hex encoded command response for the personalization step
 */
GenericDataResult performPersonalization(const std::string& pCommand) {
	if (eidAppletService) {
		return eidAppletService->performPersonalization(pCommand);
	}

	return {EidServiceResult::ERROR, "Service not initialized"};
}


//! Performs initialization of the Personalization

/*!
   \param pChallenge base64 encoded challenge for key attestation.
   \param pPin from AA2 validated 6 digit PIN, once the user entered the mobile-ID-PIN 2 times.
              It is only required for HW_KEYSTORE and will be ignored in all other cases.
   \return InitializeResult with base64 encoded public key and signed challenge
 */
InitializeResult initializePersonalization(const std::string& pChallenge, const std::string& pPin) {
	if (eidAppletService) {
		return eidAppletService->initializePersonalization(pPin, pChallenge);
	}

	return {EidServiceResult::ERROR};
}


//! Closes any open channel to the SE.

/*!
   \return EidServiceResult
 */
EidServiceResult releaseAppletConnection() {
	if (eidAppletService) {
		return eidAppletService->releaseAppletConnection();
	}

	return EidServiceResult::ERROR;
}


//! Finalize the personalization flow and provide the init-eID-PIN as a return value. This function call
//! also closes the channel to the Supplementary Security Domain from the Service Provider.
//! Thus, this function must also be called in the event of an error, e.g. if personalization flow has
//! been interrupted.

/*!
   \return PersonalizationResult mInitPin contains the initial-eID-PIN used for personalization.
                                If the mInitPIN is blank, a new personalization must be started.
 */
PersonalizationResult finalizePersonalization() {
	if (eidAppletService) {
		return eidAppletService->finalizePersonalization();
	}

	return {EidServiceResult::ERROR};
}


//! Release all resources and shut down the eID-Applet-Service-Lib on Android

/*!
   \return GenericDataResult mData is blank if mResult is equal to EidServiceResult::SUCCESS,
                           otherwise it contains an error message
 */
GenericDataResult shutdownService() {
	if (eidAppletService) {
		GenericDataResult result = eidAppletService->shutdownService();
		delete eidAppletService;
		eidAppletService = nullptr;
		return result;
	}

	return {EidServiceResult::ERROR, "Service not initialized"};
}


#endif


/**
 * function for relaying the info if handler is present
 */

extern "C"
JNIEXPORT void JNICALL Java_de_bdr_eid_1applet_1service_1lib_tsm_ProgressUpdater_notify(JNIEnv* env __unused,
		jobject thiz __unused, jint progress) {
	if (progressHandler != nullptr) {
		progressHandler(progress);
	}
}
