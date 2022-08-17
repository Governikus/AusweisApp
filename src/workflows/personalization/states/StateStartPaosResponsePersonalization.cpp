/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "StateStartPaosResponsePersonalization.h"

#include "context/PersonalizationContext.h"

Q_DECLARE_LOGGING_CATEGORY(secure)

using namespace governikus;


StateStartPaosResponsePersonalization::StateStartPaosResponsePersonalization(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateStartPaosResponsePersonalization::run()
{
	const QSharedPointer<StartPaosResponse>& startPaosResponse = getContext()->getStartPaosResponse();
	if (!startPaosResponse)
	{
		Q_EMIT fireAbort();
		return;
	}

	const ECardApiResult& result = startPaosResponse->getResult();
	if (result.isOk())
	{
		const auto& context = qobject_cast<PersonalizationContext*>(getContext());
		const auto& blockingCode = startPaosResponse->getBlockingCode();
		if (context && !blockingCode.isEmpty())
		{
			context->setBlockingCode(blockingCode);
			context->setRemainingAttempts(startPaosResponse->getRemainingAttempts());
			context->setRemainingDays(startPaosResponse->getRemainingDays());
			qDebug() << "Personalization was successful.";
			qCDebug(secure) << context->getRemainingAttempts() << "attempt(s) left in the next" << context->getRemainingDays() << "day(s)";
		}

		Q_EMIT fireContinue();
		return;
	}

	qDebug() << "Processing server result:" << result.getMajorString() << result.getMinorString() << result.getMessage();
	updateStartPaosResult(result);
	Q_EMIT fireAbort();
}
