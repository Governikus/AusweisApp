/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "Result.h"

#include "LanguageLoader.h"

#include <QDebug>
#include <QStringBuilder>

using namespace governikus;

#define RESULTMAJOR "http://www.bsi.bund.de/ecard/api/1.1/resultmajor"
#define RESULTMINOR "http://www.bsi.bund.de/ecard/api/1.1/resultminor"

const QMap<Result::Major, QString> Result::mMajorResults = {
	{Result::Major::Ok, QString::fromLatin1(RESULTMAJOR "#ok")},
	{Result::Major::Warning, QString::fromLatin1(RESULTMAJOR "#warning")},
	{Result::Major::Error, QString::fromLatin1(RESULTMAJOR "#error")}
};

// See TR-03112, section 4.2 for details about the codes
//      AL -> Applicaction Layer
//      DP -> Dispatcher
//      IFDL -> Interface Device Layer
//      KEY
//      SAL -> Service Access Layer
const QMap<GlobalStatus::Code, QString> Result::mMinorResults = {
	{GlobalStatus::Code::Paos_Error_AL_Unknown_Error, QString::fromLatin1(RESULTMINOR "/al/common#unknownError")},
	{GlobalStatus::Code::Paos_Error_AL_No_Permission, QString::fromLatin1(RESULTMINOR "/al/common#noPermission")},
	{GlobalStatus::Code::Paos_Error_AL_Internal_Error, QString::fromLatin1(RESULTMINOR "/al/common#internalError")},
	{GlobalStatus::Code::Paos_Error_AL_Parameter_Error, QString::fromLatin1(RESULTMINOR "/al/common#parameterError")},
	{GlobalStatus::Code::Paos_Error_AL_Unkown_API_Function, QString::fromLatin1(RESULTMINOR "/al/common#unkownAPIFunction")},
	{GlobalStatus::Code::Paos_Error_AL_Not_Initialized, QString::fromLatin1(RESULTMINOR "/al/common#notInitialized")},
	{GlobalStatus::Code::Paos_Error_AL_Warning_Connection_Disconnected, QString::fromLatin1(RESULTMINOR "/al/common#warningConnectionDisconnected")},
	{GlobalStatus::Code::Paos_Error_AL_Session_Terminated_Warning, QString::fromLatin1(RESULTMINOR "/al/common#SessionTerminatedWarning")},
	{GlobalStatus::Code::Paos_Error_AL_Communication_Error, QString::fromLatin1(RESULTMINOR "/al/common#communicationError")},
	{GlobalStatus::Code::Paos_Error_DP_Timeout_Error, QString::fromLatin1(RESULTMINOR "/dp#timeout")},
	{GlobalStatus::Code::Paos_Error_DP_Unknown_Channel_Handle, QString::fromLatin1(RESULTMINOR "/dp#unknownChannelHandle")},
	{GlobalStatus::Code::Paos_Error_DP_Communication_Error, QString::fromLatin1(RESULTMINOR "/dp#communicationError")},
	{GlobalStatus::Code::Paos_Error_DP_Trusted_Channel_Establishment_Failed, QString::fromLatin1(RESULTMINOR "/dp#trustedChannelEstablishmentFailed")},
	{GlobalStatus::Code::Paos_Error_DP_Unknown_Protocol, QString::fromLatin1(RESULTMINOR "/dp#unknownProtocol")},
	{GlobalStatus::Code::Paos_Error_DP_Unknown_Cipher_Suite, QString::fromLatin1(RESULTMINOR "/dp#unknownCipherSuite")},
	{GlobalStatus::Code::Paos_Error_DP_Unknown_Webservice_Binding, QString::fromLatin1(RESULTMINOR "/dp#unknownWebserviceBinding")},
	{GlobalStatus::Code::Paos_Error_DP_Node_Not_Reachable, QString::fromLatin1(RESULTMINOR "/dp#nodeNotReachable")},
	{GlobalStatus::Code::Paos_Error_IFDL_Timeout_Error, QString::fromLatin1(RESULTMINOR "/ifdl/common#timeoutError")},
	{GlobalStatus::Code::Paos_Error_IFDL_InvalidSlotHandle, QString::fromLatin1(RESULTMINOR "/ifdl/common#invalidSlotHandle")},
	{GlobalStatus::Code::Paos_Error_IFDL_CancellationByUser, QString::fromLatin1(RESULTMINOR "/ifdl#cancellationByUser")},
	{GlobalStatus::Code::Paos_Error_KEY_KeyGenerationNotPossible, QString::fromLatin1(RESULTMINOR "/il/key#keyGenerationNotPossible")},
	{GlobalStatus::Code::Paos_Error_SAL_Cancellation_by_User, QString::fromLatin1(RESULTMINOR "/sal#cancellationByUser")},
	{GlobalStatus::Code::Paos_Error_SAL_CertificateChainInterrupted, QString::fromLatin1(RESULTMINOR "/sal#certificateChainInterrupted")},
	{GlobalStatus::Code::Paos_Error_SAL_Invalid_Key, QString::fromLatin1(RESULTMINOR "/sal#invalidKey")},
	{GlobalStatus::Code::Paos_Error_SAL_SecurityConditionNotSatisfied, QString::fromLatin1(RESULTMINOR "/sal#securityConditionNotSatisfied")},
	{GlobalStatus::Code::Paos_Error_SAL_MEAC_AgeVerificationFailedWarning, QString::fromLatin1(RESULTMINOR "/sal/mEAC#AgeVerificationFailedWarning")},
	{GlobalStatus::Code::Paos_Error_SAL_MEAC_CommunityVerificationFailedWarning, QString::fromLatin1(RESULTMINOR "/sal/mEAC#CommunityVerificationFailedWarning")},
	{GlobalStatus::Code::Paos_Error_SAL_MEAC_DocumentValidityVerificationFailed, QString::fromLatin1(RESULTMINOR "/sal/mEAC#DocumentValidityVerificationFailed")}
};


Result Result::fromStatus(const GlobalStatus& pStatus)
{
	switch (pStatus)
	{
		case GlobalStatus::Code::Unknown_Error:
		case GlobalStatus::Code::Paos_Unexpected_Warning:
		case GlobalStatus::Code::Paos_Error_AL_Unknown_Error:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_AL_Unknown_Error, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::No_Error:
			return createOk();

		case GlobalStatus::Code::Workflow_Card_Removed:
		case GlobalStatus::Code::Paos_Error_IFDL_CancellationByUser:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_IFDL_CancellationByUser, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Workflow_Preverification_Developermode_Error:
		case GlobalStatus::Code::Workflow_Preverification_Error:
		case GlobalStatus::Code::Workflow_No_Unique_AtCvc:
		case GlobalStatus::Code::Workflow_No_Unique_DvCvc:
		case GlobalStatus::Code::Paos_Error_SAL_CertificateChainInterrupted:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_SAL_CertificateChainInterrupted, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Workflow_Certificate_No_Description:
		case GlobalStatus::Code::Workflow_Certificate_No_Url_In_Description:
		case GlobalStatus::Code::Workflow_Certificate_Hash_Error:
		case GlobalStatus::Code::Workflow_Certificate_Sop_Error:
		case GlobalStatus::Code::Paos_Error_AL_Parameter_Error:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_AL_Parameter_Error, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Workflow_TrustedChannel_Establishment_Error:
		case GlobalStatus::Code::Workflow_TrustedChannel_Error_From_Server:
		case GlobalStatus::Code::Workflow_TrustedChannel_Hash_Not_In_Description:
		case GlobalStatus::Code::Workflow_TrustedChannel_No_Data_Received:
		case GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Certificate_Unsupported_Algorithm_Or_Length:
		case GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable:
		case GlobalStatus::Code::Workflow_TrustedChannel_TimeOut:
		case GlobalStatus::Code::Workflow_TrustedChannel_Proxy_Error:
		case GlobalStatus::Code::Workflow_TrustedChannel_Ssl_Establishment_Error:
		case GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error:
		case GlobalStatus::Code::Workflow_TrustedChannel_Other_Network_Error:
		case GlobalStatus::Code::Paos_Error_DP_Trusted_Channel_Establishment_Failed:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_DP_Trusted_Channel_Establishment_Failed, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Workflow_Cancellation_By_User:
		case GlobalStatus::Code::Card_Cancellation_By_User:
		case GlobalStatus::Code::Paos_Error_SAL_Cancellation_by_User:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_SAL_Cancellation_by_User, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Workflow_No_Permission_Error:
		case GlobalStatus::Code::Paos_Error_AL_No_Permission:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_AL_No_Permission, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Workflow_Communication_Missing_Redirect_Url:
		case GlobalStatus::Code::Workflow_Error_Page_Transmission_Error:
		case GlobalStatus::Code::Workflow_Redirect_Transmission_Error:
		case GlobalStatus::Code::Workflow_Processing_Error:
		case GlobalStatus::Code::Workflow_Reader_Became_Inaccessible:
		case GlobalStatus::Code::Workflow_Reader_Communication_Error:
		case GlobalStatus::Code::Workflow_Reader_Device_Connection_Error:
		case GlobalStatus::Code::Workflow_Reader_Device_Scan_Error:
		case GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided:
		case GlobalStatus::Code::Network_Ssl_Establishment_Error:
		case GlobalStatus::Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length:
		case GlobalStatus::Code::Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length:
		case GlobalStatus::Code::Workflow_Nerwork_Ssl_Hash_Not_In_Certificate_Description:
		case GlobalStatus::Code::Workflow_Network_Empty_Redirect_Url:
		case GlobalStatus::Code::Workflow_Network_Expected_Redirect:
		case GlobalStatus::Code::Workflow_Network_Invalid_Scheme:
		case GlobalStatus::Code::Workflow_Network_Malformed_Redirect_Url:
		case GlobalStatus::Code::Network_ServiceUnavailable:
		case GlobalStatus::Code::Network_TimeOut:
		case GlobalStatus::Code::Network_Proxy_Error:
		case GlobalStatus::Code::Network_Other_Error:
		case GlobalStatus::Code::Workflow_Wrong_Parameter_Invocation:
		case GlobalStatus::Code::Card_Invalid_Pin:
		case GlobalStatus::Code::Card_Invalid_Can:
		case GlobalStatus::Code::Card_Invalid_Puk:
		case GlobalStatus::Code::Card_Puk_Blocked:
		case GlobalStatus::Code::Paos_Error_AL_Communication_Error:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_AL_Communication_Error, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Workflow_AlreadyInProgress_Error:
		case GlobalStatus::Code::Card_Not_Found:
		case GlobalStatus::Code::Card_Communication_Error:
		case GlobalStatus::Code::Card_Input_TimeOut:
		case GlobalStatus::Code::Card_Pin_Blocked:
		case GlobalStatus::Code::Card_Pin_Not_Blocked:
		case GlobalStatus::Code::Card_NewPin_Mismatch:
		case GlobalStatus::Code::Card_NewPin_Invalid_Length:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_AL_Unknown_Error, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Workflow_Cannot_Confirm_IdCard_Authenticity:
		case GlobalStatus::Code::Workflow_Unknown_Paos_Form_EidServer:
		case GlobalStatus::Code::Workflow_Unexpected_Message_From_EidServer:
		case GlobalStatus::Code::Workflow_Pin_Blocked_And_Puk_Objectionable:
		case GlobalStatus::Code::Card_Protocol_Error:
		case GlobalStatus::Code::Paos_Error_AL_Internal_Error:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_AL_Internal_Error, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_AL_Unkown_API_Function:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_AL_Unkown_API_Function, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_AL_Not_Initialized:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_AL_Not_Initialized, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_AL_Warning_Connection_Disconnected:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_AL_Warning_Connection_Disconnected, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_AL_Session_Terminated_Warning:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_AL_Session_Terminated_Warning, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_DP_Timeout_Error:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_DP_Timeout_Error, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_DP_Unknown_Channel_Handle:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_DP_Unknown_Channel_Handle, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_DP_Communication_Error:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_DP_Communication_Error, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_DP_Unknown_Protocol:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_DP_Unknown_Protocol, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_DP_Unknown_Cipher_Suite:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_DP_Unknown_Cipher_Suite, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_DP_Unknown_Webservice_Binding:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_DP_Unknown_Webservice_Binding, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_DP_Node_Not_Reachable:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_DP_Node_Not_Reachable, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_IFDL_Timeout_Error:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_IFDL_Timeout_Error, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_IFDL_InvalidSlotHandle:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_IFDL_InvalidSlotHandle, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_KEY_KeyGenerationNotPossible:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_KEY_KeyGenerationNotPossible, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_SAL_Invalid_Key:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_SAL_Invalid_Key, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_SAL_SecurityConditionNotSatisfied:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_SAL_SecurityConditionNotSatisfied, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_SAL_MEAC_AgeVerificationFailedWarning:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_SAL_MEAC_AgeVerificationFailedWarning, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_SAL_MEAC_CommunityVerificationFailedWarning:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_SAL_MEAC_CommunityVerificationFailedWarning, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::Paos_Error_SAL_MEAC_DocumentValidityVerificationFailed:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_SAL_MEAC_DocumentValidityVerificationFailed, pStatus.toErrorDescription(), pStatus.getOrigin());

		case GlobalStatus::Code::RemoteReader_CloseCode_NormalClose:
			return createOk();

		case GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose:
		case GlobalStatus::Code::RemoteReader_CloseCode_Undefined:
		case GlobalStatus::Code::RemoteConnector_InvalidRequest:
		case GlobalStatus::Code::RemoteConnector_EmptyPassword:
		case GlobalStatus::Code::RemoteConnector_NoSupportedApiLevel:
		case GlobalStatus::Code::RemoteConnector_ConnectionTimeout:
		case GlobalStatus::Code::RemoteConnector_ConnectionError:
		case GlobalStatus::Code::RemoteConnector_RemoteHostRefusedConnection:
		case GlobalStatus::Code::Downloader_File_Not_Found:
		case GlobalStatus::Code::Downloader_Cannot_Save_File:
		case GlobalStatus::Code::Downloader_Data_Corrupted:
			return Result(Result::Major::Error, GlobalStatus::Code::Paos_Error_AL_Unknown_Error, pStatus.toErrorDescription(), pStatus.getOrigin());
	}

	Q_UNREACHABLE();
}


Result Result::createOk()
{
	return Result(Result::Major::Ok, GlobalStatus::Code::No_Error);
}


Result::Major Result::parseMajor(const QString& pMajor)
{
	for (auto iter = mMajorResults.cbegin(); iter != mMajorResults.cend(); ++iter)
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


GlobalStatus::Code Result::parseMinor(const QString& pMinor)
{
	for (auto iter = mMinorResults.cbegin(); iter != mMinorResults.cend(); ++iter)
	{
		if (pMinor == iter.value())
		{
			return iter.key();
		}
	}

	if (!pMinor.isEmpty())
	{
		qWarning() << "Unknown ResultMinor:" << pMinor;
		return GlobalStatus::Code::Unknown_Error;
	}

	return GlobalStatus::Code::No_Error;
}


bool Result::isMajor(const QString& major)
{
	return Major::Unknown != parseMajor(major);
}


bool Result::isMinor(const QString& minor)
{
	return GlobalStatus::Code::Unknown_Error != parseMinor(minor);
}


QString Result::getMessage(GlobalStatus::Code pMinor)
{
	switch (pMinor)
	{
		case GlobalStatus::Code::Paos_Error_AL_Unknown_Error:
			return tr("An unexpected error has occurred during processing.");

		case GlobalStatus::Code::Paos_Error_AL_No_Permission:
			return tr("Use of the function by the client application is not permitted.");

		case GlobalStatus::Code::Paos_Error_AL_Internal_Error:
			return tr("An internal error has occurred during processing.");

		case GlobalStatus::Code::Paos_Error_AL_Parameter_Error:
			return tr("There was some problem with a provided or omitted parameter.");

		case GlobalStatus::Code::Paos_Error_AL_Unkown_API_Function:
			return tr("The API function is unknown.");

		case GlobalStatus::Code::Paos_Error_AL_Not_Initialized:
			return tr("The framework or layer has not been initialized.");

		case GlobalStatus::Code::Paos_Error_AL_Warning_Connection_Disconnected:
			return tr("The active session has been terminated.");

		case GlobalStatus::Code::Paos_Error_AL_Session_Terminated_Warning:
			return tr("The active session has been terminated.");

		case GlobalStatus::Code::Paos_Error_AL_Communication_Error:
			return tr("A Communication error occurred during processing.");

		case GlobalStatus::Code::Paos_Error_DP_Timeout_Error:
			return tr("The operation was terminated as the set time was exceeded.");

		case GlobalStatus::Code::Paos_Error_DP_Unknown_Channel_Handle:
			return tr("The operation was aborted as an invalid channel handle was used.");

		case GlobalStatus::Code::Paos_Error_DP_Communication_Error:
			return tr("A Communication error occurred during processing.");

		case GlobalStatus::Code::Paos_Error_DP_Trusted_Channel_Establishment_Failed:
			return tr("A trusted channel could not be opened.");

		case GlobalStatus::Code::Paos_Error_DP_Unknown_Protocol:
			return tr("The operation was aborted as an unknown protocol was used.");

		case GlobalStatus::Code::Paos_Error_DP_Unknown_Cipher_Suite:
			return tr("The operation was aborted as an unknown cipher suite was used.");

		case GlobalStatus::Code::Paos_Error_DP_Unknown_Webservice_Binding:
			return tr("The operation was aborted as an unknown web service binding was used.");

		case GlobalStatus::Code::Paos_Error_DP_Node_Not_Reachable:
			return tr("A Communication error occurred during processing.");

		case GlobalStatus::Code::Paos_Error_IFDL_Timeout_Error:
			return tr("The operation was terminated as the set time was exceeded.");

		case GlobalStatus::Code::Paos_Error_KEY_KeyGenerationNotPossible:
			return tr("Signature certificate key generation is not possible.");

		case GlobalStatus::Code::Paos_Error_SAL_Cancellation_by_User:
			return tr("The process was cancelled by the user.");

		case GlobalStatus::Code::Paos_Error_SAL_CertificateChainInterrupted:
			return tr("One or more certificate checks failed. The operation will be aborted due to security reasons.");

		case GlobalStatus::Code::Paos_Error_SAL_Invalid_Key:
			return tr("This action cannot be performed. The online identification function of your ID card is deactivated. Please contact the authority responsible for issuing your identification document to activate the online identification function.");

		case GlobalStatus::Code::Paos_Error_SAL_SecurityConditionNotSatisfied:
			return tr("The authenticity of your ID card could not be verified. Please make sure that you are using a genuine ID card. Please note that test applications require the use of a test ID card.");

		case GlobalStatus::Code::Paos_Error_SAL_MEAC_AgeVerificationFailedWarning:
			return tr("The age verification failed.");

		case GlobalStatus::Code::Paos_Error_SAL_MEAC_CommunityVerificationFailedWarning:
			return tr("The community verification failed.");

		case GlobalStatus::Code::Paos_Error_SAL_MEAC_DocumentValidityVerificationFailed:
			return tr("The ID card is invalid or disabled.");

		case GlobalStatus::Code::Unknown_Error:
		default:
			return QString();
	}
}


Result::ResultData::ResultData(Major pMajor, GlobalStatus::Code pMinor, const QString& pMessage, Origin pOrigin)
	: QSharedData()
	, mMajor(pMajor)
	, mMinor(pMinor)
	, mMessage(pMessage)
	, mMessageLang(LanguageLoader::getInstance().getUsedLocale().bcp47Name().left(2))
	, mOrigin(pOrigin)
{
	if (pMinor != GlobalStatus::Code::No_Error && pMinor != GlobalStatus::Code::Unknown_Error && !mMinorResults.contains(pMinor))
	{
		qCritical() << "Unknown status code in minor paos result:" << pMinor;
		Q_ASSERT(false);
	}
}


Result::Result(Major pMajor, GlobalStatus::Code pMinor, const QString& pMessage, Origin pOrigin)
	: d(new ResultData(pMajor, pMinor, pMessage, pOrigin))
{
}


Result::Result(const QString& pMajor, const QString& pMinor, const QString& pMessage, Origin pOrigin)
	: Result(parseMajor(pMajor), parseMinor(pMinor), pMessage, pOrigin)
{
}


Result::Result(const GlobalStatus& pStatus)
	: Result(fromStatus(pStatus))
{

}


bool Result::operator ==(const Result& pResult) const
{
	return *d == *pResult.d;
}


Result::Major Result::getMajor() const
{
	return d->mMajor;
}


GlobalStatus::Code Result::getMinor() const
{
	return d->mMinor;
}


QString Result::getMessage() const
{
	return d->mMessage;
}


const QString& Result::getMessageLang() const
{
	return d->mMessageLang;
}


QString Result::getMajorString() const
{
	return mMajorResults.value(d->mMajor);
}


QString Result::getMinorString() const
{
	return mMinorResults.value(d->mMinor);
}


bool Result::isValid() const
{
	switch (d->mMajor)
	{
		case Major::Unknown:
			return false;

		case Major::Ok:
			return d->mMinor == GlobalStatus::Code::No_Error;

		default:
			return d->mMinor != GlobalStatus::Code::Unknown_Error;
	}
}


bool Result::isOk() const
{
	return d->mMajor == Major::Ok && d->mMinor == GlobalStatus::Code::No_Error;
}


bool Result::isOriginServer() const
{
	return d->mOrigin == Origin::Server;
}


GlobalStatus Result::toStatus() const
{
	QString message = getMessage();
	if (message.isEmpty() || isOriginServer())
	{
		// if the message is not set, we clearly use the result minor
		// if the server sent the message, it can be in any language, so we take the result minor
		message = Result::getMessage(getMinor());
	}

	switch (getMajor())
	{
		case Major::Unknown:
			return GlobalStatus(GlobalStatus::Code::Unknown_Error, message);

		case Major::Ok:
			if (isOk())
			{
				return GlobalStatus::Code::No_Error;
			}
		// FALLTHROUGH

		case Major::Error:
			return GlobalStatus(getMinor(), message, d->mOrigin);

		case Major::Warning:
			return GlobalStatus(GlobalStatus::Code::Paos_Unexpected_Warning, message);
	}

	Q_UNREACHABLE();
}


governikus::Result::operator GlobalStatus() const
{
	return toStatus();
}


QJsonObject Result::toJson() const
{
	QJsonObject obj;

	obj[QLatin1String("major")] = getMajorString();
	if (getMinor() != GlobalStatus::Code::No_Error)
	{
		obj[QLatin1String("minor")] = getMinorString();
	}

	const auto& message = getMessage();
	if (!message.isEmpty())
	{
		obj[QLatin1String("message")] = message;
	}

	const auto& minorDesc = Result::getMessage(getMinor());
	if (!minorDesc.isEmpty())
	{
		obj[QLatin1String("description")] = minorDesc;
	}

	const auto& lang = getMessageLang();
	if (!lang.isEmpty() && (!message.isEmpty() || !minorDesc.isEmpty()))
	{
		obj[QLatin1String("language")] = lang;
	}

	return obj;
}


QDebug operator <<(QDebug pDbg, const governikus::Result& pResult)
{
	const QString string = pResult.getMajorString() % QLatin1String(" | ") % pResult.getMinorString() % QLatin1String(" | ") % pResult.getMessage();
	QDebugStateSaver saver(pDbg);
	pDbg.space() << "Result:";
	pDbg.quote() << string;
	return pDbg;
}
