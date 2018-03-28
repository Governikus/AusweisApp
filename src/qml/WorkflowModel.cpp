/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowModel.h"

#include "AppSettings.h"
#include "context/AuthContext.h"
#include "GeneralSettings.h"

using namespace governikus;


WorkflowModel::WorkflowModel(QObject* pParent)
	: QObject(pParent)
	, mContext()
{
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


QString WorkflowModel::getReaderPlugInType() const
{
	if (mContext && !mContext->getReaderPlugInTypes().isEmpty())
	{
		return getEnumName(mContext->getReaderPlugInTypes().at(0));
	}

	return QString();
}


void WorkflowModel::setReaderPlugInType(const QString& pReaderPlugInType)
{
	setReaderPlugInType(Enum<ReaderManagerPlugInType>::fromString(pReaderPlugInType, ReaderManagerPlugInType::UNKNOWN));
}


void WorkflowModel::setReaderPlugInType(const ReaderManagerPlugInType pReaderPlugInType)
{
	if (!mContext)
	{
		return;
	}
	mContext->setReaderPlugInTypes({pReaderPlugInType});

	GeneralSettings& settings = AppSettings::getInstance().getGeneralSettings();
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
	const GeneralSettings& settings = AppSettings::getInstance().getGeneralSettings();

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
