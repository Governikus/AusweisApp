/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateInitializePersonalization.h"

#include "ReaderManager.h"
#include "SmartManager.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


StateInitializePersonalization::StateInitializePersonalization(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateInitializePersonalization::run()
{
	const auto& context = getContext();
	Q_ASSERT(context);

	//: LABEL ANDROID IOS
	context->setProgress(20, tr("Personalizing the Smart-eID"));

	const auto& pin = context->getPin();
	const auto& challenge = context->getChallenge();
	const auto func = [pin, challenge] {
				const auto& smartManager = SmartManager::get(true);
				const auto& result = smartManager->initializePersonalization(challenge, pin);
				if (result.mResult != EidServiceResult::SUCCESS)
				{
					smartManager->releaseConnection();
				}

				return QVariant::fromValue(result);
			};

	mConnections += Env::getSingleton<ReaderManager>()->callExecuteCommand(func, this, &StateInitializePersonalization::onCommandDone);
}


void StateInitializePersonalization::onCommandDone(const QVariant& pResult)
{
	const auto [result, preparePersonalizationData] = pResult.value<InitializeResult>();

	if (result == EidServiceResult::SUCCESS)
	{
		getContext()->setPreparePersonalizationData(QString::fromStdString(preparePersonalizationData));
		Q_EMIT fireContinue();
		return;
	}

	qCWarning(card_smart) << "Initialization of personalization failed";
	updateStatus(GlobalStatus::Code::Workflow_Smart_eID_PrePersonalization_Failed);
	Q_EMIT fireAbort(FailureCode::Reason::Initialize_Personalization_Failed);
}
