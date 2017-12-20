/*!
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ChangePinModel.h"

#include "context/ChangePinContext.h"
#include "ReaderManager.h"


using namespace governikus;


ChangePinModel::ChangePinModel(QObject* pParent)
	: WorkflowModel(pParent)
{
}


ChangePinModel::~ChangePinModel()
{
}


void ChangePinModel::resetContext(const QSharedPointer<ChangePinContext>& pContext)
{
	mContext = pContext;
	WorkflowModel::resetContext(pContext);

	if (mContext)
	{
		connect(mContext.data(), &ChangePinContext::fireSuccessMessageChanged, this, &WorkflowModel::fireResultChanged);

		Q_EMIT fireResultChanged();
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
