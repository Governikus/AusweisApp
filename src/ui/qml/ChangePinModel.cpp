/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ChangePinModel.h"

#include "controller/ChangePinController.h"


using namespace governikus;


void ChangePinModel::resetChangePinContext(const QSharedPointer<ChangePinContext>& pContext)
{
	mContext = pContext;
	WorkflowModel::resetWorkflowContext(pContext);

	if (mContext)
	{
		connect(mContext.data(), &ChangePinContext::fireSuccessMessageChanged, this, &WorkflowModel::fireResultChanged);
		connect(mContext.data(), &ChangePinContext::firePaceResultUpdated, this, &ChangePinModel::onPaceResultUpdated);

		Q_EMIT fireSupportedPlugInTypesChanged();
		Q_EMIT fireIsCurrentSmartCardAllowedChanged();
	}
}


void ChangePinModel::startWorkflow(bool pRequestTransportPin, bool pActivateUi)
{
	Q_EMIT fireStartWorkflow(ChangePinController::createWorkflowRequest(pRequestTransportPin, pActivateUi));
}


QString ChangePinModel::getResultString() const
{
	if (!mContext)
	{
		return QString();
	}

	return isError() ? WorkflowModel::getResultString() : mContext->getSuccessMessage();
}


QList<ReaderManagerPlugInType> ChangePinModel::getSupportedReaderPlugInTypes() const
{
	auto plugins = WorkflowModel::getSupportedReaderPlugInTypes();
	if (mContext && mContext->isRequestTransportPin())
	{
		plugins.removeOne(ReaderManagerPlugInType::SMART);
	}
	return plugins;
}


bool ChangePinModel::isRequestTransportPin() const
{
	if (!mContext)
	{
		return false;
	}
	return mContext->isRequestTransportPin();
}


void ChangePinModel::onPaceResultUpdated()
{
	if (mContext->getLastPaceResult() == CardReturnCode::OK_PUK)
	{
		Q_EMIT fireOnPinUnlocked();
		return;
	}

	Q_EMIT fireOnPasswordUsed();
}
