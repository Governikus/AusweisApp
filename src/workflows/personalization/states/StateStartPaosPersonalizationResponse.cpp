/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateStartPaosPersonalizationResponse.h"


Q_DECLARE_LOGGING_CATEGORY(secure)


using namespace governikus;


StateStartPaosPersonalizationResponse::StateStartPaosPersonalizationResponse(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateStartPaosPersonalizationResponse::run()
{
	const QSharedPointer<StartPaosResponse>& startPaosResponse = getContext()->getStartPaosResponse();
	if (!startPaosResponse)
	{
		updateStatus(GlobalStatus::Code::Workflow_Start_Paos_Response_Missing);
		Q_EMIT fireAbort(FailureCode::Reason::Start_Paos_Response_Personalization_Empty);
		return;
	}

	const auto& remainingAttempts = startPaosResponse->getRemainingAttempts();
	const auto& remainingDays = startPaosResponse->getRemainingDays();
	qCDebug(secure) << remainingAttempts << "attempt(s) left in the next" << remainingDays << "day(s)";

	const auto& context = getContext();
	if (context)
	{
		context->setFinalizeStatus(startPaosResponse->getStatusCode());
		context->setRemainingAttempts(remainingAttempts);
		context->setRemainingDays(remainingDays);
	}

	const ECardApiResult& result = startPaosResponse->getResult();
	if (result.isOk())
	{
		const auto& blockingCode = startPaosResponse->getBlockingCode();
		if (context && !blockingCode.isEmpty())
		{
			context->setBlockingCode(blockingCode);
			qDebug() << "Personalization was successful.";
		}

		Q_EMIT fireContinue();
		return;
	}

	qDebug() << "Processing server result:" << result.getMajorString() << result.getMinorString() << result.getMessage();
	if (result.getMinor() == ECardApiResult::Minor::AL_No_Permission && remainingDays < 0)
	{
		const auto& dateString = context ? context->getRestrictionDate() : QString();
		updateStatus({GlobalStatus::Code::Workflow_Smart_eID_Personalization_Denied, {GlobalStatus::ExternalInformation::PERSONALIZATION_RESTRICTION_DATE, dateString}
				});
	}
	updateStartPaosResult(result);

	Q_EMIT fireAbort(FailureCode::Reason::Start_Paos_Response_Personalization_Invalid);
}
