/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateStartPaosResponse.h"

using namespace governikus;

StateStartPaosResponse::StateStartPaosResponse(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateStartPaosResponse::run()
{
	const QSharedPointer<StartPaosResponse>& startPaosResponse = getContext()->getStartPaosResponse();
	Q_ASSERT(startPaosResponse);

	const ECardApiResult& result = startPaosResponse->getResult();
	if (result.isOk())
	{
		Q_EMIT fireContinue();
		return;
	}

	qDebug() << "Processing server result:" << result.getMajorString() << result.getMinorString() << result.getMessage();
	updateStartPaosResult(result);
	Q_EMIT fireAbort();
}
