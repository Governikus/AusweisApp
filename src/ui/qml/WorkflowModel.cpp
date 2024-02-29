/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowModel.h"

#include "AppSettings.h"
#include "ApplicationModel.h"
#include "Email.h"
#include "Env.h"
#include "FuncUtils.h"
#include "GeneralSettings.h"
#include "Initializer.h"
#include "PinResetInformationModel.h"
#include "ReaderConfiguration.h"
#include "ReaderManager.h"
#include "SmartModel.h"
#include "context/AuthContext.h"

#include <QDesktopServices>


using namespace governikus;

INIT_FUNCTION([] {
			qRegisterMetaType<QList<int>>("QList<ReaderManagerPlugInType>");
		})

WorkflowModel::WorkflowModel(QObject* pParent)
	: QObject(pParent)
	, mContext()
#if defined(Q_OS_IOS)
	, mRemoteScanWasRunning(false)
#endif
{
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireCardInserted, this, &WorkflowModel::fireHasCardChanged);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireCardRemoved, this, &WorkflowModel::fireHasCardChanged);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded, this, &WorkflowModel::fireHasCardChanged);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderRemoved, this, &WorkflowModel::fireHasCardChanged);
	connect(Env::getSingleton<SmartModel>(), &SmartModel::fireSmartStateChanged, this, &WorkflowModel::fireIsCurrentSmartCardAllowedChanged);

	connect(Env::getSingleton<ApplicationModel>(), &ApplicationModel::fireApplicationStateChanged, this, &WorkflowModel::onApplicationStateChanged);

	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireDeveloperOptionsChanged, this, &WorkflowModel::fireSupportedPlugInTypesChanged);
}


void WorkflowModel::resetWorkflowContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &WorkflowModel::fireCurrentStateChanged);
		connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &WorkflowModel::fireStateEntered);
		connect(mContext.data(), &WorkflowContext::fireResultChanged, this, &WorkflowModel::fireResultChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderPlugInTypesChanged, this, &WorkflowModel::fireReaderPlugInTypeChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderPlugInTypesChanged, this, &WorkflowModel::fireHasCardChanged);
		connect(mContext.data(), &WorkflowContext::fireCardConnectionChanged, this, &WorkflowModel::fireSelectedReaderChanged);
		connect(mContext.data(), &WorkflowContext::fireCardConnectionChanged, this, &WorkflowModel::fireHasCardChanged);
		connect(mContext.data(), &WorkflowContext::fireEidTypeMismatchChanged, this, &WorkflowModel::fireIsCurrentSmartCardAllowedChanged);
		connect(mContext.data(), &WorkflowContext::fireEidTypeMismatchChanged, this, &WorkflowModel::fireEidTypeMismatchErrorChanged);
		connect(mContext.data(), &WorkflowContext::fireNextWorkflowPending, this, &WorkflowModel::fireNextWorkflowPendingChanged);
		connect(mContext.data(), &WorkflowContext::fireRemoveCardFeedbackChanged, this, &WorkflowModel::fireRemoveCardFeedbackChanged);
		Q_EMIT fireWorkflowStarted();
	}
	else
	{
		Q_EMIT fireWorkflowFinished();
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


ReaderManagerPlugInType WorkflowModel::getReaderPlugInType() const
{
	if (mContext && !mContext->getReaderPlugInTypes().isEmpty())
	{
		return mContext->getReaderPlugInTypes().at(0);
	}

	return ReaderManagerPlugInType::UNKNOWN;
}


void WorkflowModel::setReaderPlugInType(ReaderManagerPlugInType pReaderPlugInType)
{
	if (!mContext)
	{
		return;
	}
	mContext->setReaderPlugInTypes({pReaderPlugInType});

	auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	settings.setLastReaderPluginType(getEnumName(pReaderPlugInType));
}


void WorkflowModel::insertCard(ReaderManagerPlugInType pType) const
{
	auto* const readerManager = Env::getSingleton<ReaderManager>();
	const auto& readerInfos = readerManager->getReaderInfos(ReaderFilter({pType}));

	for (const auto& readerInfo : readerInfos)
	{
		readerManager->insert(readerInfo);
	}
}


void WorkflowModel::insertSmartCard()
{
	insertCard(ReaderManagerPlugInType::SMART);
}


void WorkflowModel::insertSimulator()
{
	insertCard(ReaderManagerPlugInType::SIMULATOR);
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
		Q_EMIT mContext->fireReaderPlugInTypesChanged(true);
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
		return mContext->getCardConnection()->getReaderInfo().getPlugInType() == ReaderManagerPlugInType::REMOTE_IFD;
	}

	return false;
}


bool WorkflowModel::hasCard() const
{
	if (!mContext)
	{
		return false;
	}

	const auto& readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter({getReaderPlugInType()}));
	const auto& readersWithEid = filter<ReaderInfo>([](const ReaderInfo& i){return i.hasEid();}, readerInfos);
	return !readersWithEid.isEmpty();
}


bool WorkflowModel::isCurrentSmartCardAllowed() const
{
	const auto& mobileEidType = Env::getSingleton<SmartModel>()->getMobileEidType();
	return mContext && mContext->isMobileEidTypeAllowed(mobileEidType);
}


bool WorkflowModel::isSmartSupported() const
{
	return getSupportedReaderPlugInTypes().contains(ReaderManagerPlugInType::SMART);
}


QList<ReaderManagerPlugInType> WorkflowModel::getSupportedReaderPlugInTypes() const
{
	auto supported = Enum<ReaderManagerPlugInType>::getList();

	if (!Env::getSingleton<AppSettings>()->getGeneralSettings().isSimulatorEnabled())
	{
		supported.removeOne(ReaderManagerPlugInType::SIMULATOR);
	}

#if !__has_include("SmartManager.h")
	supported.removeOne(ReaderManagerPlugInType::SMART);
#endif

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


QString WorkflowModel::getStatusCodeImage() const
{
	switch (getStatusCode())
	{
		case GlobalStatus::Code::Network_ServiceUnavailable:
		case GlobalStatus::Code::Network_ServerError:
		case GlobalStatus::Code::Network_ClientError:
		case GlobalStatus::Code::Network_Ssl_Establishment_Error:
		case GlobalStatus::Code::Network_TimeOut:
		case GlobalStatus::Code::Network_Proxy_Error:
		case GlobalStatus::Code::Network_Other_Error:
			return QStringLiteral("qrc:///images/workflow_error_network_%1.svg");

		case GlobalStatus::Code::Paos_Generic_Server_Error:
		case GlobalStatus::Code::Workflow_Cannot_Confirm_IdCard_Authenticity:
		case GlobalStatus::Code::Card_Not_Found:
		case GlobalStatus::Code::Card_Communication_Error:
		case GlobalStatus::Code::Card_Protocol_Error:
		case GlobalStatus::Code::Card_Unexpected_Transmit_Status:
		case GlobalStatus::Code::Card_Cancellation_By_User:
		case GlobalStatus::Code::Card_Input_TimeOut:
		case GlobalStatus::Code::Card_Pin_Deactivated:
		case GlobalStatus::Code::Card_Pin_Blocked:
		case GlobalStatus::Code::Card_Pin_Not_Blocked:
		case GlobalStatus::Code::Card_NewPin_Mismatch:
		case GlobalStatus::Code::Card_NewPin_Invalid_Length:
		case GlobalStatus::Code::Card_ValidityVerificationFailed:
			return QStringLiteral("qrc:///images/workflow_error_card_%1.svg");

		case GlobalStatus::Code::Card_Invalid_Pin:
			return QStringLiteral("qrc:///images/workflow_error_wrong_pin_%1.svg");

		case GlobalStatus::Code::Card_Invalid_Can:
			return QStringLiteral("qrc:///images/workflow_error_wrong_can_%1.svg");

		case GlobalStatus::Code::Card_Invalid_Puk:
			return QStringLiteral("qrc:///images/workflow_error_wrong_puk_%1.svg");

		case GlobalStatus::Code::Card_Puk_Blocked:
			return QStringLiteral("qrc:///images/workflow_error_puk_blocked_%1.svg");

		case GlobalStatus::Code::No_Error:
			return QStringLiteral("qrc:///images/status_ok_%1.svg");

		default:
			return QStringLiteral("qrc:///images/status_error_%1.svg");
	}
}


QString WorkflowModel::getStatusHintText() const
{
	switch (getStatusCode())
	{
		case GlobalStatus::Code::Card_Puk_Blocked:
			return Env::getSingleton<PinResetInformationModel>()->getRequestNewPinHint();

		case GlobalStatus::Code::Card_Pin_Deactivated:
			return Env::getSingleton<PinResetInformationModel>()->getActivateOnlineFunctionHint();

		case GlobalStatus::Code::Card_ValidityVerificationFailed:
			return tr("Contact your local citizens' office (B\u00FCrgeramt) to apply for a new ID card or to unblock the ID card.");

		case GlobalStatus::Code::Card_Smart_Invalid:
			//: LABEL ANDROID IOS The hint text that is shwon right above the redirect button that appears when a user tried to usa an unusable Smart-eID
			return tr("Renew your Smart-eID and set a new PIN in the Smart-eID menu.");

		default:
			return QString();
	}
}


QString WorkflowModel::getStatusHintActionText() const
{
	switch (getStatusCode())
	{
		case GlobalStatus::Code::Card_Puk_Blocked:
			return Env::getSingleton<PinResetInformationModel>()->getPinResetActionText();

		case GlobalStatus::Code::Card_Pin_Deactivated:
			return Env::getSingleton<PinResetInformationModel>()->getActivateOnlineFunctionActionText();

		case GlobalStatus::Code::Card_Smart_Invalid:
			//: LABEL ANDROID IOS The text on the redirect button that appears when the user tried to use an unusable Smart-eID
			return tr("Go to Smart-eID menu");

		default:
			return QString();
	}
}


bool WorkflowModel::invokeStatusHintAction()
{
	switch (getStatusCode())
	{
		case GlobalStatus::Code::Card_Puk_Blocked:
		case GlobalStatus::Code::Card_Pin_Deactivated:
			QDesktopServices::openUrl(Env::getSingleton<PinResetInformationModel>()->getPinResetUrl());
			return true;

		case GlobalStatus::Code::Card_Smart_Invalid:
			Q_EMIT fireShowUiRequest(UiModule::SMART_EID);
			return true;

		default:
			return false;
	}
}


bool WorkflowModel::showRemoveCardFeedback() const
{
	return mContext && mContext->showRemoveCardFeedback();
}


void WorkflowModel::setRemoveCardFeedback(bool pEnabled)
{
	if (mContext)
	{
		mContext->setRemoveCardFeedback(pEnabled);
	}
}


void WorkflowModel::setInitialPluginType()
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	const GeneralSettings& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

	const QString& lastReaderPluginTypeString = settings.getLastReaderPluginType();
	const auto& lastReaderPluginType = Enum<ReaderManagerPlugInType>::fromString(lastReaderPluginTypeString, ReaderManagerPlugInType::UNKNOWN);

	if (lastReaderPluginType == ReaderManagerPlugInType::UNKNOWN || !getSupportedReaderPlugInTypes().contains(lastReaderPluginType))
	{
		setReaderPlugInType(ReaderManagerPlugInType::NFC);
		return;
	}
	setReaderPlugInType(lastReaderPluginType);
#else
	if (!mContext)
	{
		return;
	}
	mContext->setReaderPlugInTypes({ReaderManagerPlugInType::PCSC, ReaderManagerPlugInType::REMOTE_IFD, ReaderManagerPlugInType::SIMULATOR});
#endif
}


bool WorkflowModel::shouldSkipResultView() const
{
	if (!mContext)
	{
		return false;
	}
	// We deliberately don't want to use GlobalStatus::isCancellationByUser(), because that would also skip the
	// ResultView when the user pressed Cancel on his card reader.
	return mContext->getStatus().getStatusCode() == GlobalStatus::Code::Workflow_Cancellation_By_User;
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
	QString url = QStringLiteral("mailto:support@ausweisapp.de?subject=%1&body=%2").arg(mailSubject, mailBody);

	QDesktopServices::openUrl(url);
}


void WorkflowModel::onApplicationStateChanged(bool pIsAppInForeground)
{
	if (!mContext)
	{
		return;
	}

#if defined(Q_OS_IOS)
	if (pIsAppInForeground)
	{
		if (mRemoteScanWasRunning)
		{
			Env::getSingleton<ReaderManager>()->startScan(ReaderManagerPlugInType::REMOTE_IFD);
			mRemoteScanWasRunning = false;
		}
	}
	else
	{
		mRemoteScanWasRunning = Env::getSingleton<ReaderManager>()->getPlugInInfo(ReaderManagerPlugInType::REMOTE_IFD).isScanRunning();
		if (mRemoteScanWasRunning)
		{
			Env::getSingleton<ReaderManager>()->stopScan(ReaderManagerPlugInType::REMOTE_IFD);
		}
	}
#else
	Q_UNUSED(pIsAppInForeground)
#endif
}


QString WorkflowModel::eidTypeMismatchError() const
{
	if (mContext && mContext->eidTypeMismatch())
	{
		if (mContext->isSmartCardUsed())
		{
			if (mContext->getAcceptedEidTypes().contains(AcceptedEidType::CARD_CERTIFIED))
			{
				//: INFO ALL_PLATFORMS
				return tr("The used Smart-eID is not accepted by the server. Please restart the remote service on your connected smartphone and try again with a physical ID card.");
			}
			else
			{
				//: INFO ALL_PLATFORMS
				return tr("The used Smart-eID is not accepted by the server. Please stop the remote service and use another Smart-eID or contact the service provider.");
			}
		}

		//: INFO ALL_PLATFORMS
		return tr("The used ID card is not accepted by the server. Please remove the ID card from your device or card reader and use a Smart-eID or contact the service provider.");
	}

	return QString();
}
