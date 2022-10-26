/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
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
			qRegisterMetaType<QVector<int>>("QVector<ReaderManagerPlugInType>");
		})

WorkflowModel::WorkflowModel(QObject* pParent)
	: QObject(pParent)
	, mContext()
#if defined(Q_OS_IOS)
	, mRemoteScanWasRunning(false)
#endif
{
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireCardInserted, this, &WorkflowModel::onReaderManagerSignal);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireCardRemoved, this, &WorkflowModel::onReaderManagerSignal);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded, this, &WorkflowModel::onReaderManagerSignal);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderRemoved, this, &WorkflowModel::onReaderManagerSignal);
	connect(Env::getSingleton<SmartModel>(), &SmartModel::fireSmartStateChanged, this, &WorkflowModel::fireIsSmartCardAllowedChanged);

	connect(Env::getSingleton<ApplicationModel>(), &ApplicationModel::fireApplicationStateChanged, this, &WorkflowModel::onApplicationStateChanged);

	connect(&Env::getSingleton<AppSettings>()->getGeneralSettings(), &GeneralSettings::fireDeveloperOptionsChanged, this, &WorkflowModel::fireSupportedPlugInTypesChanged);

	QMetaObject::invokeMethod(this, &WorkflowModel::onReaderManagerSignal, Qt::QueuedConnection);
}


void WorkflowModel::resetWorkflowContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &WorkflowModel::fireCurrentStateChanged);
		connect(mContext.data(), &WorkflowContext::fireResultChanged, this, &WorkflowModel::fireResultChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderPlugInTypesChanged, this, &WorkflowModel::fireReaderPlugInTypeChanged);
		connect(mContext.data(), &WorkflowContext::fireCardConnectionChanged, this, &WorkflowModel::fireSelectedReaderChanged);
		connect(mContext.data(), &WorkflowContext::fireIsSmartCardAllowedChanged, this, &WorkflowModel::fireIsSmartCardAllowedChanged);
		connect(mContext.data(), &WorkflowContext::fireNextWorkflowPending, this, &WorkflowModel::fireNextWorkflowPendingChanged);
		connect(mContext.data(), &WorkflowContext::fireRemoveCardFeedbackChanged, this, &WorkflowModel::fireRemoveCardFeedbackChanged);
	}

	/*
	 * Only this state change is emitted when the context is reset, i.e. after the end of the workflow
	 */
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


void WorkflowModel::insertCard(ReaderManagerPlugInType pType)
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


void WorkflowModel::startScanIfNecessary()
{
	if (mContext)
	{
		Q_EMIT mContext->fireReaderPlugInTypesChanged();
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


bool WorkflowModel::isSmartCardAllowed() const
{
	if (!mContext)
	{
		return false;
	}

	MobileEidType mobileEidType = Env::getSingleton<SmartModel>()->getMobileEidType();
	if (mobileEidType == MobileEidType::UNKNOWN)
	{
		return false;
	}

	const auto& acceptedEidTypes = mContext->getAcceptedEidTypes();
	switch (mobileEidType)
	{
		case MobileEidType::SE_CERTIFIED:
			return acceptedEidTypes.contains(AcceptedEidType::SE_CERTIFIED);

		case MobileEidType::SE_ENDORSED:
			return acceptedEidTypes.contains(AcceptedEidType::SE_ENDORSED);

		case MobileEidType::HW_KEYSTORE:
			return acceptedEidTypes.contains(AcceptedEidType::HW_KEYSTORE);

		case MobileEidType::UNKNOWN:
			return false;
	}
	Q_UNREACHABLE();
}


bool WorkflowModel::isSmartSupported() const
{
	return getSupportedReaderPlugInTypes().contains(ReaderManagerPlugInType::SMART);
}


QVector<ReaderManagerPlugInType> WorkflowModel::getSupportedReaderPlugInTypes() const
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


QString WorkflowModel::getReaderImage() const
{
	return mReaderImage;
}


QString WorkflowModel::getStatusHintText() const
{
	switch (getStatusCode())
	{
		case GlobalStatus::Code::Card_Puk_Blocked:
			return Env::getSingleton<PinResetInformationModel>()->getRequestNewPinHint();

		case GlobalStatus::Code::Card_Pin_Deactivated:
			return Env::getSingleton<PinResetInformationModel>()->getActivateOnlineFunctionHint();

		default:
			return QString();
	}
}


QString WorkflowModel::getStatusHintActionText() const
{
	switch (getStatusCode())
	{
		case GlobalStatus::Code::Card_Puk_Blocked:
		case GlobalStatus::Code::Card_Pin_Deactivated:
			return Env::getSingleton<PinResetInformationModel>()->getPinResetActionText();

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

	return tr("AusweisApp2 error report - %1").arg(mContext->getStatus().toErrorDescription());
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
		mRemoteScanWasRunning = Env::getSingleton<ReaderManager>()->isScanRunning(ReaderManagerPlugInType::REMOTE_IFD);
		if (mRemoteScanWasRunning)
		{
			Env::getSingleton<ReaderManager>()->stopScan(ReaderManagerPlugInType::REMOTE_IFD);
		}
	}
#else
	Q_UNUSED(pIsAppInForeground)
#endif
}


void WorkflowModel::onReaderManagerSignal()
{
	QString newReaderImage;
	const auto& readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter::UniqueReaderTypes);
	const auto& readersWithEid = filter<ReaderInfo>([](const ReaderInfo& i){return i.hasEid();}, readerInfos);
	if (readersWithEid.size() == 1)
	{
		newReaderImage = readersWithEid.at(0).getReaderConfigurationInfo().getIconWithNPA()->lookupUrl().toString();
	}
	else if (readerInfos.size() == 1)
	{
		newReaderImage = readerInfos.at(0).getReaderConfigurationInfo().getIcon()->lookupUrl().toString();
	}
	else if (readerInfos.size() > 1)
	{
		newReaderImage = ReaderConfiguration::getMultipleReaderIconPath().replace(QLatin1Char(':'), QLatin1String("qrc://"));
	}
	else
	{
		newReaderImage = ReaderConfiguration::getNoReaderFoundIconPath().replace(QLatin1Char(':'), QLatin1String("qrc://"));
	}

	if (newReaderImage != mReaderImage)
	{
		mReaderImage = newReaderImage;
		Q_EMIT fireReaderImageChanged();
	}
}
