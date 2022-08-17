/*
 * \copyright Copyright (c) 2021 Governikus GmbH & Co. KG, Germany
 */

#include "eid_applet_interface_mock.h"

#include "MockSmartEidRestClient.h"

#include <QSharedPointer>

#include <cassert>
#include <iostream>


QSharedPointer<governikus::MockSmartEidRestClient> mRestInterface(new governikus::MockSmartEidRestClient);

struct Data
{
	bool ephemeralResult = true;
	EidUpdateInfo updateInfo = mRestInterface->isEnabled() ? EidUpdateInfo::UP_TO_DATE : EidUpdateInfo::UNAVAILABLE;
	EidStatus smartEidStatus = mRestInterface->isEnabled() ? EidStatus::NO_PERSONALIZATION : EidStatus::UNAVAILABLE;
	EidServiceResult installSmartEidResult = mRestInterface->isEnabled() ? EidServiceResult::SUCCESS : EidServiceResult::UNSUPPORTED;
	EidServiceResult deleteSmartEidResult = mRestInterface->isEnabled() ? EidServiceResult::SUCCESS : EidServiceResult::UNSUPPORTED;
	GenericDataResult apduCommandResult;
	GenericDataResult personalizationResult;
	InitializeResult initializePersonalizationResult;
	EidServiceResult deletePersonalizationResult = mRestInterface->isEnabled() ? EidServiceResult::SUCCESS : EidServiceResult::UNSUPPORTED;
}
mData;


void governikus::setEphemeralResult(bool pEphemeral)
{
	mData.ephemeralResult = pEphemeral;
}


void governikus::setUpdateInfo(EidUpdateInfo pStatus)
{
	mData.updateInfo = pStatus;
}


EidUpdateInfo getUpdateInfo()
{
	return mData.updateInfo;
}


void governikus::setSmartEidStatus(EidStatus pStatus)
{
	mData.smartEidStatus = pStatus;
}


EidStatus getSmartEidStatus()
{
	return mData.smartEidStatus;
}


void governikus::setInstallSmartEidResult(EidServiceResult pResult)
{
	mData.installSmartEidResult = pResult;
}


EidServiceResult installSmartEid(const ProgressHandler&)
{
	if (mData.installSmartEidResult == EidServiceResult::SUCCESS)
	{
		governikus::setSmartEidStatus(EidStatus::NO_PERSONALIZATION);
	}
	return mData.installSmartEidResult;
}


void governikus::setDeleteSmartEidResult(EidServiceResult pResult)
{
	mData.deleteSmartEidResult = pResult;
}


EidServiceResult deleteSmartEid(const ProgressHandler&)
{
	if (mData.deleteSmartEidResult == EidServiceResult::SUCCESS)
	{
		governikus::setSmartEidStatus(EidStatus::NO_PROVISIONING);
	}
	return mData.deleteSmartEidResult;
}


void governikus::setApduCommandResult(const GenericDataResult& pResult)
{
	// Assert that last setApduCommandResult was received by performAPDUCommand
	assert(!mData.ephemeralResult || mData.apduCommandResult.mResult == EidServiceResult::UNDEFINED);
	mData.apduCommandResult = pResult;
}


GenericDataResult performAPDUCommand(const std::string& pCommandApdu)
{
	std::cout << "Mock result for:" << pCommandApdu << std::endl;
	const auto result = mData.apduCommandResult;
	if (mData.ephemeralResult)
	{
		mData.apduCommandResult = GenericDataResult();
	}
	return result;
}


void governikus::setPersonalizationResult(const GenericDataResult& pResult)
{
	// Assert that last setPersonalizationResult was received by performPersonalization
	assert(!mData.ephemeralResult || mData.personalizationResult.mResult == EidServiceResult::UNDEFINED);
	mData.personalizationResult = pResult;
}


GenericDataResult performPersonalization(const std::string& pCommandPersonalization)
{
	std::cout << "Mock result for:" << pCommandPersonalization << std::endl;

	if (mRestInterface->isEnabled())
	{
		return mRestInterface->sendPersonalizationCommand(pCommandPersonalization);
	}

	const auto result = mData.personalizationResult;
	if (mData.ephemeralResult)
	{
		mData.personalizationResult = GenericDataResult();
	}
	return result;
}


void governikus::setInitializePersonalizationResult(const InitializeResult& pResult)
{
	// Assert that last setPrePersonalizationResult was received by initializePrePersonalization
	assert(!mData.ephemeralResult || mData.initializePersonalizationResult.mResult == EidServiceResult::UNDEFINED);
	mData.initializePersonalizationResult = pResult;
}


InitializeResult initializePersonalization(const std::string& pChallenge, const std::string& pPin)
{
	std::cout << "Mock result for:" << pChallenge << " | " << pPin << std::endl;

	if (mRestInterface->isEnabled())
	{
		return mRestInterface->createSession();
	}

	const auto result = mData.initializePersonalizationResult;
	if (mData.ephemeralResult)
	{
		mData.initializePersonalizationResult = InitializeResult();
	}
	return result;
}


void governikus::setDeletePersonalizationResult(EidServiceResult pResult)
{
	mData.deletePersonalizationResult = pResult;
}


EidServiceResult deletePersonalization()
{
	if (mData.deletePersonalizationResult == EidServiceResult::SUCCESS)
	{
		governikus::setSmartEidStatus(EidStatus::NO_PERSONALIZATION);
	}
	return mData.deletePersonalizationResult;
}


#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR

PrepareIdentificationResult prepareIdentification(const std::string& pChat)
{
	(void) pChat;
	return {EidServiceResult::ERROR, "", "", ""};
}


GenericDataResult getChallenge()
{
	return {EidServiceResult::ERROR, ""};
}


TAandCAResult performTAandCA(
		const std::list<std::string>& pTerminalCvcChain,
		const std::string& pAuxiliaryData,
		const std::string& pSignature,
		const std::string& pPin,
		const std::string& pEphemeralPublicKey)
{
	(void) pTerminalCvcChain;
	(void) pAuxiliaryData;
	(void) pSignature;
	(void) pPin;
	(void) pEphemeralPublicKey;
	return {EidServiceResult::ERROR, "", "", ""};
}


#elif defined(__ANDROID__)

GenericDataResult initializeService(JNIEnv* env, jobject applicationContext)
{
	(void) env;
	(void) applicationContext;
	return {EidServiceResult::ERROR, ""};
}


PersonalizationResult finalizePersonalization()
{
	if (mRestInterface->isEnabled())
	{
		return mRestInterface->deleteSession();
	}
	return {EidServiceResult::ERROR, ""};
}


EidServiceResult releaseAppletConnection()
{
	return EidServiceResult::ERROR;
}


GenericDataResult shutdownService()
{
	return {EidServiceResult::ERROR, ""};
}


#endif

void governikus::initMock()
{
	mRestInterface.reset(new MockSmartEidRestClient());
	mData = Data {};
}
