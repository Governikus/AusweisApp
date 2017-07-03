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
	const QSharedPointer<StartPaosResponse>& startPaosResponse = getContext()->getStartPaosResponse();
	Q_ASSERT(startPaosResponse);

	const Result& result = startPaosResponse->getResult();
	if (result.isOk())
	{
		Q_EMIT fireSuccess();
		return;
	}

	qDebug() << "Processing server result:" << result.getMajorString() << result.getMinorString() << result.getMessage();
	setStatus(result.toStatus());
	Q_EMIT fireError();

}
