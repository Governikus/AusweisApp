/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ChangePinModel.h"

#include "ReaderManager.h"

using namespace governikus;


void ChangePinModel::resetChangePinContext(const QSharedPointer<ChangePinContext>& pContext)
{
	mContext = pContext;
	WorkflowModel::resetWorkflowContext(pContext);

	if (mContext)
	{
		connect(mContext.data(), &ChangePinContext::fireSuccessMessageChanged, this, &WorkflowModel::fireResultChanged);
		connect(mContext.data(), &ChangePinContext::firePaceResultUpdated, this, &ChangePinModel::onPaceResultUpdated);

		Q_EMIT fireNewContextSet();
	}
}


QString ChangePinModel::getResultString() const
{
	if (!mContext)
	{
		return QString();
	}

	return isError() ? WorkflowModel::getResultString() : mContext->getSuccessMessage();
}


void ChangePinModel::onPaceResultUpdated()
{
	if (mContext->getLastPaceResult() == CardReturnCode::OK_PUK)
	{
		Q_EMIT fireOnPinUnlocked();
	}
}
