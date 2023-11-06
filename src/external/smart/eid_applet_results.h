/*
 * Copyright (C) 2023 by Bundesdruckerei GmbH and Governikus GmbH & Co. KG
 * Licensed under the EUPL-1.2
 *
 * v0.23.0
 */

#pragma once

#include <string>


/**
 * States of the eID-Applet and Smart-eID.
 */
enum class EidStatus : int {
	///< The Smart eID is not provisioned on this device. The support of
	///< the device is unknown. Nothing is currently installed or initialized.
	///< Applet solution: The eID-Applet is not installed.
	///< Non-Applet solution: No KeyPair is present on the device.
	NO_PROVISIONING = 0x0100020,
	///< The Smart eID is provisioned on this device. A personalization is
	///< necessary.
	///< Applet solution: The eID-Applet is installed; a personalization of
	///< Smart eID is required.
	///< Non-Applet solution: KeyPair is present; a personalization of
	///< Smart eID is required.
	NO_PERSONALIZATION = 0x0100030,
	///< The Smart eID is provisioned on this device but is not accessible
	///< or is corrupted. The Smart eID must be deleted.
	///< Applet solution: The eID-Applet is installed, Smart eID is either not
	///< personalized or personalized but not accessible; the eID-Applet
	///< must be deleted
	///< Non-Applet solution: Smart eID is not accessible due to no longer
	///< fulfilled security requirements.
	UNUSABLE = 0x0100050,
	///< The Smart eID is provisioned, personalized and accessible on this
	///< device. The Smart eID can be used.
	///< Applet solution: The eID-Applet is installed, personalized and
	///< accessible.
	///< Non-Applet solution: The non-Applet Solution is personalized and
	///< accessible.
	PERSONALIZED = 0x0300010,
	///< The required certificates for the Smart eID are expired
	///< Applet solution: not applicable
	///< Non-applet solution: The Smart eID is temporarily disabled. The
	///< certificates must be refreshed to turn the Smart eID fully functional
	///< again.
	CERT_EXPIRED = 0x0100060,
	///< Request could not be processed successfully and the Smart eID
	///< state could not be determined. Process must be repeated.
	INTERNAL_ERROR = 0x1100000
};

/**
 * Support Status of the eID-Applet and Smart-eID.
 */
enum class EidSupportStatus : int {
	///< The device is not supported.
	///< Applet solution: The device is not supported by TSMS
	///< Non-Applet solution: The device is not supported e.g., does not
	///< offer required security mechanisms
	UNAVAILABLE = 0x0400010,
	///< The device is supported for the Smart eID. Currently nothing is
	///< provisioned
	///< Applet solution: The device is supported by TSMS.
	///< Non-Applet solution: The device is supported and fulfills all
	///< security requirements for the non-Applet solution
	AVAILABLE = 0x0400020,
	///< The Smart eID is supported, already provisioned and up to date
	///< on this device.
	///< Applet solution: Latest supported version of the eID-Applet is
	///< already installed.
	///< Non-Applet solution: All required certificates and keys are up to
	///< date.
	UP_TO_DATE = 0x0400040,
	///< The Smart eID is supported and already provisioned on this
	///< device but a new version is available. It is recommended to delete
	///< and reinstall the Smart eID so that the new version is provisioned.
	///< Applet solution: A new version of the eID-Applet or CSP
	///< implementation or both is available and may be installed on the
	///< device.
	///< Non-Applet solution: A Non-Applet solution is currently used on
	///< the device. The device is now also supported by the TSMS.
	///< Provisioning of the applet solution is available.
	UPDATE_AVAILABLE = 0x0400030,
	///< The online check could not be processed successfully e.g., due to
	///< a network issue. Process must be repeated.
	INTERNAL_ERROR = 0x2100000
};

enum class EidServiceResult : int {
	SUCCESS = 0x00000000,
	UNDEFINED = 0x30000000,
	INFO = 0x31000000,
	WARN = 0x32000000,
	ERROR = 0x33000000,
	UNSUPPORTED = 0x34000000,
	OVERLOAD_PROTECTION = 0x35000000,
	UNDER_MAINTENANCE = 0x36000000,
	NFC_NOT_ACTIVATED = 0x37000000,
	INTEGRITY_CHECK_FAILED = 0x38000000,
	NOT_AUTHENTICATED = 0x39000000,
	NETWORK_CONNECTION_ERROR = 0x40000000
};

enum class SmartEidType : int {
	UNKNOWN = 0x02000010,
	APPLET = 0x02000020,
	NON_APPLET = 0x02000030
};

/**
 * Generic return value for functions.
 */
struct GenericDataResult {
	GenericDataResult(
		EidServiceResult result = EidServiceResult::UNDEFINED,
		const std::string& basicString = "")
		: mResult(result), mData(basicString) {
	}


	EidServiceResult mResult = EidServiceResult::UNDEFINED;
	std::string mData;
};

/**
 * Return value for the getSmartEidSupportInfo.
 */
struct EidSupportStatusResult {
	EidSupportStatusResult(
		EidServiceResult result = EidServiceResult::UNDEFINED,
		EidSupportStatus status = EidSupportStatus::INTERNAL_ERROR)
		: mResult(result), mStatus(status) {
	}


	EidServiceResult mResult = EidServiceResult::UNDEFINED;
	EidSupportStatus mStatus = EidSupportStatus::INTERNAL_ERROR;
};

/**
 * mPreparePersonalizationData is blank if mResult is not equal to EidServiceResult::SUCCESS.
 */
struct InitializeResult {
	InitializeResult(
		EidServiceResult result = EidServiceResult::UNDEFINED,
		const std::string& preparePersonalizationData = "")
		: mResult(result), mPreparePersonalizationData(preparePersonalizationData) {
	}


	EidServiceResult mResult = EidServiceResult::UNDEFINED;
	std::string mPreparePersonalizationData;
};

/**
 * Return value for a finalized personalization.
 */
struct PersonalizationResult {
	PersonalizationResult(
		EidServiceResult result = EidServiceResult::UNDEFINED,
		const std::string& initPIN = "")
		: mResult(result), mInitPIN(initPIN) {
	}


	EidServiceResult mResult = EidServiceResult::UNDEFINED;
	std::string mInitPIN;
};

/**
 * Return value for the service information of the Smart-eID.
 */
struct ServiceInformationResult {
	ServiceInformationResult(
		EidServiceResult result = EidServiceResult::UNDEFINED,
		SmartEidType type = SmartEidType::UNKNOWN,
		const std::string& challengeType = "",
		const std::string& libVersionName = "")
		: mResult(result), mType(type), mChallengeType(challengeType), mLibVersionName(libVersionName) {
	}


	EidServiceResult mResult = EidServiceResult::UNDEFINED;
	SmartEidType mType = SmartEidType::UNKNOWN;
	std::string mChallengeType;
	std::string mLibVersionName;
};

/**
 * mCertificationAuthorityReference, mEfCardAccess and mIdIcc are blank
 * if mResult is not equal to EidServiceResult::SUCCESS.
 */
struct PrepareIdentificationResult {
	EidServiceResult mResult = EidServiceResult::UNDEFINED;
	std::string mCertificationAuthorityReference;
	std::string mEfCardAccess;
	std::string mIdIcc;
};

/**
 * mEfCardSecurity, mAuthenticationToken and mNonce are blank
 * if mResult is not equal to EidServiceResult::SUCCESS.
 */
struct TAandCAResult {
	EidServiceResult mResult = EidServiceResult::UNDEFINED;
	std::string mEfCardSecurity;
	std::string mAuthenticationToken;
	std::string mNonce;
};
