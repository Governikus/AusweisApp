/*!
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SmartManager.h"

#include "AppSettings.h"
#include "ReaderManager.h"
#include "SecureStorage.h"
#include "command/TransmitCommand.h"

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
			storage->getSmartVersionTag().toStdString(),
			storage->getSmartSsdAid().toStdString());

	if (result != EidServiceResult::SUCCESS)
	{
		qCDebug(card_smart) << "Failed to initialize the service:" << QString::fromStdString(msg);
		return;
	}

#endif

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
#ifdef Q_OS_ANDROID
	const auto [result, msg] = shutdownService();
	if (result != EidServiceResult::SUCCESS)
	{
		qCDebug(card_smart) << "Failed to shutdown the service:" << QString::fromStdString(msg);
	}
#endif

	qCDebug(card_smart) << "SmartManager destroyed";
}


EidStatus SmartManager::status() const
{
	if (!isValid())
	{
		return EidStatus::UNAVAILABLE;
	}

	const auto& status = getSmartEidStatus();
	qCDebug(card_smart) << "getSmartEidStatus() finished with" << status;
	return status;
}


EidUpdateInfo SmartManager::updateInfo()
{
	if (!isValid())
	{
		return EidUpdateInfo::UNAVAILABLE;
	}

	const auto& updateInfo = getUpdateInfo();
	qCDebug(card_smart) << "getUpdateInfo() finished with" << updateInfo;
	return updateInfo;
}


bool SmartManager::deleteSmart(const ProgressHandler& pHandler) const
{
	if (!isValid())
	{
		return false;
	}

	const auto& deleteResult = deleteSmartEid(pHandler);
	qCDebug(card_smart) << "deleteSmartEid() finished with" << deleteResult;
	return deleteResult == EidServiceResult::SUCCESS;
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


bool SmartManager::installSmart(const ProgressHandler& pHandler) const
{
	if (!isValid())
	{
		return false;
	}

	const auto& installResult = installSmartEid(pHandler);
	qCDebug(card_smart) << "installSmartEid() finished with" << installResult;
	return installResult == EidServiceResult::SUCCESS;
}


InitializeResult SmartManager::initializePersonalization(const QString& pChallenge, const QString& pPin) const
{
	if (!isValid())
	{
		return InitializeResult();
	}

	return ::initializePersonalization(pChallenge.toStdString(), pPin.toStdString());
}


QByteArrayList SmartManager::performPersonalization(const QVector<InputAPDUInfo>& pInputApdus) const
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


PersonalizationResult SmartManager::finalizePersonalization() const
{
	if (!isValid())
	{
		return PersonalizationResult();
	}

#ifdef Q_OS_ANDROID
	return ::finalizePersonalization();

#else
	return {EidServiceResult::SUCCESS};

#endif
}


EstablishPaceChannelOutput SmartManager::prepareIdentification(const QByteArray& pChat) const
{
	if (!isValid())
	{
		return EstablishPaceChannelOutput(CardReturnCode::COMMAND_FAILED);
	}

#ifdef Q_OS_IOS
	const auto& result = ::prepareIdentification(pChat.toHex().toStdString());

	EstablishPaceChannelOutput establishPaceChannelOutput;
	establishPaceChannelOutput.setPaceReturnCode(fromEidServiceResult(result.mResult));
	establishPaceChannelOutput.setCarCurr(QByteArray::fromStdString(result.mCertificationAuthorityReference));
	establishPaceChannelOutput.setEfCardAccess(fromHex(result.mEfCardAccess));
	establishPaceChannelOutput.setIdIcc(fromHex(result.mIdIcc));

	return establishPaceChannelOutput;

#else
	Q_UNUSED(pChat)
	return EstablishPaceChannelOutput(CardReturnCode::COMMAND_FAILED);

#endif
}


ResponseApduResult SmartManager::challenge() const
{
	if (!isValid())
	{
		return {CardReturnCode::COMMAND_FAILED};
	}

#ifdef Q_OS_IOS
	const auto& result = getChallenge();

	ResponseApduResult responseApduResult;
	responseApduResult.mReturnCode = fromEidServiceResult(result.mResult);
	responseApduResult.mResponseApdu = ResponseApdu(fromHex(result.mData));

	return responseApduResult;

#else
	return {CardReturnCode::COMMAND_FAILED};

#endif
}


TerminalAndChipAuthenticationResult SmartManager::performTAandCA(const CVCertificateChain& pTerminalCvcChain, const QByteArray& pAuxiliaryData, const QByteArray& pSignature, const QByteArray& pPin, const QByteArray& pEphemeralPublicKey) const
{
	if (!isValid())
	{
		return {CardReturnCode::COMMAND_FAILED};
	}

#ifdef Q_OS_IOS
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

#else
	Q_UNUSED(pTerminalCvcChain)
	Q_UNUSED(pAuxiliaryData)
	Q_UNUSED(pSignature)
	Q_UNUSED(pPin)
	Q_UNUSED(pEphemeralPublicKey)
	return {CardReturnCode::COMMAND_FAILED};

#endif
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


#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
	#define HEX hex
#else
	#define HEX Qt::hex
#endif


QDebug operator<<(QDebug pDbg, const EidStatus& pStatus)
{
	const auto& toString = [](const EidStatus& status){
				switch (status)
				{
					case EidStatus::UNAVAILABLE:
						return QLatin1String("UNAVAILABLE");

					case EidStatus::NO_PROVISIONING:
						return QLatin1String("NO_PROVISIONING");

					case EidStatus::NO_PERSONALIZATION:
						return QLatin1String("NO_PERSONALIZATION");

					case EidStatus::APPLET_UNUSABLE:
						return QLatin1String("APPLET_UNUSABLE");

					case EidStatus::PERSONALIZED:
						return QLatin1String("PERSONALIZED");

					case EidStatus::INTERNAL_ERROR:
						return QLatin1String("INTERNAL_ERROR");
				}

				Q_UNREACHABLE();
			};

	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << toString(pStatus) << " 0x" << HEX << static_cast<int>(pStatus);
	return pDbg;
}


QDebug operator<<(QDebug pDbg, const EidUpdateInfo& pInfo)
{
	const auto& toString = [](const EidUpdateInfo& info){
				switch (info)
				{
					case EidUpdateInfo::UNAVAILABLE:
						return QLatin1String("UNAVAILABLE");

					case EidUpdateInfo::NO_PROVISIONING:
						return QLatin1String("NO_PROVISIONING");

					case EidUpdateInfo::UPDATE_AVAILABLE:
						return QLatin1String("UPDATE_AVAILABLE");

					case EidUpdateInfo::UP_TO_DATE:
						return QLatin1String("UP_TO_DATE");

					case EidUpdateInfo::INTERNAL_ERROR:
						return QLatin1String("INTERNAL_ERROR");
				}

				Q_UNREACHABLE();
			};

	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << toString(pInfo) << " 0x" << HEX << static_cast<int>(pInfo);
	return pDbg;
}


QDebug operator<<(QDebug pDbg, const EidServiceResult& pResult)
{
	const auto& toString = [](const EidServiceResult& result){
				switch (result)
				{
					case EidServiceResult::SUCCESS:
						return QLatin1String("SUCCESS");

					case EidServiceResult::UNDEFINED:
						return QLatin1String("UNDEFINED");

					case EidServiceResult::INFO:
						return QLatin1String("INFO");

					case EidServiceResult::WARN:
						return QLatin1String("WARN");

					case EidServiceResult::ERROR:
						return QLatin1String("ERROR");

					case EidServiceResult::UNSUPPORTED:
						return QLatin1String("UNSUPPORTED");
				}

				Q_UNREACHABLE();
			};

	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << toString(pResult) << " 0x" << HEX << static_cast<int>(pResult);
	return pDbg;
}
