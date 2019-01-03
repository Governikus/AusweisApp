/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowModel.h"

#include "AppSettings.h"
#include "FuncUtils.h"
#include "GeneralSettings.h"
#include "ReaderConfiguration.h"
#include "ReaderManager.h"


using namespace governikus;


WorkflowModel::WorkflowModel(QObject* pParent)
	: QObject(pParent)
	, mContext()
{
	onReaderManagerSignal();
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireCardInserted, this, &WorkflowModel::onReaderManagerSignal);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireCardRemoved, this, &WorkflowModel::onReaderManagerSignal);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded, this, &WorkflowModel::onReaderManagerSignal);
	connect(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderRemoved, this, &WorkflowModel::onReaderManagerSignal);
}


WorkflowModel::~WorkflowModel()
{
}


void WorkflowModel::resetContext(const QSharedPointer<WorkflowContext>& pContext)
{
	mContext = pContext;
	if (mContext)
	{
		connect(mContext.data(), &WorkflowContext::fireStateChanged, this, &WorkflowModel::fireCurrentStateChanged);
		connect(mContext.data(), &WorkflowContext::fireResultChanged, this, &WorkflowModel::fireResultChanged);
		connect(mContext.data(), &WorkflowContext::fireReaderPlugInTypesChanged, this, &WorkflowModel::fireReaderPlugInTypeChanged);
		connect(mContext.data(), &WorkflowContext::fireCardConnectionChanged, this, &WorkflowModel::fireIsBasicReaderChanged);
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


void WorkflowModel::startWorkflow()
{
	Q_EMIT fireStartWorkflow();
}


void WorkflowModel::cancelWorkflow()
{
	if (mContext)
	{
		Q_EMIT mContext->fireCancelWorkflow();
	}
}


void WorkflowModel::cancelWorkflowOnPinBlocked()
{
	if (mContext)
	{
		mContext->setStatus(GlobalStatus::Code::Workflow_Pin_Blocked_And_Puk_Objectionable);
		Q_EMIT mContext->fireCancelWorkflow();
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


bool WorkflowModel::isBasicReader()
{
	if (mContext && mContext->getCardConnection())
	{
		return mContext->getCardConnection()->getReaderInfo().isBasicReader();
	}

	return true;
}


void WorkflowModel::setInitialPluginType()
{
	const GeneralSettings& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

	const QString& lastReaderPluginTypeString = settings.getLastReaderPluginType();
	const auto& lastReaderPluginType = Enum<ReaderManagerPlugInType>::fromString(lastReaderPluginTypeString, ReaderManagerPlugInType::UNKNOWN);

	if (lastReaderPluginType == ReaderManagerPlugInType::UNKNOWN)
	{
#if defined(Q_OS_ANDROID)
		setReaderPlugInType(ReaderManagerPlugInType::NFC);
#elif defined(Q_OS_IOS)
		setReaderPlugInType(ReaderManagerPlugInType::BLUETOOTH);
#else
		setReaderPlugInType(ReaderManagerPlugInType::PCSC);
#endif
		return;
	}
	setReaderPlugInType(lastReaderPluginType);
}


bool WorkflowModel::selectedReaderHasCard() const
{
	if (mContext && !mContext->getReaderName().isEmpty())
	{
		return Env::getSingleton<ReaderManager>()->getReaderInfo(mContext->getReaderName()).hasCard();
	}
	return false;
}


void WorkflowModel::onReaderManagerSignal()
{
	QString newReaderImage;
	const auto& readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter::UniqueReaderTypes);
	const auto& readersWithNPA = filter<ReaderInfo>([](const ReaderInfo& i){return i.hasEidCard();}, readerInfos);
	if (readersWithNPA.size() == 1)
	{
		newReaderImage = readersWithNPA.at(0).getReaderConfigurationInfo().getIconWithNPA()->lookupPath();
	}
	else if (readerInfos.size() == 1)
	{
		newReaderImage = readerInfos.at(0).getReaderConfigurationInfo().getIcon()->lookupPath();
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
