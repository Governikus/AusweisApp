/*!
 * Result.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "Result.h"

#include "LanguageLoader.h"
#include "ReturnCodeUtil.h"

#include <QDebug>
#include <QStringBuilder>

using namespace governikus;

#define RESULTMAJOR "http://www.bsi.bund.de/ecard/api/1.1/resultmajor"
#define RESULTMINOR "http://www.bsi.bund.de/ecard/api/1.1/resultminor"

const QMap<Result::Major, QString> Result::mMajorResults = {
	{Result::Major::Ok, RESULTMAJOR "#ok"},
	{Result::Major::Warning, RESULTMAJOR "#warning"},
	{Result::Major::Error, RESULTMAJOR "#error"}
};

const QMap<Result::Minor, QString> Result::mMinorResults = {
	{Result::Minor::AL_Unknown_Error, RESULTMINOR "/al/common#unknownError"},
	{Result::Minor::AL_No_Permission, RESULTMINOR "/al/common#noPermission"},
	{Result::Minor::AL_Internal_Error, RESULTMINOR "/al/common#internalError"},
	{Result::Minor::AL_Parameter_Error, RESULTMINOR "/al/common#parameterError"},
	{Result::Minor::AL_Unkown_API_Function, RESULTMINOR "/al/common#unkownAPIFunction"},
	{Result::Minor::AL_Not_Initialized, RESULTMINOR "/al/common#notInitialized"},
	{Result::Minor::AL_Warning_Connection_Disconnected, RESULTMINOR "/al/common#warningConnectionDisconnected"},
	{Result::Minor::AL_Session_Terminated_Warning, RESULTMINOR "/al/common#SessionTerminatedWarning"},
	{Result::Minor::AL_Communication_Error, RESULTMINOR "/al/common#communicationError"},
	{Result::Minor::DP_Timeout_Error, RESULTMINOR "/dp#timeout"},
	{Result::Minor::DP_Unknown_Channel_Handle, RESULTMINOR "/dp#unknownChannelHandle"},
	{Result::Minor::DP_Communication_Error, RESULTMINOR "/dp#communicationError"},
	{Result::Minor::DP_Trusted_Channel_Establishment_Failed, RESULTMINOR "/dp#trustedChannelEstablishmentFailed"},
	{Result::Minor::DP_Unknown_Protocol, RESULTMINOR "/dp#unknownProtocol"},
	{Result::Minor::DP_Unknown_Cipher_Suite, RESULTMINOR "/dp#unknownCipherSuite"},
	{Result::Minor::DP_Unknown_Webservice_Binding, RESULTMINOR "/dp#unknownWebserviceBinding"},
	{Result::Minor::DP_Node_Not_Reachable, RESULTMINOR "/dp#nodeNotReachable"},
	{Result::Minor::SAL_Cancellation_by_User, RESULTMINOR "/sal#cancellationByUser"},
	{Result::Minor::IFDL_Timeout_Error, RESULTMINOR "/ifdl/common#timeoutError"},
	{Result::Minor::IFDL_InvalidSlotHandle, RESULTMINOR "/ifdl/common#invalidSlotHandle"},
	{Result::Minor::IFDL_CancellationByUser, RESULTMINOR "/ifdl#cancellationByUser"},
	{Result::Minor::KEY_KeyGenerationNotPossible, RESULTMINOR "/il/key#keyGenerationNotPossible"},
	{Result::Minor::SAL_CertificateChainInterrupted, RESULTMINOR "/sal#certificateChainInterrupted"},
	{Result::Minor::SAL_Invalid_Key, RESULTMINOR "/sal#invalidKey"},
	{Result::Minor::SAL_SecurityConditionNotSatisfied, RESULTMINOR "/sal#securityConditionNotSatisfied"},
	{Result::Minor::SAL_MEAC_AgeVerificationFailedWarning, RESULTMINOR "/sal/mEAC#AgeVerificationFailedWarning"},
	{Result::Minor::SAL_MEAC_CommunityVerificationFailedWarning, RESULTMINOR "/sal/mEAC#CommunityVerificationFailedWarning"},
	{Result::Minor::SAL_MEAC_DocumentValidityVerificationFailed, RESULTMINOR "/sal/mEAC#DocumentValidityVerificationFailed"},
};

const QMap<Result::Minor, const char*> Result::mMinorDescriptions = {
	{Result::Minor::AL_Unknown_Error, QT_TR_NOOP("An unexpected error has occurred during processing.")},
	{Result::Minor::AL_No_Permission, QT_TR_NOOP("Use of the function by the client application is not permitted.")},
	{Result::Minor::AL_Internal_Error, QT_TR_NOOP("An internal error has occurred during processing.")},
	{Result::Minor::AL_Parameter_Error, QT_TR_NOOP("There was some problem with a provided or omitted parameter.")},
	{Result::Minor::AL_Unkown_API_Function, QT_TR_NOOP("The API function is unknown.")},
	{Result::Minor::AL_Not_Initialized, QT_TR_NOOP("The framework or layer has not been initialized.")},
	{Result::Minor::AL_Warning_Connection_Disconnected, QT_TR_NOOP("The active session has been terminated.")},
	{Result::Minor::AL_Session_Terminated_Warning, QT_TR_NOOP("The active session has been terminated.")},
	{Result::Minor::AL_Communication_Error, QT_TR_NOOP("A Communication error occurred during processing.")},
	{Result::Minor::DP_Timeout_Error, QT_TR_NOOP("The operation was terminated as the set time was exceeded.")},
	{Result::Minor::DP_Unknown_Channel_Handle, QT_TR_NOOP("The operation was aborted as an invalid channel handle was used.")},
	{Result::Minor::DP_Communication_Error, QT_TR_NOOP("A Communication error occurred during processing.")},
	{Result::Minor::DP_Trusted_Channel_Establishment_Failed, QT_TR_NOOP("A trusted channel could not be opened.")},
	{Result::Minor::DP_Unknown_Protocol, QT_TR_NOOP("The operation was aborted as an unknown protocol was used.")},
	{Result::Minor::DP_Unknown_Cipher_Suite, QT_TR_NOOP("The operation was aborted as an unknown cipher suite was used.")},
	{Result::Minor::DP_Unknown_Webservice_Binding, QT_TR_NOOP("The operation was aborted as an unknown web service binding was used.")},
	{Result::Minor::DP_Node_Not_Reachable, QT_TR_NOOP("A Communication error occurred during processing.")},
	{Result::Minor::SAL_Cancellation_by_User, QT_TR_NOOP("The operation was aborted due to cancellation by user.")},
	{Result::Minor::IFDL_Timeout_Error, QT_TR_NOOP("The operation was terminated as the set time was exceeded.")},
	{Result::Minor::KEY_KeyGenerationNotPossible, QT_TR_NOOP("Signature certificate key generation is not possible.")},
	{Result::Minor::SAL_CertificateChainInterrupted, QT_TR_NOOP("One or more certificate checks failed. The operation will be aborted due to security reasons.")},
	{Result::Minor::SAL_Invalid_Key, QT_TR_NOOP("This action cannot be performed. The online identification function of your ID card is deactivated. Please contact your citizen centre to activate the online identification function.")},
	{Result::Minor::SAL_SecurityConditionNotSatisfied, QT_TR_NOOP("The authenticity of your ID card could not be verified. Please make sure that you are using a genuine ID card. Please note that test applications require the use of a test ID card.")},
	{Result::Minor::SAL_MEAC_AgeVerificationFailedWarning, QT_TR_NOOP("The age verification failed.")},
	{Result::Minor::SAL_MEAC_CommunityVerificationFailedWarning, QT_TR_NOOP("The community verification failed.")},
	{Result::Minor::SAL_MEAC_DocumentValidityVerificationFailed, QT_TR_NOOP("The ID card is invalid or disabled.")},
};


Result Result::createOk()
{
	return Result(Result::Major::Ok, Result::Minor::null);
}


Result Result::createError(ReturnCode pReturnCode)
{
	QPair<Result::Minor, QString> errorPair = ReturnCodeUtil::toError(pReturnCode);
	return Result(Result::Major::Error, errorPair.first, errorPair.second, QString(), pReturnCode);
}


Result Result::createInternalError(const QString& pMessage, const QString& pLang)
{
	return Result(Result::Major::Error, Result::Minor::AL_Internal_Error, pMessage, pLang);
}


Result Result::createParameterError(const QString& pMessage, const QString& pLang)
{
	return Result(Result::Major::Error, Result::Minor::AL_Parameter_Error, pMessage, pLang);
}


Result Result::createCommunicationError(const QString& pMessage, const QString& pLang)
{
	return Result(Result::Major::Error, Result::Minor::AL_Communication_Error, pMessage, pLang);
}


Result Result::createCertChainInterruptedError(const QString& pMessage, const QString& pLang)
{
	return Result(Result::Major::Error, Result::Minor::SAL_CertificateChainInterrupted, pMessage, pLang);
}


Result Result::createNoPermissionError(const QString& pMessage, const QString& pLang)
{
	return Result(Result::Major::Error, Result::Minor::AL_No_Permission, pMessage, pLang);
}


Result Result::createInvalidKeyError(const QString& pMessage, const QString& pLang)
{
	return Result(Result::Major::Error, Result::Minor::SAL_Invalid_Key, pMessage, pLang);
}


Result Result::createCardRemovedError(const QString& pMessage, const QString& pLang)
{
	return Result(Result::Major::Error, Result::Minor::IFDL_CancellationByUser, pMessage, pLang);
}


Result Result::createTrustedChannelEstablishmentError(const QString& pMessage, const QString& pLang)
{
	return Result(Result::Major::Error, Result::Minor::DP_Trusted_Channel_Establishment_Failed, pMessage, pLang);
}


Result Result::createCancelByUserError()
{
	return Result(Result::Major::Error, Result::Minor::SAL_Cancellation_by_User, ReturnCodeUtil::toMessage(ReturnCode::CANCELLATION_BY_USER));
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
	return Major::null;
}


Result::Minor Result::parseMinor(const QString& pMinor)
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
	}
	return Minor::null;
}


bool Result::isMajor(const QString& major)
{
	return Major::null != parseMajor(major);
}


bool Result::isMinor(const QString& minor)
{
	return Minor::null != parseMinor(minor);
}


Result::ResultData::ResultData(Major pMajor, Minor pMinor, const QString& pMessage, const QString& pLang, ReturnCode pReturnCode, Origin pOrigin)
	: QSharedData()
	, mMajor(pMajor)
	, mMinor(pMinor)
	, mMessage(pMessage)
	, mMessageLang(pLang.isEmpty() ? LanguageLoader::getInstance().getUsedLocale().bcp47Name().left(2) : pLang)
	, mReturnCode(pReturnCode)
	, mOrigin(pOrigin)
{
}


Result::Result(Major pMajor, Minor pMinor, const QString& pMessage, const QString& pLang, ReturnCode pReturnCode, Origin pOrigin)
	: d(new ResultData(pMajor, pMinor, pMessage, pLang, pReturnCode, pOrigin))
{
}


Result::Result(const QString& pMajor, const QString& pMinor, const QString& pMessage, const QString& pLang, ReturnCode pReturnCode, Origin pOrigin)
	: Result(parseMajor(pMajor), parseMinor(pMinor), pMessage, pLang, pReturnCode, pOrigin)
{
}


Result::Major Result::getMajor() const
{
	return d->mMajor;
}


Result::Minor Result::getMinor() const
{
	return d->mMinor;
}


const QString& Result::getMessage() const
{
	return d->mMessage;
}


const QString& Result::getMessageLang() const
{
	return d->mMessageLang;
}


ReturnCode Result::getReturnCode() const
{
	return d->mReturnCode;
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
		case Major::null:
			return false;

		case Major::Ok:
			return d->mMinor == Minor::null;

		default:
			return d->mMinor != Minor::null;
	}
}


bool Result::isOk() const
{
	return d->mMajor == Major::Ok && d->mMinor == Minor::null;
}


QString Result::getMinorDescription() const
{
	return tr(mMinorDescriptions.value(d->mMinor));
}


bool Result::isOriginServer() const
{
	return d->mOrigin == Origin::SERVER;
}


QJsonObject Result::toJson() const
{
	QJsonObject obj;

	obj["major"] = getMajorString();
	if (getMinor() != Minor::null)
	{
		obj["minor"] = getMinorString();
	}

	const auto& message = getMessage();
	if (!message.isEmpty())
	{
		obj["message"] = message;
	}

	const auto& minorDesc = getMinorDescription();
	if (!minorDesc.isEmpty())
	{
		obj["description"] = minorDesc;
	}

	const auto& lang = getMessageLang();
	if (!lang.isEmpty() && (!message.isEmpty() || !minorDesc.isEmpty()))
	{
		obj["language"] = lang;
	}

	return obj;
}


QString Result::toString() const
{
	return getMajorString() % " | " % getMinorString() % " | " % getMessage();
}


QDebug operator <<(QDebug pDbg, const governikus::Result& pResult)
{
	QDebugStateSaver saver(pDbg);
	pDbg.space() << "Result:";
	pDbg.quote() << pResult.toString();
	return pDbg;
}
