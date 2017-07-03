/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StateStartPaosResponse.h"

using namespace governikus;

StateStartPaosResponse::StateStartPaosResponse(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateStartPaosResponse::run()
{
	Q_ASSERT(getContext()->getStartPaosResponse());
	if (getContext()->getStartPaosResponse()->getResult().isOk())
	{
		Q_EMIT fireSuccess();
	}
	else
	{
		// we override our result with the one sent from server
		Result result = getContext()->getStartPaosResponse()->getResult();
		qDebug() << "Set server result to model:" << result.getMajorString() << result.getMinorString() << result.getMessage();
		getContext()->setResult(result);
		Q_EMIT fireError();
	}
}
