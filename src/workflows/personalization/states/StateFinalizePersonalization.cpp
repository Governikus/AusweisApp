/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateFinalizePersonalization.h"

#include "ReaderManager.h"
#include "SmartManager.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


StateFinalizePersonalization::StateFinalizePersonalization(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateFinalizePersonalization::run()
{
	const auto& context = getContext();
	Q_ASSERT(context);

	//: LABEL ANDROID IOS
	context->setProgress(90, tr("Finalizing the Smart-eID"));

	const int finalizeStatus = context->getFinalizeStatus();
	const auto func = [finalizeStatus] {
				const auto& result = QVariant::fromValue(SmartManager::get()->finalizePersonalization(finalizeStatus));
				SmartManager::releaseConnection();
				return result;
			};

	*this << Env::getSingleton<ReaderManager>()->callExecuteCommand(func, this, &StateFinalizePersonalization::onCommandDone);
}


void StateFinalizePersonalization::onCommandDone(const QVariant& pResult)
{
	const auto& context = getContext();
	Q_ASSERT(context);

	if (context->getStatus().isError())
	{
		Q_ASSERT(context->getFailureCode().has_value());
		Q_EMIT firePropagateAbort();
		return;
	}

	const auto [result, initPin] = pResult.value<PersonalizationResult>();
	if (result != EidServiceResult::SUCCESS)
	{
		qCWarning(card_smart) << "Finalization of personalization failed";
		updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Personalization_Failed);
		Q_EMIT fireAbort(FailureCode::Reason::Finalize_Personalization_Failed);
		return;
	}

	// ensure to avoid access rights in StateEstablishPaceChannel
	context->setDidAuthenticateEac1(nullptr);

	context->setPin(QString::fromStdString(initPin));
	Q_EMIT fireContinue();
}
