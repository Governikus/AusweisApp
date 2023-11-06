/*
 * Copyright (C) 2023 by Bundesdruckerei GmbH and Governikus GmbH & Co. KG
 * Licensed under the EUPL-1.2
 *
 * v0.23.0
 *
 * +------------------------------+--------------+---------------+---------------+
 * | Function               TOPIC | SE_CERTIFIED |  SE_ENDORSED  |  HW_KEYSTORE  |
 * +------------------------------+--------------+---------------+---------------+
 * |                      GENERAL |--------------+---------------+---------------|
 * | initializeService            |      X       |       X       |       X       |
 * | shutdownService              |      X       |       X       |       X       |
 * | getServiceInformation        |      X       |       X       |       X       |
 * | getSmartEidSupportInfo       |      X       |       X       |       X       |
 * | getSmartEidStatus            |      X       |       X       |       X       |
 * |           DEVICE PREPARATION |--------------+---------------+---------------|
 * | installSmartEid              |      X       |               |               |
 * | deleteSmartEid               |      X       |               |               |
 * |              PERSONALIZATION |--------------+---------------+---------------|
 * | initializePersonalization    |      X       |       X       |       X       |
 * | performPersonalization       |      X       |       X       |       X       |
 * | finalizePersonalization      |      X       |       X       |       X       |
 * | deletePersonalization        |      X       |       X       |       X       |
 * | releaseAppletConnection      |      X       |               |               |
 * |               IDENTIFICATION |--------------+---------------+---------------|
 * | performAPDUCommand           |      X       |       X       |       X 1)    |
 * | prepareIdentification        |              |               |       X 1)    |
 * | getChallenge                 |              |               |       X 1)    |
 * | performTAandCA               |              |               |       X 1)    |
 * +------------------------------+--------------+---------------+---------------+
 *
 * 1) performAPDUCommand is only partially implemented
      and is supplemented by additional functions.
 */

#pragma once

#include "eid_applet_results.h"

#include <functional>
#include <list>
#include <string>

#if defined(__ANDROID__)
	#include <jni.h>
#elif defined(__APPLE__)
	#include <TargetConditionals.h>
#endif


//! Handler for relaying progress notification obtained from TSM-API.

/*!
   \param progress (0..100) A number representing a percentage value
                   to indicate the relative progress of the action execution.
 */
using ProgressHandler = std::function<void (int progress)>;

// -------------------------------------------------------------------------------------------------
// GENERAL
// -------------------------------------------------------------------------------------------------

#if defined(__ANDROID__)

//! Performs initialization of eID-Applet-Service-Lib on Android. This method should be called from
//! the main thread / the thread that created the JVM

/*!
   \param env The android JNI Environment pointer.
   \param applicationContext The android application context.
   \param pServiceId The id of the service from the Trusted Service Management System.
   \param pSsdAid AID of the specific security domain that is created for the given
                 Trusted Service Management Service.
   \param pVersionTag Optional ServiceVersionTag for the Trusted Service Management usage.
                     By default, this string can be left blank. Passing a specific ServiceVersionTag
                     is required only in debug case.
   \return GenericDataResult mData is blank if mResult is equal to EidServiceResult::SUCCESS,
                            otherwise it contains an error message
 */
GenericDataResult initializeService(
	JNIEnv* env,
	jobject applicationContext,
	const std::string& pServiceId,
	const std::string& pSsdAid,
	const std::string& pVersionTag = "");

#else

//! Performs initialization of eID-Applet-Service-Lib.

/*!
   \return GenericDataResult mData is blank if mResult is equal to EidServiceResult::SUCCESS,
                            otherwise it contains an error message
 */
GenericDataResult initializeService();

#endif

//! Release all resources and shut down the eID-Applet-Service-Lib on Android

/*!
   \return GenericDataResult mData is blank if mResult is equal to EidServiceResult::SUCCESS,
                           otherwise it contains an error message
 */
GenericDataResult shutdownService();

//! Request the service information of the Smart-eID. This function does not include an online-check.

/*!
   \return ServiceInformationResult
 */
ServiceInformationResult getServiceInformation();

//! Provides information of available updates of the installed eID-Applet and/or CSP implementation
//! or whether the device is supported by Trusted Service Management System. The function includes
//! an online-check.

/*!
   \return EidSupportStatusResult with the supportInfo of the Smart-eID
                                 e.g. EidSupportStatus::UPDATE_AVAILABLE for the eID-Applet.
 */
EidSupportStatusResult getSmartEidSupportInfo();

//! Provides the current Smart-eID Status. This function does not include an online-check.

/*!
   \return The status of the Smart-eID e.g. EidStatus::NO_PERSONALIZATION for the eID-Applet.
 */
EidStatus getSmartEidStatus();

// -------------------------------------------------------------------------------------------------
// DEVICE PREPARATION
// -------------------------------------------------------------------------------------------------

//! Performs the remote provisioning of the eID-applet from the Trusted Service Management System
//! to the eSE on this device, or the ATM module initialization and the license check.

/*!
   \param pHandler function pointer of the implemented ProgressHandler function
   \return EidServiceResult
 */
EidServiceResult installSmartEid(const ProgressHandler& pHandler);

//! Performs remote deletion of the eID-applet in eSE on this device, or resets ATM module,
//! keychain items and Secure Enclave elements.

/*!
   \param pHandler function pointer of the implemented ProgressHandler function
   \return EidServiceResult
 */
EidServiceResult deleteSmartEid(const ProgressHandler& pHandler);

// -------------------------------------------------------------------------------------------------
// PERSONALIZATION
// -------------------------------------------------------------------------------------------------

//! Performs initialization of the Personalization

/*!
   \param pChallenge base64 encoded challenge for key attestation.
   \param pPin from AA2 validated 6 digit PIN, once the user entered the mobile-ID-PIN 2 times.
              It is only required for HW_KEYSTORE and will be ignored in all other cases.
   \return InitializeResult with base64 encoded public key and signed challenge
 */
InitializeResult initializePersonalization(
	const std::string& pChallenge,
	const std::string& pPin = "");

//! Performs personalization in a generic way controlled by Personalization Service

/*!
   \param pCommand byte2hex encoded Command e.g APDU for Android
   \return GenericDataResult with byte2hex encoded command response for the personalization step
 */
GenericDataResult performPersonalization(const std::string& pCommand);

//! Finalize the personalization flow and provide the init-eID-PIN as a return value. This function call
//! also closes the channel to the Supplementary Security Domain from the Service Provider and adjusts
//! the EidStatus in an error event accordingly.
//! Thus, this function must also be called in the event of an error, e.g. if personalization flow has
//! been interrupted.

/*!
   \param status The status code of the performed personalization.
   \return PersonalizationResult mInitPin contains the initial-eID-PIN used for personalization.
                                If the mInitPIN is blank, a new personalization must be started.
 */
PersonalizationResult finalizePersonalization(int status = 0);

//! Delete Personalization from eID Applet.

/*!
   \return EidServiceResult
 */
EidServiceResult deletePersonalization();

//! Closes any open channel to the SE.

/*!
   \return EidServiceResult
 */
EidServiceResult releaseAppletConnection();

// -------------------------------------------------------------------------------------------------
// IDENTIFICATION
// -------------------------------------------------------------------------------------------------

//! Performs APDU command

/*!
   \param pCommandApdu byte2hex encoded APDU
   \return GenericDataResult with byte2hex encoded APDU response
 */
GenericDataResult performAPDUCommand(const std::string& pCommandApdu);

//! Prepares the identification and returns the PrepareIdentificationResult.

/*!
   \param pChat CertificateHolderAuthorizationTemplate according to EAC1OutputType in TR-03112-7 3.6.4.1.
               byte2hex encoded. Example: 7f4c12060904007f0007030102025305000513ff00
   \return UserAuthenticationResult according to EAC1OutputType in TR-03112-7 3.6.4.1., byte2hex encoded
 */
PrepareIdentificationResult prepareIdentification(const std::string& pChat);

//! Get Challenge

/*!
   \return GenericDataResult with challenge according to EAC1OutputType in TR-03112-7 3.6.4.1.
          byte2hex encoded and terminated with 9000.
 */
GenericDataResult getChallenge();

//! Performs the terminal and chip authentication.

/*!
   \param pTerminalCvcChain List representing the terminal certificate chain according to EAC1InputType in TR-03112-7 3.6.4.1.
                           The first element is the terminal certificate, the last element is the certificate signed by the
                           certificate referenced by the CAR in the prepareIdentification response.
                           The elements of the list are byte2hex encoded. Example:
                           7f218201487f4e8201005f29010042104445445674494447564b333030303132
                           7f494f060a04007f00070202020203864104a7ba9a8cd0f294ea653ab42cb713
                           54af775d6fa98091dfe3af602cfe3837225a2e8573384b16d6fc9215815a9c47
                           fbdd3fb0224a184a6146198d7ee5c77837585f200e444544454d4f5041413030
                           3436347f4c12060904007f0007030102025305000513ff075f25060201000301
                           015f2406020100040100655e732d060904007f00070301030180204ebb52e497
                           c3549ca1102ecf55b6626c1afb00d2cdfcad369d37083ece26139e732d060904
                           007f00070301030280206ccf8efd02e71b274c8c4f29122310ef2d7ffdfb4c61
                           1fe267f8576da42e7ba25f37402b096b45c029b2184cba8d745431a6820e4bcb
                           b7ba14d3c7745dddec147cc1d208fe0547ebcc44e7384a52aafb39f7d83e43e6
                           15ad9a22b84cd911e75171e555
   \param pAuxiliaryData Authenticated Auxiliary Data according to EAC1InputType in TR-03112-7 3.6.4.1.
                        byte2hex encoded. Example: 67177315060904007f00070301040253083230323130333136
   \param pSignature Signature according to EAC2InputType TR-03112-7 3.6.4.2.
                    byte2hex encoded. Example:
                    0d472d904137c057a9d7c34d675413326050549f71fc04aa625791dc5debedca
                    20d6dce02bc11c2ad6b0b749e9440099924b429101255dfdb02029e720f06714
   \param pPin from AA2 validated 6 digit PIN, once the user entered the mobile-ID-PIN 2 times.
   \param pEphemeralPublicKey EphemeralPublicKey according to EAC2InputType TR-03112-7 3.6.4.2.
                             byte2hex encoded. Example:
                             045e5297e977a637b30834632934d1e00ade870053d740d64a5df9efb938bd29c4
                             682b803fc5857fc9ffe6aae16e4254c02b2cc8d861226501e152776954d6643d
   \return TAandCAResult according to EAC2OutputType TR-03112-7 3.6.4.2., byte2hex encoded
 */
TAandCAResult performTAandCA(
	const std::list<std::string>& pTerminalCvcChain,
	const std::string& pAuxiliaryData,
	const std::string& pSignature,
	const std::string& pPin,
	const std::string& pEphemeralPublicKey);
