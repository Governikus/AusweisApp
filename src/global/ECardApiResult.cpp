/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ECardApiResult.h"

#include "LanguageLoader.h"

#include <QDebug>
#include <QStringBuilder>

using namespace governikus;

constexpr const char* RESULTMAJOR = "http://www.bsi.bund.de/ecard/api/1.1/resultmajor";
constexpr const char* RESULTMINOR = "http://www.bsi.bund.de/ecard/api/1.1/resultminor";

const QMap<ECardApiResult::Major, QString> ECardApiResult::cMajorResults = {
	{ECardApiResult::Major::Ok, QLatin1String(RESULTMAJOR) + QLatin1String("#ok")},
	{ECardApiResult::Major::Warning, QLatin1String(RESULTMAJOR) + QLatin1String("#warning")},
	{ECardApiResult::Major::Error, QLatin1String(RESULTMAJOR) + QLatin1String("#error")}
};

// See TR-03112-1 Section 4.2 Codes for errors and warnings
//      AL -> Applicaction Layer
//      DP -> Dispatcher
//      IFDL -> Interface Device Layer
//      IL -> Identify Layer
//      KEY
//      SAL -> Service Access Layer
const QMap<ECardApiResult::Minor, QString> ECardApiResult::cMinorResults = {
	{ECardApiResult::Minor::AL_Unknown_Error, QLatin1String(RESULTMINOR) + QLatin1String("/al/common#unknownError")},
	{ECardApiResult::Minor::AL_No_Permission, QLatin1String(RESULTMINOR) + QLatin1String("/al/common#noPermission")},
	{ECardApiResult::Minor::AL_Internal_Error, QLatin1String(RESULTMINOR) + QLatin1String("/al/common#internalError")},
	{ECardApiResult::Minor::AL_Parameter_Error, QLatin1String(RESULTMINOR) + QLatin1String("/al/common#parameterError")},
	{ECardApiResult::Minor::AL_Unknown_API_Function, QLatin1String(RESULTMINOR) + QLatin1String("/al/common#unknownAPIFunction")},
	{ECardApiResult::Minor::AL_Not_Initialized, QLatin1String(RESULTMINOR) + QLatin1String("/al/common#notInitialized")},
	{ECardApiResult::Minor::AL_Warning_Connection_Disconnected, QLatin1String(RESULTMINOR) + QLatin1String("/al/common#warningConnectionDisconnected")},
	{ECardApiResult::Minor::AL_Session_Terminated_Warning, QLatin1String(RESULTMINOR) + QLatin1String("/al/common#SessionTerminatedWarning")},
	{ECardApiResult::Minor::AL_Communication_Error, QLatin1String(RESULTMINOR) + QLatin1String("/al/common#communicationError")},
	{ECardApiResult::Minor::DP_Timeout_Error, QLatin1String(RESULTMINOR) + QLatin1String("/dp#timeout")},
	{ECardApiResult::Minor::DP_Unknown_Channel_Handle, QLatin1String(RESULTMINOR) + QLatin1String("/dp#unknownChannelHandle")},
	{ECardApiResult::Minor::DP_Communication_Error, QLatin1String(RESULTMINOR) + QLatin1String("/dp#communicationError")},
	{ECardApiResult::Minor::DP_Trusted_Channel_Establishment_Failed, QLatin1String(RESULTMINOR) + QLatin1String("/dp#trustedChannelEstablishmentFailed")},
	{ECardApiResult::Minor::DP_Unknown_Protocol, QLatin1String(RESULTMINOR) + QLatin1String("/dp#unknownProtocol")},
	{ECardApiResult::Minor::DP_Unknown_Cipher_Suite, QLatin1String(RESULTMINOR) + QLatin1String("/dp#unknownCipherSuite")},
	{ECardApiResult::Minor::DP_Unknown_Webservice_Binding, QLatin1String(RESULTMINOR) + QLatin1String("/dp#unknownWebserviceBinding")},
	{ECardApiResult::Minor::DP_Node_Not_Reachable, QLatin1String(RESULTMINOR) + QLatin1String("/dp#nodeNotReachable")},
	{ECardApiResult::Minor::IFDL_Timeout_Error, QLatin1String(RESULTMINOR) + QLatin1String("/ifdl/common#timeoutError")},
	{ECardApiResult::Minor::IFDL_UnknownSlot, QLatin1String(RESULTMINOR) + QLatin1String("/ifdl/terminal#unknownSlot")},
	{ECardApiResult::Minor::IFDL_InvalidSlotHandle, QLatin1String(RESULTMINOR) + QLatin1String("/ifdl/common#invalidSlotHandle")},
	{ECardApiResult::Minor::IFDL_CancellationByUser, QLatin1String(RESULTMINOR) + QLatin1String("/ifdl#cancellationByUser")},
	{ECardApiResult::Minor::IFDL_IFD_SharingViolation, QLatin1String(RESULTMINOR) + QLatin1String("/ifdl/terminal#IFDSharingViolation")},
	{ECardApiResult::Minor::IFDL_Terminal_NoCard, QLatin1String(RESULTMINOR) + QLatin1String("/ifdl/terminal#noCard")},
	{ECardApiResult::Minor::IFDL_IO_RepeatedDataMismatch, QLatin1String(RESULTMINOR) + QLatin1String("/ifdl/IO#repeatedDataMismatch")},
	{ECardApiResult::Minor::IFDL_IO_UnknownPINFormat, QLatin1String(RESULTMINOR) + QLatin1String("/ifdl/IO#unknownPINFormat")},
	{ECardApiResult::Minor::IL_Signature_InvalidCertificatePath, QLatin1String(RESULTMINOR) + QLatin1String("/il/signature#invalidCertificatePath")},
	{ECardApiResult::Minor::KEY_KeyGenerationNotPossible, QLatin1String(RESULTMINOR) + QLatin1String("/il/key#keyGenerationNotPossible")},
	{ECardApiResult::Minor::SAL_Cancellation_by_User, QLatin1String(RESULTMINOR) + QLatin1String("/sal#cancellationByUser")},
	{ECardApiResult::Minor::SAL_Invalid_Key, QLatin1String(RESULTMINOR) + QLatin1String("/sal#invalidKey")},
	{ECardApiResult::Minor::SAL_SecurityConditionNotSatisfied, QLatin1String(RESULTMINOR) + QLatin1String("/sal#securityConditionNotSatisfied")},
	{ECardApiResult::Minor::SAL_MEAC_AgeVerificationFailedWarning, QLatin1String(RESULTMINOR) + QLatin1String("/sal/mEAC#AgeVerificationFailedWarning")},
	{ECardApiResult::Minor::SAL_MEAC_CommunityVerificationFailedWarning, QLatin1String(RESULTMINOR) + QLatin1String("/sal/mEAC#CommunityVerificationFailedWarning")},
	{ECardApiResult::Minor::SAL_MEAC_DocumentValidityVerificationFailed, QLatin1String(RESULTMINOR) + QLatin1String("/sal/mEAC#DocumentValidityVerificationFailed")},
};

QMap<GlobalStatus::Code, ECardApiResult::Minor> ECardApiResult::cConversionMap1 = {};
QMap<ECardApiResult::Minor, GlobalStatus::Code> ECardApiResult::cConversionMap2 = {};


ECardApiResult ECardApiResult::fromStatus(const GlobalStatus& pStatus, const std::optional<FailureCode>& pFailureCode)
{
	if (pStatus.getStatusCode() == GlobalStatus::Code::No_Error)
	{
		return createOk();
	}

	const QString& message = pStatus.toErrorDescription();
	const Minor minor = fromStatus(pStatus.getStatusCode());
	const Origin status = fromStatus(pStatus.getOrigin());

	return ECardApiResult(Major::Error, minor, message, status, pFailureCode);
}


ECardApiResult ECardApiResult::createOk()
{
	return ECardApiResult(Major::Ok, Minor::null);
}


void ECardApiResult::initConversionMaps()
{
	if (!cConversionMap1.isEmpty() || !cConversionMap2.isEmpty())
	{
		return;
	}

	addConversionElement(GlobalStatus::Code::No_Error, Minor::null);

	addConversionElement(GlobalStatus::Code::Paos_Error_AL_Unknown_Error, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Paos_Unexpected_Warning, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Unknown_Error, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Card_Unexpected_Transmit_Status, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Card_NewPin_Invalid_Length, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Workflow_AlreadyInProgress_Error, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Wrong_Length_Error, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Card_Not_Found, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Card_Communication_Error, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Card_Input_TimeOut, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Card_Pin_Blocked, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Card_Pin_Not_Blocked, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Card_NewPin_Mismatch, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Card_ValidityVerificationFailed, Minor::SAL_MEAC_DocumentValidityVerificationFailed);
	addConversionElement(GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::IfdConnector_InvalidRequest, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::IfdConnector_NoSupportedApiLevel, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::IfdConnector_ConnectionTimeout, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::IfdConnector_ConnectionError, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::IfdConnector_RemoteHostRefusedConnection, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Downloader_File_Not_Found, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Downloader_Cannot_Save_File, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Downloader_Data_Corrupted, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Downloader_Missing_Platform, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Downloader_Aborted, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Update_Execution_Failed, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Card_Smart_Invalid, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Smart_eID_Unavailable, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Smart_eID_Authentication_Failed, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Smart_eID_ServiceInformation_Query_Failed, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Smart_eID_PrePersonalization_Failed, Minor::AL_Unknown_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Smart_eID_Personalization_Failed, Minor::AL_Unknown_Error);

	addConversionElement(GlobalStatus::Code::Workflow_No_Permission_Error, Minor::AL_No_Permission);
	addConversionElement(GlobalStatus::Code::Card_Pin_Deactivated, Minor::AL_No_Permission);
	addConversionElement(GlobalStatus::Code::Card_Puk_Blocked, Minor::AL_No_Permission);
	addConversionElement(GlobalStatus::Code::Workflow_Smart_eID_Personalization_Denied, Minor::AL_No_Permission);

	addConversionElement(GlobalStatus::Code::Paos_Error_AL_Internal_Error, Minor::AL_Internal_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Cannot_Confirm_IdCard_Authenticity, Minor::AL_Internal_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Unknown_Paos_From_EidServer, Minor::AL_Internal_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Start_Paos_Response_Missing, Minor::AL_Internal_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer, Minor::AL_Internal_Error);
	addConversionElement(GlobalStatus::Code::Card_Protocol_Error, Minor::AL_Internal_Error);

	addConversionElement(GlobalStatus::Code::Workflow_Certificate_No_Description, Minor::AL_Parameter_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Certificate_No_Url_In_Description, Minor::AL_Parameter_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Certificate_Hash_Error, Minor::AL_Parameter_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Certificate_Sop_Error, Minor::AL_Parameter_Error);

	addConversionElement(GlobalStatus::Code::Paos_Error_AL_Communication_Error, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Communication_Missing_Redirect_Url, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Browser_Transmission_Error, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Reader_Became_Inaccessible, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Network_Ssl_Establishment_Error, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Network_Ssl_Hash_Not_In_Certificate_Description, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Network_Empty_Redirect_Url, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Network_Expected_Redirect, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Network_Invalid_Scheme, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Network_Malformed_Redirect_Url, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Network_ServiceUnavailable, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Network_ServerError, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Network_ClientError, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Network_TimeOut, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Network_Proxy_Error, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Network_Other_Error, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Workflow_Wrong_Parameter_Invocation, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Card_Invalid_Pin, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Card_Invalid_Can, Minor::AL_Communication_Error);
	addConversionElement(GlobalStatus::Code::Card_Invalid_Puk, Minor::AL_Communication_Error);

	addConversionElement(GlobalStatus::Code::Paos_Error_DP_Trusted_Channel_Establishment_Failed, Minor::DP_Trusted_Channel_Establishment_Failed);
	addConversionElement(GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error, Minor::DP_Trusted_Channel_Establishment_Failed);
	addConversionElement(GlobalStatus::Code::Workflow_TrustedChannel_Server_Error, Minor::DP_Trusted_Channel_Establishment_Failed);
	addConversionElement(GlobalStatus::Code::Workflow_TrustedChannel_Client_Error, Minor::DP_Trusted_Channel_Establishment_Failed);
	addConversionElement(GlobalStatus::Code::Workflow_TrustedChannel_Hash_Not_In_Description, Minor::DP_Trusted_Channel_Establishment_Failed);
	addConversionElement(GlobalStatus::Code::Workflow_TrustedChannel_No_Data_Received, Minor::DP_Trusted_Channel_Establishment_Failed);
	addConversionElement(GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Connection_Unsupported_Algorithm_Or_Length, Minor::DP_Trusted_Channel_Establishment_Failed);
	addConversionElement(GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Certificate_Unsupported_Algorithm_Or_Length, Minor::DP_Trusted_Channel_Establishment_Failed);
	addConversionElement(GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable, Minor::DP_Trusted_Channel_Establishment_Failed);
	addConversionElement(GlobalStatus::Code::Workflow_TrustedChannel_TimeOut, Minor::DP_Trusted_Channel_Establishment_Failed);
	addConversionElement(GlobalStatus::Code::Workflow_TrustedChannel_Proxy_Error, Minor::DP_Trusted_Channel_Establishment_Failed);
	addConversionElement(GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error, Minor::DP_Trusted_Channel_Establishment_Failed);
	addConversionElement(GlobalStatus::Code::Workflow_TrustedChannel_Other_Network_Error, Minor::DP_Trusted_Channel_Establishment_Failed);

	addConversionElement(GlobalStatus::Code::Workflow_No_Unique_DvCvc, ECardApiResult::Minor::IL_Signature_InvalidCertificatePath);
	addConversionElement(GlobalStatus::Code::Workflow_No_Unique_AtCvc, ECardApiResult::Minor::IL_Signature_InvalidCertificatePath);
	addConversionElement(GlobalStatus::Code::Workflow_Preverification_Error, ECardApiResult::Minor::IL_Signature_InvalidCertificatePath);
	addConversionElement(GlobalStatus::Code::Workflow_Preverification_Developermode_Error, ECardApiResult::Minor::IL_Signature_InvalidCertificatePath);

	addConversionElement(GlobalStatus::Code::Paos_Error_SAL_Cancellation_by_User, Minor::SAL_Cancellation_by_User);
	addConversionElement(GlobalStatus::Code::Workflow_InternalError_BeforeTcToken, Minor::SAL_Cancellation_by_User);
	addConversionElement(GlobalStatus::Code::Workflow_Cancellation_By_User, Minor::SAL_Cancellation_by_User);
	addConversionElement(GlobalStatus::Code::Card_Cancellation_By_User, Minor::SAL_Cancellation_by_User);
	addConversionElement(GlobalStatus::Code::Workflow_Card_Removed, Minor::SAL_Cancellation_by_User);

	addConversionElement(GlobalStatus::Code::Paos_Error_SAL_Invalid_Key, Minor::SAL_Invalid_Key);

	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::AL_Unknown_API_Function);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::AL_Not_Initialized);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::AL_Warning_Connection_Disconnected);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::AL_Session_Terminated_Warning);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::DP_Timeout_Error);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::DP_Unknown_Channel_Handle);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::DP_Communication_Error);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::DP_Unknown_Protocol);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::DP_Unknown_Cipher_Suite);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::DP_Unknown_Webservice_Binding);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::DP_Node_Not_Reachable);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::IFDL_Timeout_Error);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::IFDL_UnknownSlot);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::IFDL_InvalidSlotHandle);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::IFDL_CancellationByUser);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::IFDL_IFD_SharingViolation);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::IFDL_Terminal_NoCard);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::IFDL_IO_RepeatedDataMismatch);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::IFDL_IO_UnknownPINFormat);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::KEY_KeyGenerationNotPossible);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::SAL_SecurityConditionNotSatisfied);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::SAL_MEAC_AgeVerificationFailedWarning);
	addConversionElement(GlobalStatus::Code::Paos_Generic_Server_Error, Minor::SAL_MEAC_CommunityVerificationFailedWarning);
}


void ECardApiResult::addConversionElement(const GlobalStatus::Code pCode, const Minor pMinor)
{
	if (!cConversionMap1.contains(pCode))
	{
		cConversionMap1[pCode] = pMinor;
	}
	else
	{
		Q_ASSERT(pCode == GlobalStatus::Code::Paos_Generic_Server_Error);
	}

	if (!cConversionMap2.contains(pMinor))
	{
		cConversionMap2[pMinor] = pCode;
	}
}


GlobalStatus::Code ECardApiResult::toStatus(const Minor pMinor)
{
	if (pMinor == Minor::null)
	{
		return GlobalStatus::Code::Paos_Error_AL_Unknown_Error;
	}

	initConversionMaps();

	if (!cConversionMap2.contains(pMinor))
	{
		qCritical() << "Critical conversion mismatch for" << pMinor;
		Q_ASSERT(false);
		return GlobalStatus::Code::Unknown_Error;
	}

	return cConversionMap2.value(pMinor);
}


ECardApiResult::Minor ECardApiResult::fromStatus(const GlobalStatus::Code pCode)
{
	initConversionMaps();

	if (!cConversionMap1.contains(pCode))
	{
		qCritical() << "Critical conversion mismatch for" << pCode;
		Q_ASSERT(false);
		return Minor::AL_Unknown_Error;
	}

	return cConversionMap1.value(pCode);
}


GlobalStatus::Origin ECardApiResult::toStatus(const ECardApiResult::Origin pSelf)
{
	switch (pSelf)
	{
		case governikus::ECardApiResult::Origin::Client:
			return governikus::GlobalStatus::Origin::Client;

		case governikus::ECardApiResult::Origin::Server:
			return governikus::GlobalStatus::Origin::Server;
	}

	Q_UNREACHABLE();
}


ECardApiResult::Origin ECardApiResult::fromStatus(const GlobalStatus::Origin pSelf)
{
	switch (pSelf)
	{
		case governikus::GlobalStatus::Origin::Client:
			return governikus::ECardApiResult::Origin::Client;

		case governikus::GlobalStatus::Origin::Server:
			return governikus::ECardApiResult::Origin::Server;
	}

	Q_UNREACHABLE();
}


ECardApiResult::Major ECardApiResult::parseMajor(const QString& pMajor)
{
	for (auto iter = cMajorResults.cbegin(); iter != cMajorResults.cend(); ++iter)
	{
		if (pMajor == iter.value())
		{
			return iter.key();
		}
	}

	if (!pMajor.isEmpty())
	{
		qWarning() << "Unknown ResultMajor:" << pMajor;
	}
	return Major::Unknown;
}


ECardApiResult::Minor ECardApiResult::parseMinor(const QString& pMinor)
{
	for (auto iter = cMinorResults.cbegin(); iter != cMinorResults.cend(); ++iter)
	{
		if (pMinor == iter.value())
		{
			return iter.key();
		}
	}

	if (!pMinor.isEmpty())
	{
		qWarning() << "Unknown ResultMinor:" << pMinor;
	}

	return Minor::null;
}


bool ECardApiResult::isMajor(const QString& major)
{
	return Major::Unknown != parseMajor(major);
}


bool ECardApiResult::isMinor(const QString& minor)
{
	return Minor::null != parseMinor(minor);
}


QString ECardApiResult::getMessage(Minor pMinor)
{
	switch (pMinor)
	{
		case Minor::AL_Unknown_Error:
			//: LABEL ALL_PLATFORMS
			return tr("An unexpected error has occurred during processing.");

		case Minor::AL_No_Permission:
			//: LABEL ALL_PLATFORMS
			return tr("Use of the function by the client application is not permitted.");

		case Minor::AL_Internal_Error:
			//: LABEL ALL_PLATFORMS
			return tr("An internal error has occurred during processing.");

		case Minor::AL_Parameter_Error:
			//: LABEL ALL_PLATFORMS
			return tr("There was some problem with a provided or omitted parameter.");

		case Minor::AL_Unknown_API_Function:
			//: LABEL ALL_PLATFORMS
			return tr("The API function is unknown.");

		case Minor::AL_Not_Initialized:
			//: LABEL ALL_PLATFORMS
			return tr("The framework or layer has not been initialized.");

		case Minor::AL_Warning_Connection_Disconnected:
		case Minor::AL_Session_Terminated_Warning:
			//: LABEL ALL_PLATFORMS
			return tr("The active session has been terminated.");

		case Minor::AL_Communication_Error:
			//: LABEL ALL_PLATFORMS
			return tr("A Communication error occurred during processing.");

		case Minor::DP_Timeout_Error:
			//: LABEL ALL_PLATFORMS
			return tr("The operation was terminated as the set time was exceeded.");

		case Minor::DP_Unknown_Channel_Handle:
			//: LABEL ALL_PLATFORMS
			return tr("The operation was aborted as an invalid channel handle was used.");

		case Minor::DP_Communication_Error:
			//: LABEL ALL_PLATFORMS
			return tr("A Communication error occurred during processing.");

		case Minor::DP_Trusted_Channel_Establishment_Failed:
			//: LABEL ALL_PLATFORMS
			return tr("A trusted channel could not be opened.");

		case Minor::DP_Unknown_Protocol:
			//: LABEL ALL_PLATFORMS
			return tr("The operation was aborted as an unknown protocol was used.");

		case Minor::DP_Unknown_Cipher_Suite:
			//: LABEL ALL_PLATFORMS
			return tr("The operation was aborted as an unknown cipher suite was used.");

		case Minor::DP_Unknown_Webservice_Binding:
			//: LABEL ALL_PLATFORMS
			return tr("The operation was aborted as an unknown web service binding was used.");

		case Minor::DP_Node_Not_Reachable:
			//: LABEL ALL_PLATFORMS
			return tr("A Communication error occurred during processing.");

		case Minor::IFDL_Timeout_Error:
			//: LABEL ALL_PLATFORMS
			return tr("The operation was terminated as the set time was exceeded.");

		case Minor::IFDL_Terminal_NoCard:
			//: LABEL ALL_PLATFORMS
			return tr("The card is missing or was removed.");

		case Minor::IFDL_IO_RepeatedDataMismatch:
			//: LABEL ALL_PLATFORMS
			return tr("The new PIN and the confirmation do not match.");

		case Minor::IFDL_IO_UnknownPINFormat:
			//: LABEL ALL_PLATFORMS
			return tr("The format of the PIN is wrong.");

		case Minor::KEY_KeyGenerationNotPossible:
			//: LABEL ALL_PLATFORMS
			return tr("Signature certificate key generation is not possible.");

		case Minor::SAL_Cancellation_by_User:
			//: LABEL ALL_PLATFORMS
			return tr("The process has been cancelled.");

		case Minor::IL_Signature_InvalidCertificatePath:
			//: LABEL ALL_PLATFORMS
			return tr("One or more certificate checks failed. The operation will be aborted due to security reasons.");

		case Minor::SAL_Invalid_Key:
			//: LABEL ALL_PLATFORMS
			return tr("This action cannot be performed. The eID function of your ID card is not activated.");

		case Minor::SAL_SecurityConditionNotSatisfied:
			//: LABEL ALL_PLATFORMS
			return tr("The authenticity of your ID card could not be verified. Please make sure that you are using a genuine ID card. Please note that test applications require the use of a test ID card.");

		case Minor::SAL_MEAC_AgeVerificationFailedWarning:
			//: LABEL ALL_PLATFORMS
			return tr("The age verification failed.");

		case Minor::SAL_MEAC_CommunityVerificationFailedWarning:
			//: LABEL ALL_PLATFORMS
			return tr("The community verification failed.");

		case Minor::SAL_MEAC_DocumentValidityVerificationFailed:
			//: LABEL ALL_PLATFORMS
			return tr("The ID card is invalid or disabled.");

		case Minor::IFDL_UnknownSlot:
		case Minor::IFDL_InvalidSlotHandle:
		case Minor::IFDL_CancellationByUser:
		case Minor::IFDL_IFD_SharingViolation:
		case Minor::null:
			return QString();
	}

	Q_UNREACHABLE();
}


ECardApiResult::ResultData::ResultData(Major pMajor, Minor pMinor, const QString& pMessage, Origin pOrigin, const std::optional<FailureCode>& pFailureCode)
	: QSharedData()
	, mMajor(pMajor)
	, mMinor(pMinor)
	, mMessage(pMessage)
	, mMessageLang(LanguageLoader::getLocaleCode())
	, mOrigin(pOrigin)
	, mFailureCode(pFailureCode)
{
}


ECardApiResult::ECardApiResult(Major pMajor, Minor pMinor, const QString& pMessage, Origin pOrigin, const std::optional<FailureCode>& pFailureCode)
	: d(new ResultData(pMajor, pMinor, pMessage, pOrigin, pFailureCode))
{
	const bool nullWithoutOk = pMajor != Major::Ok && pMinor == Minor::null;
	const bool okWithoutNull = pMajor == Major::Ok && pMinor != Minor::null;

	if (nullWithoutOk || okWithoutNull)
	{
		qCritical() << "Invalid combination of Major(" << pMajor << ") and Minor (" << pMinor << ") supplied.";
		d = new ResultData(Major::Error, Minor::AL_Internal_Error, pMessage, pOrigin);
	}
}


ECardApiResult::ECardApiResult(const QString& pMajor, const QString& pMinor, const QString& pMessage, Origin pOrigin)
	: d(new ResultData(parseMajor(pMajor), parseMinor(pMinor), pMessage, pOrigin))
{
	// No parameter sanitization here, since we receive values from remote hosts.
}


ECardApiResult::ECardApiResult(const GlobalStatus& pStatus, const std::optional<FailureCode>& pFailureCode)
	: ECardApiResult(fromStatus(pStatus, pFailureCode))
{
}


bool ECardApiResult::operator ==(const ECardApiResult& pResult) const
{
	return *d == *pResult.d;
}


ECardApiResult::Major ECardApiResult::getMajor() const
{
	return d->mMajor;
}


ECardApiResult::Minor ECardApiResult::getMinor() const
{
	return d->mMinor;
}


QString ECardApiResult::getMessage() const
{
	return d->mMessage;
}


const QString& ECardApiResult::getMessageLang() const
{
	return d->mMessageLang;
}


const std::optional<FailureCode>& ECardApiResult::getFailureCode() const
{
	return d->mFailureCode;
}


QString ECardApiResult::getMajorString(ECardApiResult::Major pMajor)
{
	return cMajorResults.value(pMajor);
}


QString ECardApiResult::getMinorString(ECardApiResult::Minor pMinor)
{
	return cMinorResults.value(pMinor);
}


QString ECardApiResult::getMajorString() const
{
	return getMajorString(d->mMajor);
}


QString ECardApiResult::getMinorString() const
{
	return getMinorString(d->mMinor);
}


bool ECardApiResult::isValid() const
{
	switch (d->mMajor)
	{
		case Major::Unknown:
			return false;

		case Major::Ok:
			return d->mMinor == Minor::null;

		default:
			return d->mMinor != Minor::null;
	}
}


bool ECardApiResult::isOk() const
{
	return d->mMajor == Major::Ok && d->mMinor == Minor::null;
}


bool ECardApiResult::isOriginServer() const
{
	return d->mOrigin == Origin::Server;
}


GlobalStatus ECardApiResult::toStatus() const
{
	GlobalStatus::ExternalInfoMap infoMap;
	const auto& message = getMessage();
	if (message.isEmpty() || isOriginServer())
	{
		// We want to map the server error to our own error message which will use the correct UI language
		infoMap.insert(GlobalStatus::ExternalInformation::ECARDAPI_ERROR, getMessage(getMinor()));

		if (message != infoMap.value(GlobalStatus::ExternalInformation::ECARDAPI_ERROR))
		{
			infoMap.insert(GlobalStatus::ExternalInformation::ECARDAPI_SERVERMESSAGE, message);
		}
	}
	else
	{
		infoMap.insert(GlobalStatus::ExternalInformation::ECARDAPI_ERROR, message);
	}

	switch (getMajor())
	{
		case Major::Unknown:
			return {GlobalStatus::Code::Unknown_Error, infoMap};

		case Major::Ok:
			if (isOk())
			{
				return GlobalStatus::Code::No_Error;
			}
			Q_FALLTHROUGH();

		case Major::Error:
			return {toStatus(getMinor()), infoMap, toStatus(d->mOrigin)};

		case Major::Warning:
			return {GlobalStatus::Code::Paos_Unexpected_Warning, infoMap};
	}

	Q_UNREACHABLE();
}


governikus::ECardApiResult::operator GlobalStatus() const
{
	return toStatus();
}


QJsonObject ECardApiResult::toJson() const
{
	QJsonObject obj;

	obj[QLatin1String("major")] = getMajorString();
	if (getMinor() != Minor::null)
	{
		obj[QLatin1String("minor")] = getMinorString();
	}

	const auto& message = getMessage();
	if (!message.isEmpty())
	{
		obj[QLatin1String("message")] = message;
	}

	const auto& minorDesc = ECardApiResult::getMessage(getMinor());
	if (!minorDesc.isEmpty())
	{
		obj[QLatin1String("description")] = minorDesc;
	}

	const auto& lang = getMessageLang();
	if (!lang.isEmpty() && (!message.isEmpty() || !minorDesc.isEmpty()))
	{
		obj[QLatin1String("language")] = lang;
	}

	const auto& failureCode = getFailureCode();
	if (failureCode.has_value())
	{
		obj[QLatin1String("reason")] = Enum<FailureCode::Reason>::getName(failureCode.value().getReason());
	}

	return obj;
}


QDebug operator <<(QDebug pDbg, const governikus::ECardApiResult& pResult)
{
	QString string = pResult.getMajorString() % QLatin1String(" | ") % pResult.getMinorString() % QLatin1String(" | ") % pResult.getMessage();
	const auto& failureCode = pResult.getFailureCode();
	if (failureCode.has_value())
	{
		string += QLatin1String(" | ");
		string += Enum<FailureCode::Reason>::getName(failureCode.value().getReason());
	}
	QDebugStateSaver saver(pDbg);
	pDbg.space() << "Result:";
	pDbg.quote() << string;
	return pDbg;
}
