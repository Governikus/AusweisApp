/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
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

		Q_EMIT fireSupportedPluginTypesChanged();
	}
}


void ChangePinModel::startWorkflow(bool pRequestTransportPin, bool pActivateUi, bool pOnlyCheckPin)
{
	Q_EMIT fireStartWorkflow(ChangePinController::createWorkflowRequest(pRequestTransportPin, pActivateUi, pOnlyCheckPin));
}


QString ChangePinModel::getResultString() const
{
	if (!mContext)
	{
		return QString();
	}

	return isError() ? WorkflowModel::getResultString() : mContext->getSuccessMessage();
}


GAnimation ChangePinModel::getStatusCodeAnimation() const
{
	if (isError())
	{
		return WorkflowModel::getStatusCodeAnimation();
	}

	return GAnimation::CHANGEPIN_SUCCESS;
}


bool ChangePinModel::isRequestTransportPin() const
{
	if (!mContext)
	{
		return false;
	}
	return mContext->isRequestTransportPin();
}


bool ChangePinModel::isOnlyCheckPin() const
{
	if (!mContext)
	{
		return false;
	}
	return mContext->isOnlyCheckPin();
}
