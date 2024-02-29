/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "SmartManager.h"

#include "AppSettings.h"
#include "ReaderManager.h"
#include "VolatileSettings.h"
#include "command/TransmitCommand.h"
#ifdef Q_OS_ANDROID
	#include "SecureStorage.h"
#endif

#include <QLoggingCategory>
#include <QOperatingSystemVersion>

#ifdef Q_OS_ANDROID
	#include <QJniEnvironment>
	#include <QJniObject>
#endif
#include <eid_applet_interface.h>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


QWeakPointer<SmartManager> SmartManager::cWeakInstance;
QSharedPointer<SmartManager> SmartManager::cStrongInstance;


[[nodiscard]] CardReturnCode fromEidServiceResult(EidServiceResult pResult)
{
	return pResult == EidServiceResult::SUCCESS ? CardReturnCode::OK : CardReturnCode::COMMAND_FAILED;
}


[[nodiscard]] QByteArray fromHex(const std::string& pString)
{
	return QByteArray::fromHex(QByteArray::fromStdString(pString));
}


QSharedPointer<SmartManager> SmartManager::get(bool pKeepConnection)
{
	QSharedPointer<SmartManager> ptr(cWeakInstance);
	if (!ptr)
	{
		ptr.reset(new SmartManager());
		cWeakInstance = QWeakPointer(ptr);
	}

	if (pKeepConnection)
	{
		cStrongInstance = ptr;
	}

	return ptr;
}


void SmartManager::releaseConnection()
{
	cStrongInstance.reset();
}


SmartManager::SmartManager()
	: QObject(nullptr)
	, mInitialized(false)
{
	if (!Env::getSingleton<ReaderManager>()->isWorkerThread())
	{
		qCCritical(card_smart) << "Only the ReaderManager thread is allowed to create a SmartManager!";
		return;
	}

#ifdef Q_OS_ANDROID
	if (QOperatingSystemVersion::current() < QOperatingSystemVersion(QOperatingSystemVersion::Android, 9))
	{
		qCDebug(card_smart) << "EidAppletService is not available on:" << QOperatingSystemVersion::current();
		return;
	}

	QJniEnvironment env;
	QJniObject context = QNativeInterface::QAndroidApplication::context();

	const auto& storage = Env::getSingleton<SecureStorage>();
	const auto [result, msg] = initializeService(env.jniEnv(), context.object<jobject>(),
			storage->getSmartServiceId().toStdString(),
			storage->getSmartSsdAid().toStdString());
#else
	const auto [result, msg] = initializeService();
#endif

	if (result != EidServiceResult::SUCCESS)
	{
		qCDebug(card_smart) << "Failed to initialize the service:" << QString::fromStdString(msg);
		return;
	}

	mInitialized = true;
	qCDebug(card_smart) << "SmartManager created";
}


bool SmartManager::isValid() const
{
	if (Env::getSingleton<ReaderManager>()->isWorkerThread())
	{
		return mInitialized;
	}

	qCCritical(card_smart) << "Only the ReaderManager thread is allowed to use a SmartManager!";
	return false;
}


SmartManager::~SmartManager()
{
	const auto [result, msg] = shutdownService();
	if (result != EidServiceResult::SUCCESS)
	{
		qCDebug(card_smart) << "Failed to shutdown the service:" << QString::fromStdString(msg);
	}

	qCDebug(card_smart) << "SmartManager destroyed";
}


bool SmartManager::smartAvailable() const
{
	if (!isValid())
	{
		return false;
	}

	const auto& eidStatus = status();
	if (eidStatus != EidStatus::INTERNAL_ERROR && eidStatus != EidStatus::NO_PROVISIONING)
	{
		return true;
	}

	const auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	if (!Env::getSingleton<VolatileSettings>()->isUsedAsSDK() && settings.doSmartUpdate())
	{
		const auto& [result, status] = updateSupportInfo();
		if (result != EidServiceResult::SUCCESS || status == EidSupportStatus::INTERNAL_ERROR)
		{
			qCDebug(card_smart) << "updateSupportInfo() failed - Cache could not be initialized";
		}
	}

	return settings.isSmartAvailable();
}


EidStatus SmartManager::status() const
{
	if (!isValid())
	{
		return EidStatus::INTERNAL_ERROR;
	}

	const auto& status = getSmartEidStatus();
	qCDebug(card_smart) << "getSmartEidStatus() finished with" << status;
	return status;
}


EidSupportStatusResult SmartManager::updateSupportInfo() const
{
	if (!isValid())
	{
		return {EidServiceResult::ERROR, EidSupportStatus::UNAVAILABLE};
	}

	const auto supportInfo = getSmartEidSupportInfo();
	qCDebug(card_smart) << "getSmartEidSupportInfo() finished with result" << supportInfo.mResult << "and status" << supportInfo.mStatus;

	if (supportInfo.mResult == EidServiceResult::SUCCESS)
	{
		auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
		switch (supportInfo.mStatus)
		{
			case EidSupportStatus::AVAILABLE:
			case EidSupportStatus::UP_TO_DATE:
			case EidSupportStatus::UPDATE_AVAILABLE:
				settings.setSmartAvailable(true);
				break;

			case EidSupportStatus::UNAVAILABLE:
				settings.setSmartAvailable(false);
				break;

			case EidSupportStatus::INTERNAL_ERROR:
				qCWarning(card_smart) << "Internal error of getSmartEidSupportInfo() - Cache update skipped";
				break;
		}
	}
	else
	{
		qCWarning(card_smart) << "getSmartEidSupportInfo() was not successful - Cache update skipped";
	}

	return supportInfo;
}


ServiceInformationResult SmartManager::serviceInformation() const
{
	if (!isValid())
	{
		return {EidServiceResult::UNDEFINED};
	}

	const auto& serviceInformation = getServiceInformation();
	qCDebug(card_smart) << "getServiceInformation() finished with" << serviceInformation;
	return serviceInformation;
}


EidServiceResult SmartManager::deleteSmart(const ProgressHandler& pHandler) const
{
	if (!isValid())
	{
		return EidServiceResult::ERROR;
	}

	const auto& deleteResult = deleteSmartEid(pHandler);
	qCDebug(card_smart) << "deleteSmartEid() finished with" << deleteResult;
	return deleteResult;
}


bool SmartManager::deletePersonalization() const
{
	if (!isValid())
	{
		return false;
	}

	const auto& deleteResult = ::deletePersonalization();
	qCDebug(card_smart) << "deletePersonalization() finished with" << deleteResult;
	return deleteResult == EidServiceResult::SUCCESS;
}


EidServiceResult SmartManager::installSmart(const ProgressHandler& pHandler) const
{
	if (!isValid())
	{
		return EidServiceResult::ERROR;
	}

	const auto& installResult = installSmartEid(pHandler);
	qCDebug(card_smart) << "installSmartEid() finished with" << installResult;
	return installResult;
}


InitializeResult SmartManager::initializePersonalization(const QString& pChallenge, const QString& pPin) const
{
	if (!isValid())
	{
		return InitializeResult();
	}

	return ::initializePersonalization(pChallenge.toStdString(), pPin.toStdString());
}


QByteArrayList SmartManager::performPersonalization(const QList<InputAPDUInfo>& pInputApdus) const
{
	if (!isValid())
	{
		return QByteArrayList();
	}

	QByteArrayList outputApduAsHex;

	for (const auto& inputApduInfo : std::as_const(pInputApdus))
	{
		const auto& cmdPersonalization = QByteArray(inputApduInfo.getInputApdu()).toHex().toStdString();
		const auto& [result, response] = ::performPersonalization(cmdPersonalization);
		if (result != EidServiceResult::SUCCESS)
		{
			qCWarning(card_smart) << "performPersonalization failed";
			break;
		}

		outputApduAsHex << QByteArray::fromStdString(response);
		ResponseApdu responseApdu(QByteArray::fromHex(outputApduAsHex.last()));
		if (!TransmitCommand::isAcceptable(inputApduInfo, responseApdu))
		{
			qCWarning(card_smart) << "Transmit unsuccessful. StatusCode does not start with acceptable status code" << inputApduInfo.getAcceptableStatusCodes();
			break;
		}
	}

	return outputApduAsHex;
}


PersonalizationResult SmartManager::finalizePersonalization(int pStatus) const
{
	if (!isValid())
	{
		return PersonalizationResult();
	}

	return ::finalizePersonalization(pStatus);
}


EstablishPaceChannelOutput SmartManager::prepareIdentification(const QByteArray& pChat) const
{
	if (!isValid())
	{
		return EstablishPaceChannelOutput(CardReturnCode::COMMAND_FAILED);
	}

	const auto& result = ::prepareIdentification(pChat.toHex().toStdString());

	EstablishPaceChannelOutput establishPaceChannelOutput;
	establishPaceChannelOutput.setPaceReturnCode(fromEidServiceResult(result.mResult));
	establishPaceChannelOutput.setCarCurr(QByteArray::fromStdString(result.mCertificationAuthorityReference));
	establishPaceChannelOutput.setEfCardAccess(fromHex(result.mEfCardAccess));
	establishPaceChannelOutput.setIdIcc(fromHex(result.mIdIcc));

	return establishPaceChannelOutput;
}


ResponseApduResult SmartManager::challenge() const
{
	if (!isValid())
	{
		return {CardReturnCode::COMMAND_FAILED};
	}

	const auto& result = getChallenge();

	ResponseApduResult responseApduResult;
	responseApduResult.mReturnCode = fromEidServiceResult(result.mResult);
	responseApduResult.mResponseApdu = ResponseApdu(fromHex(result.mData));

	return responseApduResult;
}


TerminalAndChipAuthenticationResult SmartManager::performTAandCA(const CVCertificateChain& pTerminalCvcChain, const QByteArray& pAuxiliaryData, const QByteArray& pSignature, const QByteArray& pPin, const QByteArray& pEphemeralPublicKey) const
{
	if (!isValid())
	{
		return {CardReturnCode::COMMAND_FAILED};
	}

	std::list<std::string> terminalCvcChain;
	for (const auto& certificate : pTerminalCvcChain)
	{
		terminalCvcChain.push_back(certificate->encode().toHex().toStdString());
	}

	const auto& result = ::performTAandCA(
			terminalCvcChain,
			pAuxiliaryData.toHex().toStdString(),
			pSignature.toHex().toStdString(),
			pPin.toStdString(),
			pEphemeralPublicKey.toHex().toStdString());

	TerminalAndChipAuthenticationResult tAandCAResult;
	tAandCAResult.mReturnCode = fromEidServiceResult(result.mResult);
	tAandCAResult.mEfCardSecurity = fromHex(result.mEfCardSecurity);
	tAandCAResult.mAuthenticationToken = fromHex(result.mAuthenticationToken);
	tAandCAResult.mNonce = fromHex(result.mNonce);

	return tAandCAResult;
}


ResponseApduResult SmartManager::transmit(const CommandApdu& pCmd) const
{
	if (!isValid())
	{
		return ResponseApduResult();
	}

	qCDebug(card_smart) << "Transmit command APDU:" << pCmd;

	const auto& result = performAPDUCommand(QByteArray(pCmd).toHex().toStdString());

	ResponseApduResult responseApduResult;
	responseApduResult.mReturnCode = fromEidServiceResult(result.mResult);
	if (result.mResult == EidServiceResult::SUCCESS)
	{
		responseApduResult.mResponseApdu = ResponseApdu(fromHex(result.mData));
		qCDebug(card_smart) << "Transmit response APDU:" << responseApduResult.mResponseApdu;
	}
	else
	{
		qCDebug(card_smart) << "Transmit failed with message:" << QByteArray::fromStdString(result.mData);
	}

	return responseApduResult;
}


void SmartManager::abortSDKWorkflow() const
{
#ifdef Q_OS_ANDROID
	QJniObject context = QNativeInterface::QAndroidApplication::context();
	if (!context.isValid())
	{
		qWarning() << "Android Context is not valid";
		return;
	}

	const auto& serviceToken = Env::getSingleton<AppSettings>()->getGeneralSettings().getIfdServiceToken();
	const auto& jServiceToken = QJniObject::fromString(serviceToken);
	QJniObject::callStaticMethod<void>(
			"com/governikus/ausweisapp2/AusweisApp2LocalIfdService",
			"abortWorkflow",
			"(Landroid/content/Context;Ljava/lang/String;)V",
			context.object<jobject>(),
			jServiceToken.object<jstring>());
#endif
}


QDebug operator<<(QDebug pDbg, const EidStatus& pStatus)
{
	QLatin1String status;
	switch (pStatus)
	{
		case EidStatus::NO_PROVISIONING:
			status = QLatin1String("NO_PROVISIONING");
			break;

		case EidStatus::NO_PERSONALIZATION:
			status = QLatin1String("NO_PERSONALIZATION");
			break;

		case EidStatus::UNUSABLE:
			status = QLatin1String("UNUSABLE");
			break;

		case EidStatus::PERSONALIZED:
			status = QLatin1String("PERSONALIZED");
			break;

		case EidStatus::INTERNAL_ERROR:
			status = QLatin1String("INTERNAL_ERROR");
			break;

		case EidStatus::CERT_EXPIRED:
			status = QLatin1String("CERT_EXPIRED");
			break;
	}

	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << status << " 0x" << Qt::hex << static_cast<int>(pStatus);
	return pDbg;
}


QDebug operator<<(QDebug pDbg, const EidSupportStatus& pInfo)
{
	QLatin1String info;
	switch (pInfo)
	{
		case EidSupportStatus::UNAVAILABLE:
			info = QLatin1String("UNAVAILABLE");
			break;

		case EidSupportStatus::UPDATE_AVAILABLE:
			info = QLatin1String("UPDATE_AVAILABLE");
			break;

		case EidSupportStatus::UP_TO_DATE:
			info = QLatin1String("UP_TO_DATE");
			break;

		case EidSupportStatus::INTERNAL_ERROR:
			info = QLatin1String("INTERNAL_ERROR");
			break;

		case EidSupportStatus::AVAILABLE:
			info = QLatin1String("AVAILABLE");
			break;
	}

	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << info << " 0x" << Qt::hex << static_cast<int>(pInfo);
	return pDbg;
}


QDebug operator<<(QDebug pDbg, const EidServiceResult& pResult)
{
	QLatin1String result;
	switch (pResult)
	{
		case EidServiceResult::SUCCESS:
			result = QLatin1String("SUCCESS");
			break;

		case EidServiceResult::UNDEFINED:
			result = QLatin1String("UNDEFINED");
			break;

		case EidServiceResult::INFO:
			result = QLatin1String("INFO");
			break;

		case EidServiceResult::WARN:
			result = QLatin1String("WARN");
			break;

		case EidServiceResult::ERROR:
			result = QLatin1String("ERROR");
			break;

		case EidServiceResult::UNSUPPORTED:
			result = QLatin1String("UNSUPPORTED");
			break;

		case EidServiceResult::OVERLOAD_PROTECTION:
			result = QLatin1String("OVERLOAD_PROTECTION");
			break;

		case EidServiceResult::UNDER_MAINTENANCE:
			result = QLatin1String("UNDER_MAINTENANCE");
			break;

		case EidServiceResult::NFC_NOT_ACTIVATED:
			result = QLatin1String("NFC_NOT_ACTIVATED");
			break;

		case EidServiceResult::INTEGRITY_CHECK_FAILED:
			result = QLatin1String("INTEGRITY_CHECK_FAILED");
			break;

		case EidServiceResult::NOT_AUTHENTICATED:
			result = QLatin1String("NOT_AUTHENTICATED");
			break;

		case EidServiceResult::NETWORK_CONNECTION_ERROR:
			result = QLatin1String("NETWORK_CONNECTION_ERROR");
			break;
	}

	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << result << " 0x" << Qt::hex << static_cast<int>(pResult);
	return pDbg;
}


QDebug operator<<(QDebug pDbg, const SmartEidType& pType)
{
	QLatin1String type;
	switch (pType)
	{
		case SmartEidType::UNKNOWN:
			type = QLatin1String("UNKNOWN");
			break;

		case SmartEidType::APPLET:
			type = QLatin1String("APPLET");
			break;

		case SmartEidType::NON_APPLET:
			type = QLatin1String("NON_APPLET");
			break;
	}

	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << type << " 0x" << Qt::hex << static_cast<int>(pType);
	return pDbg;
}


QDebug operator<<(QDebug pDbg, const ServiceInformationResult& pResult)
{
	QDebugStateSaver saver(pDbg);
	pDbg << "{" << pResult.mResult << "|" << pResult.mType << "|" << QString::fromStdString(pResult.mChallengeType) << "}";

	return pDbg;
}
