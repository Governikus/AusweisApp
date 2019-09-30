/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ChangePinModel.h"

#include "ReaderManager.h"
#include "SingletonHelper.h"

using namespace governikus;


defineSingleton(ChangePinModel)


void ChangePinModel::resetContext(const QSharedPointer<ChangePinContext>& pContext)
{
	mContext = pContext;
	WorkflowModel::resetContext(pContext);

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


ChangePinModel& ChangePinModel::getInstance()
{
	return *Instance;
}


void ChangePinModel::onPaceResultUpdated()
{
	if (mContext->getLastPaceResult() == CardReturnCode::OK_PUK)
	{
		Q_EMIT fireOnPinUnlocked();
	}
}
