/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowModel.h"

#include "AppSettings.h"
#include "Email.h"
#include "Env.h"
#include "FuncUtils.h"
#include "Initializer.h"
#include "ReaderManager.h"
#include "context/AuthContext.h"

#include <QDesktopServices>


using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QList<int>>("QList<ReaderManagerPluginType>");
		})


#if defined(Q_OS_IOS)
	#define DETECTION_ONLY false
#else
	#define DETECTION_ONLY true
#endif


WorkflowModel::WorkflowModel()
	: QObject()
	, mContext()
	, mRemoteIfdManager([this]() {
				return !mContext.isNull();
			}, DETECTION_ONLY)
{
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireCardInserted, this, &WorkflowModel::fireHasCardChanged);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireCardRemoved, this, &WorkflowModel::fireHasCardChanged);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded, this, &WorkflowModel::fireHasCardChanged);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderRemoved, this, &WorkflowModel::fireHasCardChanged);

	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireDeveloperOptionsChanged, this, &WorkflowModel::fireSupportedPluginTypesChanged);
}


void WorkflowModel::resetWorkflowContext(const QSharedPointer<WorkflowContext>& pContext)
{
	const bool finishedSuccessfully = !isError();
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &WorkflowModel::fireCurrentStateChanged);
		connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &WorkflowModel::fireStateEntered);
		connect(mContext.data(), &WorkflowContext::fireResultChanged, this, &WorkflowModel::fireResultChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderPluginTypesChanged, this, &WorkflowModel::fireReaderPluginTypeChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderPluginTypesChanged, this, &WorkflowModel::fireHasCardChanged);
		connect(mContext.data(), &WorkflowContext::fireCardConnectionChanged, this, &WorkflowModel::fireSelectedReaderChanged);
		connect(mContext.data(), &WorkflowContext::fireCardConnectionChanged, this, &WorkflowModel::fireHasCardChanged);
		connect(mContext.data(), &WorkflowContext::fireEidTypeMismatchChanged, this, &WorkflowModel::fireEidTypeMismatchErrorChanged);
		connect(mContext.data(), &WorkflowContext::fireNextWorkflowPending, this, &WorkflowModel::fireNextWorkflowPendingChanged);
		connect(mContext.data(), &WorkflowContext::fireRemoveCardFeedbackChanged, this, &WorkflowModel::fireRemoveCardFeedbackChanged);
		connect(mContext.data(), &WorkflowContext::firePaceResultUpdated, this, &WorkflowModel::onPaceResultUpdated);
		connect(mContext.data(), &WorkflowContext::firePaceResultUpdated, this, &WorkflowModel::fireLastReturnCodeChanged);
		Q_EMIT fireWorkflowStarted();
	}
	else
	{
		Q_EMIT fireWorkflowFinished(finishedSuccessfully);
	}

	Q_EMIT fireCurrentStateChanged(getCurrentState());
	Q_EMIT fireResultChanged();
}


QString WorkflowModel::getCurrentState() const
{
	return mContext ? mContext->getCurrentState() : QString();
}


QString WorkflowModel::getResultString() const
{
	return mContext ? mContext->getStatus().toErrorDescription(true) : QString();
}


bool WorkflowModel::isError() const
{
	return mContext && mContext->getStatus().isError();
}


bool WorkflowModel::isMaskedError() const
{
	return mContext && mContext->getStatus().isMessageMasked();
}


bool WorkflowModel::isPukInoperative() const
{
	if (!mContext)
	{
		return false;
	}
	return mContext->getStatus().getStatusCode() == GlobalStatus::Code::Card_Puk_Blocked;
}


CardReturnCode WorkflowModel::getLastReturnCode() const
{
	if (mContext)
	{
		return mContext->getLastPaceResult();
	}
	return CardReturnCode::UNDEFINED;
}


ReaderManagerPluginType WorkflowModel::getReaderPluginType() const
{
	if (mContext && !mContext->getReaderPluginTypes().isEmpty())
	{
		return mContext->getReaderPluginTypes().at(0);
	}

	return ReaderManagerPluginType::UNKNOWN;
}


void WorkflowModel::setReaderPluginType(ReaderManagerPluginType pReaderPluginType)
{
	if (!mContext)
	{
		return;
	}
	mContext->setReaderPluginTypes({pReaderPluginType});
}


void WorkflowModel::insertCard(ReaderManagerPluginType pType) const
{
	auto* const readerManager = Env::getSingleton<ReaderManager>();
	const auto& readerInfos = readerManager->getReaderInfos(ReaderFilter({pType}));

	for (const auto& readerInfo : readerInfos)
	{
		readerManager->insert(readerInfo);
	}
}


void WorkflowModel::insertSimulator() const
{
	insertCard(ReaderManagerPluginType::SIMULATOR);
}


void WorkflowModel::continueWorkflow()
{
	if (mContext)
	{
		mContext->setStateApproved();
	}
}


void WorkflowModel::cancelWorkflow()
{
	if (mContext)
	{
		Q_EMIT mContext->fireCancelWorkflow();
	}
}


void WorkflowModel::startScanExplicitly()
{
	if (mContext)
	{
		Q_EMIT mContext->fireReaderPluginTypesChanged(true);
	}
}


bool WorkflowModel::isBasicReader() const
{
	if (mContext && mContext->getCardConnection())
	{
		return mContext->getCardConnection()->getReaderInfo().isBasicReader();
	}

	return true;
}


bool WorkflowModel::isRemoteReader() const
{
	if (mContext && mContext->getCardConnection())
	{
		return mContext->getCardConnection()->getReaderInfo().getPluginType() == ReaderManagerPluginType::REMOTE_IFD;
	}

	return false;
}


bool WorkflowModel::getCardInitiallyAppeared() const
{
	if (!mContext)
	{
		return false;
	}
	return mContext->getCardInitiallyAppeared();
}


bool WorkflowModel::hasCard() const
{
	if (!mContext)
	{
		return false;
	}

	const auto& readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter({getReaderPluginType()}));
	const auto& readersWithEid = filter<ReaderInfo>([](const ReaderInfo& i){return i.hasEid();}, readerInfos);
	return !readersWithEid.isEmpty();
}


QList<ReaderManagerPluginType> WorkflowModel::getSupportedReaderPluginTypes() const
{
	auto supported = Enum<ReaderManagerPluginType>::getList();

	if (!Env::getSingleton<AppSettings>()->getSimulatorSettings().isEnabled())
	{
		supported.removeOne(ReaderManagerPluginType::SIMULATOR);
	}

	return supported;
}


bool WorkflowModel::getNextWorkflowPending() const
{
	return mContext->hasNextWorkflowPending();
}


GlobalStatus::Code WorkflowModel::getStatusCode() const
{
	return mContext ? mContext->getStatus().getStatusCode() : GlobalStatus::Code::Unknown_Error;
}


QString WorkflowModel::getStatusCodeDisplayString() const
{
	//: ALL_PLATFORMS
	return tr("Error code: %1").arg(getEnumName(getStatusCode()));
}


GAnimation WorkflowModel::getStatusCodeAnimation() const
{
	switch (getStatusCode())
	{
		case GlobalStatus::Code::No_Error:
			return GAnimation::STATUS_OK;

		case GlobalStatus::Code::Network_ServiceUnavailable:
		case GlobalStatus::Code::Network_ServerError:
		case GlobalStatus::Code::Network_ClientError:
		case GlobalStatus::Code::Network_Ssl_Establishment_Error:
		case GlobalStatus::Code::Network_TimeOut:
		case GlobalStatus::Code::Network_Proxy_Error:
		case GlobalStatus::Code::Network_Other_Error:
			return GAnimation::NETWORK_ERROR;

		case GlobalStatus::Code::Paos_Generic_Server_Error:
		case GlobalStatus::Code::Workflow_Cannot_Confirm_IdCard_Authenticity:
		case GlobalStatus::Code::Card_Not_Found:
		case GlobalStatus::Code::Card_Communication_Error:
		case GlobalStatus::Code::Card_Protocol_Error:
		case GlobalStatus::Code::Card_Unexpected_Transmit_Status:
		case GlobalStatus::Code::Card_Cancellation_By_User:
		case GlobalStatus::Code::Card_Input_TimeOut:
		case GlobalStatus::Code::Card_Pin_Deactivated:
		case GlobalStatus::Code::Card_Pin_Not_Blocked:
		case GlobalStatus::Code::Card_NewPin_Mismatch:
		case GlobalStatus::Code::Card_NewPin_Invalid_Length:
		case GlobalStatus::Code::Card_ValidityVerificationFailed:
			return GAnimation::CARD_ERROR;

		case GlobalStatus::Code::Card_Invalid_Pin:
			return GAnimation::PIN_ERROR;

		case GlobalStatus::Code::Card_Invalid_Can:
			return GAnimation::CAN_ERROR;

		case GlobalStatus::Code::Card_Invalid_Puk:
			return GAnimation::PUK_ERROR;

		case GlobalStatus::Code::Card_Puk_Blocked:
			return GAnimation::PUK_BLOCKED;

		default:
			return GAnimation::STATUS_ERROR;
	}
}


QString WorkflowModel::getStatusHintText() const
{
	if (getStatusCode() == GlobalStatus::Code::Card_ValidityVerificationFailed)
	{
		return tr("Contact your local citizens' office (B\u00FCrgeramt) to apply for a new ID card or to unblock the ID card.");
	}

	return QString();
}


bool WorkflowModel::showRemoveCardFeedback() const
{
	return mContext && mContext->showRemoveCardFeedback();
}


void WorkflowModel::setInitialPluginType()
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	const GeneralSettings& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

	const QString& preferredTechnologyString = settings.getPreferredTechnology();
	const auto& preferredTechnology = Enum<ReaderManagerPluginType>::fromString(preferredTechnologyString, ReaderManagerPluginType::UNKNOWN);

	if (preferredTechnology == ReaderManagerPluginType::UNKNOWN || !getSupportedReaderPluginTypes().contains(preferredTechnology))
	{
		const auto& pluginInfo = Env::getSingleton<ReaderManager>()->getPluginInfo(ReaderManagerPluginType::NFC);
		if (pluginInfo.isAvailable())
		{
			setReaderPluginType(ReaderManagerPluginType::NFC);
			return;
		}

		setReaderPluginType(ReaderManagerPluginType::REMOTE_IFD);
		return;
	}
	setReaderPluginType(preferredTechnology);
#else
	if (!mContext)
	{
		return;
	}
	mContext->setReaderPluginTypes({ReaderManagerPluginType::PCSC, ReaderManagerPluginType::REMOTE_IFD, ReaderManagerPluginType::SIMULATOR});
#endif
}


bool WorkflowModel::isCancellationByUser() const
{
	if (!mContext)
	{
		return false;
	}
	return mContext->getStatus().isCancellationByUser();
}


QString WorkflowModel::getEmailHeader() const
{
	if (!mContext)
	{
		return QString();
	}

	return tr("%1 error report - %2").arg(QCoreApplication::applicationName(), mContext->getStatus().toErrorDescription());
}


QString WorkflowModel::getEmailBody(bool pPercentEncoding, bool pAddLogNotice) const
{
	if (!mContext)
	{
		return QString();
	}
	const auto& authContext = qobject_cast<AuthContext*>(mContext);

	return generateMailBody(mContext->getStatus(), authContext.isNull() ? QUrl() : authContext->getTcTokenUrl(), pPercentEncoding, pAddLogNotice);
}


void WorkflowModel::sendResultMail() const
{
	Q_ASSERT(mContext);
	QString mailSubject = getEmailHeader();
	QString mailBody = getEmailBody(true, true);
	const auto url = QUrl(QStringLiteral("mailto:support@ausweisapp.de?subject=%1&body=%2").arg(mailSubject, mailBody));

	QDesktopServices::openUrl(url);
}


void WorkflowModel::onPaceResultUpdated()
{
	if (mContext->getLastPaceResult() == CardReturnCode::OK_PUK)
	{
		Q_EMIT fireOnPinUnlocked();
		return;
	}
	if (mContext->getLastPaceResult() == CardReturnCode::OK_CAN)
	{
		Q_EMIT fireOnCanSuccess();
		return;
	}

	Q_EMIT fireOnPasswordUsed();

}


QString WorkflowModel::eidTypeMismatchError() const
{
	if (mContext && mContext->eidTypeMismatch())
	{
		//: ALL_PLATFORMS
		return tr("The used ID card is not accepted by the server. Please remove the ID card from your device or card reader and use a suitable ID card or contact the service provider.");
	}

	return QString();
}
