/*
 * Copyright (C) 2021 Bundesdruckerei GmbH and Governikus GmbH
 *
 * v0.17.0
 */

#pragma once

#include <string>

#if defined(__APPLE__)
	#include <TargetConditionals.h>
#endif


/**
 * States of the eID-Applet and Smart-eID.
 */
enum class EidStatus : int {
	///< The device is not supported or the initial online-check
	///< with function #getUpdateInfo() has not yet been executed.
	UNAVAILABLE = 0x0100010,
	///< The device is support but a provisioning is required.
	NO_PROVISIONING = 0x0100020,
	///< The eID-Applet is installed and a personalization of
	///< Smart eID is required.
	NO_PERSONALIZATION = 0x0100030,
	///< Smart eID is either not personalized or personalized
	///< but not accessible, the eID-Applet must be deleted.
	APPLET_UNUSABLE = 0x0100050,
	///< The eID-Applet is installed, Smart eID is personalized
	///< and accessible; the Smart eID is ready to be used.
	///< The status bytes of MSE SET AT (PACE) will provide the
	///< current PIN state (initial, operational or blocked).
	PERSONALIZED = 0x0300010,
	///< Smart-eID Status could not be detected due to an
	///< internal error.
	INTERNAL_ERROR = 0x1100000
};

/**
 * Update-Info of the eID-Applet and Smart-eID.
 */
enum class EidUpdateInfo : int {
	///< The device is not supported.
	UNAVAILABLE = 0x0400010,
	///< Device is supported, but the update status cannot be
	///< detected because no eID-Applet is currently installed.
	NO_PROVISIONING = 0x0400020,
	///< A new version of the already installed eID-Applet or
	///< CSP implementation or both is available and may be
	///< installed. The eID-Applet is either
	///< in state EidStatus::PROVISIONED or in one of
	///< EidStatus::PERSONALIZED.
	UPDATE_AVAILABLE = 0x0400030,
	///< The installed eID-Applet is up to date.
	UP_TO_DATE = 0x0400040,
	///< Update-Info could not be acquired due to an
	///< internal error.
	INTERNAL_ERROR = 0x2100000
};

enum class EidServiceResult : int {
	SUCCESS = 0x00000000,
	UNDEFINED = 0x30000000,
	INFO = 0x31000000,
	WARN = 0x32000000,
	ERROR = 0x33000000,
	UNSUPPORTED = 0x34000000
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


	EidServiceResult mResult;
	std::string mData;
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

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR

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

#endif
