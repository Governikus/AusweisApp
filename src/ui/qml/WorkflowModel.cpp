/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowModel.h"

#include "AppSettings.h"
#include "context/AuthContext.h"
#include "Email.h"
#include "Env.h"
#include "FuncUtils.h"
#include "GeneralSettings.h"
#include "PinResetInformationModel.h"
#include "ReaderConfiguration.h"
#include "ReaderManager.h"

#include <QDesktopServices>


using namespace governikus;


WorkflowModel::WorkflowModel(QObject* pParent)
	: QObject(pParent)
	, mContext()
{
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireCardInserted, this, &WorkflowModel::onReaderManagerSignal);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireCardRemoved, this, &WorkflowModel::onReaderManagerSignal);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded, this, &WorkflowModel::onReaderManagerSignal);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderRemoved, this, &WorkflowModel::onReaderManagerSignal);

	QMetaObject::invokeMethod(this, &WorkflowModel::onReaderManagerSignal, Qt::QueuedConnection);
}


WorkflowModel::~WorkflowModel()
{
}


void WorkflowModel::resetWorkflowContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &WorkflowModel::fireCurrentStateChanged);
		connect(mContext.data(), &WorkflowContext::fireResultChanged, this, &WorkflowModel::fireResultChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderPlugInTypesChanged, this, &WorkflowModel::fireReaderPlugInTypeChanged);
		connect(mContext.data(), &WorkflowContext::fireCardConnectionChanged, this, &WorkflowModel::fireIsBasicReaderChanged);
		connect(mContext.data(), &WorkflowContext::fireNextWorkflowPending, this, &WorkflowModel::fireNextWorkflowPendingChanged);
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

	GeneralSettings& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	settings.setLastReaderPluginType(getEnumName(pReaderPlugInType));
	settings.save();
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


void WorkflowModel::cancelWorkflowToChangePin()
{
	if (mContext)
	{
		mContext->setStatus(GlobalStatus::Code::Workflow_Cancellation_By_User);
		Q_EMIT mContext->fireCancelWorkflow();
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


void WorkflowModel::setInitialPluginType()
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	const GeneralSettings& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

	const QString& lastReaderPluginTypeString = settings.getLastReaderPluginType();
	const auto& lastReaderPluginType = Enum<ReaderManagerPlugInType>::fromString(lastReaderPluginTypeString, ReaderManagerPlugInType::UNKNOWN);

	if (lastReaderPluginType == ReaderManagerPlugInType::UNKNOWN)
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
	mContext->setReaderPlugInTypes({ReaderManagerPlugInType::PCSC, ReaderManagerPlugInType::REMOTE});
#endif
}


bool WorkflowModel::selectedReaderHasCard() const
{
	if (mContext && !mContext->getReaderName().isEmpty())
	{
		return Env::getSingleton<ReaderManager>()->getReaderInfo(mContext->getReaderName()).hasCard();
	}
	return false;
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


void WorkflowModel::onReaderManagerSignal()
{
	QString newReaderImage;
	const auto& readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter::UniqueReaderTypes);
	const auto& readersWithNPA = filter<ReaderInfo>([](const ReaderInfo& i){return i.hasEidCard();}, readerInfos);
	if (readersWithNPA.size() == 1)
	{
		newReaderImage = readersWithNPA.at(0).getReaderConfigurationInfo().getIconWithNPA()->lookupUrl().toString();
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
