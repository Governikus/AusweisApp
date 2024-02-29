/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateStartPaosResponse.h"

#include "SurveyModel.h"

using namespace governikus;


StateStartPaosResponse::StateStartPaosResponse(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateStartPaosResponse::run()
{
	Q_ASSERT(getContext()->getPin().isEmpty() && "PACE passwords must be cleared as soon as possible.");
	Q_ASSERT(getContext()->getCan().isEmpty() && "PACE passwords must be cleared as soon as possible.");
	Q_ASSERT(getContext()->getPuk().isEmpty() && "PACE passwords must be cleared as soon as possible.");

	const QSharedPointer<StartPaosResponse>& startPaosResponse = getContext()->getStartPaosResponse();
	if (!startPaosResponse)
	{
		updateStatus(GlobalStatus::Code::Workflow_Start_Paos_Response_Missing);
		Q_EMIT fireAbort(FailureCode::Reason::Start_Paos_Response_Missing);
		return;
	}

	const ECardApiResult& result = startPaosResponse->getResult();
	Env::getSingleton<SurveyModel>()->setAuthWasSuccessful(result.isOk());
	if (result.isOk())
	{
		Q_EMIT fireContinue();
		return;
	}

	qDebug() << "Processing server result:" << result.getMajorString() << result.getMinorString() << result.getMessage();
	updateStartPaosResult(result);
	Q_EMIT fireAbort(FailureCode::Reason::Start_Paos_Response_Error);
}
