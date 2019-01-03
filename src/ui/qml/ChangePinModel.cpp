/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ChangePinModel.h"

#include "Env.h"
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
